#![cfg_attr(target_os = "none", no_std)]

mod ch9350;

use core::{cell::UnsafeCell, ops::Deref};

#[cfg(target_os = "none")]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

/// # Safety
///
/// uhhhhhh
#[derive(Debug)]
#[repr(transparent)]
struct Singleton<T>(UnsafeCell<T>);

unsafe impl<T> Sync for Singleton<T> {}

impl<T> Deref for Singleton<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        unsafe { &*(self.0.get() as *const _) }
    }
}

impl<T> Singleton<T> {
    const fn new(inner: T) -> Self {
        Self(UnsafeCell::new(inner))
    }

    fn get_mut(&self) -> &mut T {
        unsafe { &mut *self.0.get() }
    }
}
