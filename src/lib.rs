#![cfg_attr(target_os = "none", no_std)]

mod sun;

#[cfg(target_os = "none")]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
