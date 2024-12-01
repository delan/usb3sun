use core::{
    ffi::{c_int, c_uint, c_void},
    mem::take,
};

use crate::Singleton;

extern "C" {
    fn usb3sun_ch9350_read() -> c_int;
    fn usb3sun_ch9350_write(data: *const c_void, len: c_uint) -> c_uint;
}

static READER: Singleton<Reader> = Singleton::new(Reader::new());

#[no_mangle]
unsafe extern "C" fn usb3sun_ch9350_task() -> TaskResponse {
    match READER.get_mut().read() {
        Some(frame @ Frame::StatusRequest { .. }) => {
            // state 0 response: seems to yield limited ValidKeyValue frames,
            // where only one frame is sent after each key down/up pair?
            // let response = b"\x57\xAB\x12\x00\x00\x00\x00\xFF\x80\x00\x20";

            // state 1 response: does not suffer from that problem
            let response = b"\x57\xAB\x12\x00\x00\x00\x00\xFF\xFF\x00\x20";
            usb3sun_ch9350_write(response.as_ptr() as *const _, response.len() as _);
            return TaskResponse::Frame(frame);
        }
        Some(frame) => return TaskResponse::Frame(frame),
        None => return TaskResponse::Idle,
    }

    TaskResponse::Idle
}

#[derive(Debug, Default)]
#[repr(C, u8)]
enum TaskResponse {
    #[default]
    Idle,
    Frame(Frame),
}

#[derive(Debug, Default)]
enum Reader {
    #[default]
    Expect57h,
    ExpectABh,
    ExpectCommandCode,

    StatusRequestValue,
    StatusChangeValue,
    ValidKeyValueLength,
    ValidKeyValueLabeling {
        length: u8,
    },
    ValidKeyValueData {
        length: u8,
        labeling: u8,
        data: [u8; 72],
        i: usize,
    },
    ValidKeyValueSerial {
        length: u8,
        labeling: u8,
        data: [u8; 72],
    },
    ValidKeyValueCheck {
        length: u8,
        labeling: u8,
        data: [u8; 72],
        serial: u8,
    },
}

#[derive(Debug)]
#[repr(C, u8)]
enum Frame {
    Unknown {
        command: u8,
    },
    StatusRequest {
        io_status: u8,
    },
    StatusChange {
        report_id: u8,
        io0_io1_status: u8,
    },
    DeviceDisconnect,
    ValidKeyValue {
        length: u8,
        labeling: u8,
        data: [u8; 72],
        serial: u8,
        check: u8,
    },
}

impl Reader {
    const fn new() -> Self {
        Self::Expect57h
    }

    fn read_octet() -> Option<u8> {
        unsafe {
            match usb3sun_ch9350_read() {
                -1 => None,
                other => Some(other as u8),
            }
        }
    }

    fn read(&mut self) -> Option<Frame> {
        if let Some(input) = Self::read_octet() {
            match take(self) {
                Self::Expect57h => {
                    if input == 0x57 {
                        *self = Self::ExpectABh;
                    }
                }
                Self::ExpectABh => {
                    if input == 0xAB {
                        *self = Self::ExpectCommandCode;
                    }
                }
                Self::ExpectCommandCode => match input {
                    0x82 => *self = Self::StatusRequestValue,
                    0x80 => *self = Self::StatusChangeValue,
                    0x86 => {
                        *self = Self::default();
                        return Some(Frame::DeviceDisconnect);
                    }
                    0x88 => *self = Self::ValidKeyValueLength,
                    command => {
                        *self = Self::default();
                        return Some(Frame::Unknown { command });
                    }
                },
                Self::StatusRequestValue => {
                    *self = Self::default();
                    return Some(Frame::StatusRequest {
                        io_status: input & 0b1111,
                    });
                }
                Self::StatusChangeValue => {
                    *self = Self::default();
                    return Some(Frame::StatusChange {
                        report_id: input & 0b1111,
                        io0_io1_status: input >> 4,
                    });
                }
                Self::ValidKeyValueLength => {
                    *self = Self::ValidKeyValueLabeling { length: input };
                }
                Self::ValidKeyValueLabeling { length } => {
                    *self = Self::ValidKeyValueData {
                        length,
                        labeling: input,
                        data: [0; 72],
                        i: 0,
                    };
                }
                Self::ValidKeyValueData {
                    length,
                    labeling,
                    mut data,
                    i,
                } => {
                    *self = if i < length.into() {
                        data[i] = input;
                        Self::ValidKeyValueData {
                            length,
                            labeling,
                            data,
                            i: i + 1,
                        }
                    } else {
                        Self::ValidKeyValueSerial {
                            length,
                            labeling,
                            data,
                        }
                    };
                }
                Self::ValidKeyValueSerial {
                    length,
                    labeling,
                    data,
                } => {
                    *self = Self::ValidKeyValueCheck {
                        length,
                        labeling,
                        data,
                        serial: input,
                    };
                }
                Self::ValidKeyValueCheck {
                    length,
                    labeling,
                    data,
                    serial,
                } => {
                    *self = Self::default();
                    return Some(Frame::ValidKeyValue {
                        length,
                        labeling,
                        data,
                        serial,
                        check: input,
                    });
                }
            }
        }

        None
    }
}
