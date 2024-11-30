#![cfg_attr(target_os = "none", no_std)]

use core::ffi::{c_char, CStr};

#[cfg(target_os = "none")]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

extern "C" {
    fn usb3sun_debug_write(data: *const c_char, len: u32) -> bool;
}

#[no_mangle]
extern "C" fn hello_rust() {
    let data = CStr::from_bytes_with_nul(b"hello from rust!\n\0").expect("guaranteed by argument");
    let len = data
        .count_bytes()
        .checked_add(1)
        .expect("length overflow on add");
    let len = len.try_into().expect("length overflow");

    unsafe {
        usb3sun_debug_write(data.as_ptr(), len);
    }
}
