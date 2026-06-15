/// Returns the current time of day in milliseconds since the Unix epoch.
#[unsafe(no_mangle)]
pub extern "C" fn current_time_ms() -> u64 {
    use std::time::SystemTime;
    let since = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH);
    let ms = since
        .expect("current time since epoch representable")
        .as_millis();
    u64::try_from(ms).expect("time since epoch as millis fits into a 64-bit integer")
}
