/// Makes a TCP connection to a remote machine.
use std::ffi::{c_char, c_int};
use std::sync::Mutex;

unsafe fn cptr_as_str<'a>(cstrp: *const c_char) -> &'a str {
    assert!(!cstrp.is_null());
    let cstr = unsafe { std::ffi::CStr::from_ptr(cstrp) };
    cstr.to_str().expect("C String is well-formed")
}

fn servport(port: u16, servp: *const c_char) -> Option<u16> {
    if port != 0 {
        return Some(port);
    }
    match unsafe { cptr_as_str(servp) } {
        "nntp" | "usenet" => Some(119),
        "snttp" | "nntps" => Some(563),
        "http" => Some(80),
        "https" => Some(443),
        "ftp" => Some(21),
        service => {
            static LOOKUP: Mutex<()> = Mutex::new(());
            let _lookup = LOOKUP.lock().expect("Locked getservbyname mutex");
            let sep = unsafe { nix::libc::getservbyname(servp, c"tcp".as_ptr()) };
            if sep.is_null() {
                eprintln!("{service}/tcp: Unknown service.");
                return None;
            }
            let se = unsafe { &*sep };
            let portbe = u16::try_from(se.s_port).expect("port is 16 bits");
            let port = u16::from_be(portbe);
            Some(port)
        }
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn get_tcp_socket(
    hostp: *const c_char,
    rport: u16,
    servp: *const c_char,
) -> c_int {
    use std::net::TcpStream;
    use std::os::fd::IntoRawFd;

    if hostp.is_null() || (rport == 0 && servp.is_null()) {
        return -1;
    }
    let Some(port) = servport(rport, servp) else {
        eprintln!("unknown service");
        return -1;
    };
    let host = unsafe { cptr_as_str(hostp) };
    let maybe = TcpStream::connect((host, port));
    let Ok(conn) = maybe else {
        eprintln!("connection to {host}:{port} failed: {maybe:?}");
        return -1;
    };
    conn.into_raw_fd()
}
