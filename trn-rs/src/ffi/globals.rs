/// !!!DANGER!!! !!!DANGER!!! This is awful.

use std::ffi::c_char;

/// Login directory
#[unsafe(no_mangle)]
static mut homedir: *mut c_char = std::ptr::null_mut();

/// Where `.` files go
#[unsafe(no_mangle)]
static mut dotdir: *mut c_char = std::ptr::null_mut();

/// Usually %./.trn
#[unsafe(no_mangle)]
static mut trndir: *mut c_char = std::ptr::null_mut();

/// News library
#[unsafe(no_mangle)]
static mut newslib: *mut c_char = std::ptr::null_mut();

/// Private news program library
#[unsafe(no_mangle)]
static mut rnlib: *mut c_char = std::ptr::null_mut();

/// Where tmp files go
#[unsafe(no_mangle)]
static mut tmpdir: *mut c_char = std::ptr::null_mut();

/// Login ID of user
#[unsafe(no_mangle)]
static mut loginName: *mut c_char = std::ptr::null_mut();

/// Real name of user
#[unsafe(no_mangle)]
static mut realName: *mut c_char = std::ptr::null_mut();

/// Host name in a posting
#[unsafe(no_mangle)]
static mut phostname: *mut c_char = std::ptr::null_mut();

/// Local host name
#[unsafe(no_mangle)]
static mut localhost: *mut c_char = std::ptr::null_mut();
