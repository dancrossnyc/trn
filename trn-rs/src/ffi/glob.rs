use std::ops::BitOrAssign;
use std::ops::Not;

#[derive(Debug, Eq, PartialEq)]
pub enum Error {
    Syntax,
    Range,
    Size,
    Instr,
    Fault,
}

pub type Result<T> = std::result::Result<T, Error>;

#[derive(Debug)]
#[repr(C)]
pub enum OpCode {
    Match = 0,
    Any = 1,
    Star = 2,
    Lit = 3,
    Class = 4,
}

impl From<OpCode> for u16 {
    fn from(op: OpCode) -> u16 {
        op as u16
    }
}

#[derive(Debug)]
pub enum Instruction {
    Match,
    Any,
    Star,
    Lit(u8),
    Class(u16),
}

impl TryFrom<u16> for Instruction {
    type Error = Error;
    fn try_from(v: u16) -> Result<Instruction> {
        match v & 0x000F {
            0 => Ok(Self::Match),
            1 => Ok(Self::Any),
            2 => Ok(Self::Star),
            3 => {
                let c = v >> 8;
                Ok(Self::Lit(c as u8))
            }
            4 => {
                let k = v >> 4;
                Ok(Self::Class(k))
            }
            _ => Err(Error::Instr),
        }
    }
}

#[inline(always)]
fn divrem(n: usize, d: usize) -> (usize, usize) {
    (n / d, n % d)
}

#[derive(Debug)]
pub struct CharClass([u16; 16]);

impl CharClass {
    fn new() -> CharClass {
        Self([0; 16])
    }

    fn contains(&self, c: u8) -> bool {
        let (index, offset) = divrem(c as usize, 16);
        self.0[index] & 1 << offset != 0
    }
}

impl BitOrAssign<u8> for CharClass {
    fn bitor_assign(&mut self, c: u8) {
        let (index, offset) = divrem(c as usize, 16);
        self.0[index] |= 1 << offset;
    }
}

impl Not for CharClass {
    type Output = Self;
    fn not(self) -> Self::Output {
        CharClass(self.0.map(|w| !w))
    }
}

struct StateSet {
    nstates: usize,
    states: Box<[u32]>,
}

impl StateSet {
    pub fn new(nstates: usize) -> StateSet {
        let nwords = nstates.div_ceil(32);
        let states = vec![0; nwords].into_boxed_slice();
        StateSet { nstates, states }
    }

    pub fn is_empty(&self) -> bool {
        self.states.iter().all(|&w| w == 0)
    }

    pub fn len(&self) -> usize {
        self.nstates
    }

    pub fn insert(&mut self, state: usize) {
        assert!(state < self.len());
        let (index, offset) = divrem(state, 32);
        self.states[index] |= 1 << offset;
    }

    pub fn contains(&self, state: usize) -> bool {
        assert!(state < self.len());
        let (index, offset) = divrem(state, 32);
        self.states[index] & 1 << offset != 0
    }

    pub fn iter(&self) -> StateSetIterator<'_> {
        let states = self;
        let pos = 0;
        StateSetIterator { states, pos }
    }

    pub fn clear(&mut self) {
        for w in self.states.iter_mut() {
            *w = 0;
        }
    }
}

struct StateSetIterator<'a> {
    states: &'a StateSet,
    pos: usize,
}

impl<'a> Iterator for StateSetIterator<'a> {
    type Item = usize;
    fn next(&mut self) -> Option<usize> {
        while self.pos < self.states.len() {
            let pos = self.pos;
            self.pos += 1;
            if self.states.contains(pos) {
                return Some(pos);
            }
        }
        None
    }
}

#[derive(Debug)]
pub struct Program {
    prog: Vec<u16>,
    ccs: Vec<CharClass>,
}

impl Program {
    fn len(&self) -> usize {
        self.prog.len()
    }

    fn is_empty(&self) -> bool {
        self.prog.is_empty()
    }

    fn instruction(&self, pc: usize) -> Result<Instruction> {
        if pc >= self.prog.len() {
            return Err(Error::Fault);
        }
        Instruction::try_from(self.prog[pc])
    }

    fn char_class(&self, k: usize) -> Result<&CharClass> {
        if k >= self.ccs.len() {
            return Err(Error::Fault);
        }
        Ok(&self.ccs[k])
    }
}

pub fn compile(source: &[u8]) -> Result<Program> {
    let mut prog = Vec::with_capacity(source.len());
    let mut ccs = Vec::new();
    let mut pattern = source.iter().copied().peekable();
    fn lit(c: u8) -> u16 {
        u16::from(c) << 8 | u16::from(OpCode::Lit)
    }
    while let Some(c) = pattern.next() {
        let inst = match c {
            b'?' => OpCode::Any.into(),
            b'*' => {
                while pattern.next_if_eq(&b'*').is_some() {}
                OpCode::Star.into()
            }
            b'[' => {
                let cc = parse_class(&mut pattern)?;
                let n = ccs.len();
                if n >= 4096 {
                    return Err(Error::Size);
                }
                let k = n as u16;
                ccs.push(cc);
                k << 4 | u16::from(OpCode::Class)
            }
            b'\\' => {
                let Some(c) = pattern.next() else {
                    return Err(Error::Syntax);
                };
                lit(c)
            }
            _ => lit(c),
        };
        prog.push(inst);
    }
    prog.push(OpCode::Match.into());

    Ok(Program { prog, ccs })
}

fn parse_class(pattern: &mut std::iter::Peekable<impl Iterator<Item = u8>>) -> Result<CharClass> {
    let negate = pattern.next_if_eq(&b'^').is_some();
    let mut cc = CharClass::new();
    let mut last = None;
    while let Some(c) = pattern.next() {
        match c {
            b']' | b'-' if last.is_none() => {
                cc |= c;
                last = Some(c);
            }
            b']' => break,
            b'-' => {
                let Some(lo) = last else {
                    return Err(Error::Range);
                };
                let Some(hi) = pattern.next() else {
                    return Err(Error::Range);
                };
                for v in lo..=hi {
                    cc |= v;
                }
                last = Some(hi);
            }
            _ => {
                cc |= c;
                last = Some(c);
            }
        }
    }
    Ok(if negate { !cc } else { cc })
}

/// Simulates execution of the compiled Thompson NFA in `prog`
/// against the input data in `s`.
pub fn execute(prog: &Program, s: &[u8]) -> Result<bool> {
    fn step(prog: &Program, pc: usize, next: &mut StateSet) -> Result<()> {
        if let Instruction::Star = prog.instruction(pc)? {
            step(prog, pc + 1, next)?;
        }
        next.insert(pc);
        Ok(())
    }
    if prog.is_empty() {
        return Ok(false);
    }
    let mut state0 = StateSet::new(prog.len());
    let mut state1 = StateSet::new(prog.len());
    let mut curr = &mut state0;
    let mut next = &mut state1;
    step(prog, 0, curr)?;
    for &c in s {
        next.clear();
        if curr.is_empty() {
            return Ok(false);
        }
        for pc in curr.iter() {
            let instr = prog.instruction(pc)?;
            match instr {
                Instruction::Any => step(prog, pc + 1, next)?,
                Instruction::Star => step(prog, pc, next)?,
                Instruction::Lit(lc) if lc == c => step(prog, pc + 1, next)?,
                Instruction::Class(k) => {
                    let cc = prog.char_class(k as usize)?;
                    if cc.contains(c) {
                        step(prog, pc + 1, next)?;
                    }
                }
                Instruction::Match | Instruction::Lit(_) => {}
            }
        }
        (curr, next) = (next, curr);
    }
    Ok(curr.contains(prog.len() - 1))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn glob_compile() {
        let obj = compile(b"abc?").expect("parsed 'abc?'");
        assert_eq!(obj.prog, [0x6103, 0x6203, 0x6303, 0x1, 0x0]);
    }

    #[test]
    fn glob_compile_class() {
        let obj = compile(b"a[bd]c[a-m]?*").expect("parsed 'a[bd]c[a-m]?*'");
        assert_eq!(obj.prog, [0x6103, 0x0004, 0x6303, 0x0014, 0x1, 0x2, 0x0]);
        assert_eq!(obj.ccs.len(), 2);
        #[rustfmt::skip]
        assert_eq!(
            obj.ccs[0].0,
            [
                0, 0, 0, 0,
                0, 0, 0b0000_0000_0001_0100, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            ]
        );
        #[rustfmt::skip]
        assert_eq!(
            obj.ccs[1].0,
            [
                0, 0, 0, 0,
                0, 0, 0b0011_1111_1111_1110, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            ]
        );
    }

    #[test]
    fn glob_astar() {
        let obj = compile(b"a*").expect("parsed `a*`");
        assert_eq!(execute(&obj, b"a"), Ok(true));
    }

    #[test]
    fn glob_astarb() {
        let obj = compile(b"a*b").expect("parsed `a*b`");
        assert_eq!(execute(&obj, b"ab"), Ok(true));
        assert_eq!(execute(&obj, b"acb"), Ok(true));
        assert_eq!(execute(&obj, b"abc"), Ok(false));
    }

    #[test]
    fn glob_xfont() {
        let obj = compile(b"-*-*-*-*-*-*-12-*-*-*-m-*-*-*").expect("parsed glob");
        assert_eq!(
            execute(&obj, b"-adobe-courier-bold-o-normal--12-120-75-75-m-70-iso8859-1"),
            Ok(true)
        );
        assert_eq!(
            execute(&obj, b"-adobe-courier-bold-o-normal--12-120-75-75-X-70-iso8859-1"),
            Ok(false)
        );
    }
}
