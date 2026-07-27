use std::ffi::{c_char, c_int};

/// Copies the UTF-8 representation of a unicode codepoint to the location
/// given by the C char pointer `ptr`.
///
/// # Safety
///
/// The caller must ensure that the value `cc` refers to a valid unicode
/// codepoint.
///
/// The caller must ensure that the pointer, `ptr`, refers to a valid
/// buffer of length sufficient to hold the number of bytes required for
/// the representation of the code point given in C.
#[unsafe(no_mangle)]
pub extern "C" fn insert_utf8_at(dst: *mut c_char, len: usize, cc: u32) -> c_int {
    let mut bs = [0; 8];
    let c: char = u32::try_into(cc).expect("cc is char");
    let s = c.encode_utf8(&mut bs);
    assert!(s.len() <= len);
    unsafe {
        std::ptr::copy(s.as_ptr().cast(), dst, s.len());
    }
    s.len().try_into().expect("UTF-8 length fits in c_int")
}
