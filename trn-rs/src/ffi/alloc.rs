//! Provides an FFI-compatible allocator for C.
//!
//! This replaces the elderly `safemalloc` function and friends.
//! This lets us use the Rust memory allocator from C code.

use std::alloc::{alloc_zeroed, realloc, dealloc, Layout};
use std::ffi::{c_char, c_void, CStr};

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
    if ptr.is_null() || !ptr.addr().is_multiple_of(ALIGN) {
        return;
    }
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
    let layout = Layout::from_size_align(nsize, ALIGN);
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn safefree(ptr: *mut c_void) -> *mut c_void {
    unsafe {
        rsfree(ptr.cast());
    }
    std::ptr::null_mut()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn estrdup(cstrp: *mut c_char) -> *mut c_char {
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn malloc0() {
        let ptr = unsafe { rsmalloc(0) };
        assert_eq!(ptr, std::ptr::null_mut());
    }

    #[test]
    fn malloc1() {
        let ptr = unsafe { rsmalloc(1) };
        assert_ne!(ptr, std::ptr::null_mut());
        unsafe {
            rsfree(ptr);
        }
    }

    #[test]
    fn mallocmut() {
        let vptr = unsafe { rsmalloc(1) };
        let ptr = vptr.cast::<u8>();
        let zero = unsafe { std::ptr::read(ptr) };
        assert_eq!(zero, 0);
        unsafe {
            std::ptr::write(ptr, 1);
        }
        let one = unsafe { std::ptr::read(ptr) };
        assert_eq!(one, 1);
        unsafe {
            rsfree(vptr);
        }
    }
}
