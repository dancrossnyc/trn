//! Provides an FFI-compatible allocator for C.
//!
//! This replaces the elderly `safemalloc` function and friends.
//! This lets us use the Rust memory allocator from C code.

use std::alloc::{Layout, alloc_zeroed, dealloc, realloc};
use std::sync::RwLock;
use std::ffi::{CStr, c_char, c_int, c_void};

/// The alignment constant.  We allocate blocks of memory that
/// are both sized and aligned sized to multiples of `ALIGN`
/// bytes.
const ALIGN: usize = 64;

/// A wrapper around the allocator API's `system` allocator with
/// well-defined semantics around size==0, for FFI.  Initializes
/// allocated memory with zeroes.
///
/// Note: This will always return nil on a zero-sized
/// allocation.  Don't try to use this to allocate Rust ZSTs;
/// it is only suitable for FFI.
///
/// # Safety
/// It is the caller's responsibility to ensure that the result
/// is not null, to avoid violating memory safety, and to make
/// sure that the returned pointer is suitably aligned for the
/// type of value it points to.
unsafe fn rsmalloc(size: usize) -> *mut u8 {
    if size == 0 {
        return std::ptr::null_mut();
    }
    let size = size.next_multiple_of(ALIGN) + ALIGN;
    let layout = Layout::from_size_align(size, ALIGN).expect("layout makes sense");
    let ptr = unsafe { alloc_zeroed(layout) };
    if ptr.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        std::ptr::write(ptr.cast(), layout);
    }
    ptr.wrapping_add(ALIGN)
}

/// A wrapper around the Rust allocator API's `dealloc` function
/// for FFI.
///
/// If the pointer argument is nil, this is a nop.  Otherwise,
/// it must have been allocated by `rsmalloc` or `rsrealloc`.
///
/// # Safety
/// The caller must ensure that, if not nil, the pointer passed
/// to this function was allocated by `rsmalloc` or `rsrealloc`,
/// that no further references will be made to the memory the
/// pointer points to, and that the pointer has not already been
/// freed.
unsafe fn rsfree(ptr: *mut u8) {
    if ptr.is_null() {
        return;
    }
    assert!(ptr.addr().is_multiple_of(ALIGN));
    let ptr = ptr.wrapping_sub(ALIGN);
    let layout = unsafe { std::ptr::read(ptr.cast::<Layout>()) };
    unsafe {
        dealloc(ptr, layout);
    }
}

/// A wrapper around the Rust allocator API's `realloc` function
/// for FFI calls.
///
/// Note that this will always free and return NULL on a
/// reallocation of size 0, so do not try to use this to
/// allocate Rust ZSTs: it is purely for FFI.  If the source
/// pointer is nil and size is positive, then the behavior is
/// that of `rsmalloc`.
///
/// The pointer argument just have been allocated previously
/// with the Rust allocator.
///
/// If successful, returns a non-nil pointer, and the old
/// pointer is considered invalid, and must not be dereferenced.
/// If the old size was smaller than the new size, elements
/// after the old size are uninitialized.  Elements at indices
/// between the start and the minimum of the old and new sizes
/// are unchanged.
///
/// On failure, returns nil, and does not free the source
/// pointer or otherwise alter its contents.
///
/// # Safety
/// It is the caller's responsibility to ensure that the result
/// is not null, to avoid violating memory safety, and to make
/// sure that any alignment for any value the pointer is used to
/// refer to is suitably aligned.  The caller must similarly
/// take care to initialize elements beyond the old size.
unsafe fn rsrealloc(ptr: *mut u8, size: usize) -> *mut u8 {
    if size == 0 {
        unsafe {
            rsfree(ptr);
        }
        return std::ptr::null_mut();
    }
    if ptr.is_null() {
        return unsafe { rsmalloc(size) };
    }
    if !ptr.addr().is_multiple_of(ALIGN) {
        return std::ptr::null_mut();
    }
    let optr = ptr.wrapping_sub(ALIGN);
    let olayout = unsafe { std::ptr::read(optr.cast::<Layout>()) };
    let oalign = olayout.align();
    let osize = olayout.size();
    assert_eq!(oalign, ALIGN);
    assert!(osize.is_multiple_of(ALIGN));
    assert!(osize > ALIGN);
    let nsize = size.next_multiple_of(ALIGN) + ALIGN;
    if nsize == osize {
        return ptr;
    }
    let nptr = unsafe { realloc(optr, olayout, nsize) };
    if nptr.is_null() {
        return std::ptr::null_mut();
    }
    if osize < nsize {
        unsafe {
            std::ptr::write_bytes(nptr.wrapping_add(osize), 0, nsize - osize);
        }
    }
    let layout = Layout::from_size_align(nsize, ALIGN).expect("layout makes sense");
    unsafe {
        std::ptr::write(nptr.cast(), layout);
    }
    nptr.wrapping_add(ALIGN)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn safemalloc(size: usize) -> *mut c_void {
    let size = usize::max(size, 1);
    let ptr = unsafe { rsmalloc(size) };
    assert_ne!(ptr, std::ptr::null_mut(), "trn: out of memory!");
    ptr.cast()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn saferealloc(ptr: *mut c_void, size: usize) -> *mut c_void {
    let size = usize::max(size, 1);
    let ptr = unsafe { rsrealloc(ptr.cast(), size) };
    assert_ne!(ptr, std::ptr::null_mut(), "trn: out of memory!");
    ptr.cast()
}

/// Returns a constant nil pointer so that the caller can do
/// things like, `p = safefree(p);` to free and null out the
/// pointer.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn safefree(ptr: *mut c_void) -> *mut c_void {
    unsafe {
        rsfree(ptr.cast());
    }
    std::ptr::null_mut()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn estrdup(cstrp: *const c_char) -> *mut c_char {
    assert!(!cstrp.is_null(), "estrdup: string is null");
    let cstr = unsafe { CStr::from_ptr(cstrp) };
    let bs = cstr.to_bytes_with_nul();
    let buf = unsafe { rsmalloc(bs.len()) };
    assert!(!buf.is_null());
    unsafe {
        std::ptr::copy(bs.as_ptr(), buf, bs.len());
    }
    buf.cast()
}

/// Code derivced from `mempool.c` etc.
///
/// The original trn `mempool` infrastructure looks like it was
/// meant to be an arena bump allocator for performance.
///
/// Unfortunately, it ended up looking more like a collection of
/// K&R style allocators layered on top of the system malloc.
/// The only reason I don't remove this entirely and replace it
/// with malloc/free is because `mp_free` frees an entire "pool"
/// at once, and tracking down all of the allocations that need
/// to be freed would be a bigger lift.

const NPOOLS: usize = 3;

#[repr(C)]
pub enum Mempool {
    Score1 = 0,
    Score2 = 1,
    SAThread = 2,
}

impl From<i32> for Mempool {
    fn from(v: c_int) -> Self {
        match v {
            0 => Self::Score1,
            1 => Self::Score2,
            2 => Self::SAThread,
            _ => panic!("bad mempool type in FFI"),
        }
    }
}

static POOLS: RwLock<[Vec<usize>; NPOOLS]> = RwLock::new([Vec::new(), Vec::new(), Vec::new()]);

/// Duplicates a C string and stashes a pointer to it in the
/// given pool.
///
/// # Safety
/// The caller must ensure that the argument C-string pointer is
/// valid, and remains so for the lifetime of the pool.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mp_estrdup(s: *const c_char, p: c_int) -> *mut c_char {
    let s = unsafe { estrdup(s) };
    let pool = Mempool::from(p);
    let mut pools = POOLS.write().expect("POOLS is not poisoned");
    let arena = &mut pools[pool as usize];
    arena.push(s.addr());
    s
}

/// Frees all pointers saved in the "pool".
///
/// # Safety
/// The caller must ensure that the pointers stashed in the pool
/// are valid, were not previously freed, and so on.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mp_free(p: c_int) {
    let pool = Mempool::from(p);
    let mut pools = POOLS.write().expect("POOLS is not poisoned");
    let arena = &mut pools[pool as usize];
    for &addr in arena.iter() {
        unsafe {
            let ptr = std::ptr::with_exposed_provenance_mut(addr);
            safefree(ptr);
        }
    }
    arena.clear();
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ptr::{copy, null_mut, read, write};

    #[test]
    fn malloc0() {
        let ptr = unsafe { rsmalloc(0) };
        assert_eq!(ptr, null_mut());
    }

    #[test]
    fn malloc1() {
        let ptr = unsafe { rsmalloc(1) };
        assert_ne!(ptr, null_mut());
        unsafe {
            rsfree(ptr);
        }
    }

    #[test]
    fn mallocmut() {
        let vptr = unsafe { rsmalloc(1) };
        let ptr = vptr.cast::<u8>();
        let zero = unsafe { read(ptr) };
        assert_eq!(zero, 0);
        unsafe {
            write(ptr, 1);
        }
        let one = unsafe { read(ptr) };
        assert_eq!(one, 1);
        unsafe {
            rsfree(vptr);
        }
    }

    #[test]
    fn realloc_null() {
        let vptr = unsafe { rsrealloc(null_mut(), 1) };
        assert_ne!(vptr, null_mut());
        unsafe {
            rsfree(vptr);
        }
    }

    #[test]
    fn realloc0_frees() {
        let vptr = unsafe { rsmalloc(8) };
        assert_ne!(vptr, null_mut());
        let nil = unsafe { rsrealloc(vptr, 0) };
        assert_eq!(nil, null_mut());
    }

    #[test]
    fn realloc() {
        let vptr = unsafe { rsmalloc(8) };
        assert_ne!(vptr, null_mut());
        unsafe {
            let bs = b"12345678";
            copy(bs.as_ptr(), vptr.cast::<u8>(), 8);
        }
        let nvptr = unsafe { rsrealloc(vptr, 16) };
        assert_ne!(nvptr, null_mut());
        let first = unsafe { read(nvptr.cast::<[u8; 8]>()) };
        assert_eq!(first, *b"12345678");
        let second = unsafe { read(nvptr.wrapping_add(8).cast::<[u8; 8]>()) };
        assert_eq!(second, [0u8, 0, 0, 0, 0, 0, 0, 0]);
        unsafe {
            rsfree(nvptr);
        }
    }

    #[test]
    fn mempool() {
        let cs = c"test";
        let sp = cs.as_ptr();
        let score1 = Mempool::Score1 as c_int;

        let pools = POOLS.read().expect("pool read lock ok");
        assert!(pools[Mempool::Score1 as usize].is_empty());
        drop(pools);

        let dup_cstr_ptr = unsafe { mp_estrdup(sp, score1) };
        assert!(!dup_cstr_ptr.is_null());
        let bs = unsafe { read(dup_cstr_ptr.cast::<[u8; 5]>()) };
        assert_eq!(bs, *b"test\0");

        let pools = POOLS.read().expect("pool read lock ok");
        assert_eq!(pools[Mempool::Score1 as usize].len(), 1);
        drop(pools);

        unsafe {
            mp_free(score1);
        }

        let pools = POOLS.read().expect("pool read lock ok");
        assert!(pools[Mempool::Score1 as usize].is_empty());
    }
}
