//! Regular expression FFI bindings.

/*
void init_compex(COMPEX *);
void free_compex(COMPEX *);
char *getbracket(COMPEX *, size_t);
char *compile(COMPEX *, const char *, bool, bool);
char *execute(COMPEX *, const char *);
*/

use std::ffi::{CStr, c_char};
use std::ops::RangeInclusive;
use regex::bytes::{RegexBuilder, Regex};

struct Inner {
    regex: Regex,
    captured: Vec<RangeInclusive<usize>>,
}

/// A type corresponding to the C type passed across the FFI
/// boundary.  We use raw pointers and so forth because that's
/// what C gives us.
#[repr(C)]
struct Compex {
    inner: Option<Box<Inner>>,
}

#[unsafe(no_mangle)]
unsafe extern "C" fn init_compex(ptr: *mut Compex) {
    assert!(!ptr.is_null());
    assert!(ptr.is_aligned());
    unsafe {
        std::ptr::write(ptr, Compex { inner: None });
    }
}

#[unsafe(no_mangle)]
unsafe extern "C" fn free_compex(ptr: *mut Compex) {
    if !ptr.is_null() {
        assert!(ptr.is_aligned());
        let ce = unsafe { &mut *ptr };
        ce.inner = None;
    }
}

#[unsafe(no_mangle)]
unsafe extern "C" fn compile(
    ptr: *mut Compex,
    regexp: *const c_char,
    ignore_case: bool,
) -> *const c_char {
    unsafe {
        free_compex(ptr);
    }
    let compex = unsafe { &mut *ptr };
    assert!(!regexp.is_null());
    let cregex = unsafe { CStr::from_ptr(regexp) };
    let Ok(re) = cregex.to_str() else {
        return c"RE is not valid UTF-8".as_ptr();
    };
    let maybe = RegexBuilder::new(re)
        .unicode(false)
        .multi_line(true)
        .case_insensitive(ignore_case)
        .build();
    let Ok(regex) = maybe else {
            return c"Badly formed regular expression".as_ptr();
    };
    let captured = Vec::new();
    let inner = Inner { regex, captured };
    compex.inner = Some(Box::new(inner));
    std::ptr::null()
}

#[unsafe(no_mangle)]
unsafe extern "C" fn execute(ptr: *mut Compex, s: *mut c_char) -> *const c_char {
    assert!(!ptr.is_null());
    assert!(ptr.is_aligned());
    assert!(!s.is_null());
    let compex = unsafe { &mut *ptr };
    let Some(inner) = compex.inner.as_mut() else {
        panic!("execute() on uninitialized Compex");
    };
    let cstr = unsafe { CStr::from_ptr(s) };
    let Some(captures) = inner.regex.captures(cstr.to_bytes()) else {
        return std::ptr::null();
    };
    inner.captured.reserve(captures.len());
    for k in 0..captures.len() {
        let capture = captures.get(k).expect("capture");
        inner.captured.push(capture.start()..=capture.end());
    }
    s.cast()
}
