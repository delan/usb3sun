#ifndef USB3SUN_RUST_H
#define USB3SUN_RUST_H

#include <cstdint>

extern "C" {

enum class FrameTag: uint8_t {
    Unknown,
    StatusRequest,
    StatusChange,
    DeviceDisconnect,
    ValidKeyValue,
};
struct StatusChange {
    uint8_t report_id;
    uint8_t id0_id1_status;
};
struct ValidKeyValue {
    uint8_t length;
    uint8_t labeling;
    uint8_t data[72];
    uint8_t serial;
    uint8_t check;
};
struct Frame {
    FrameTag tag;
    union {
        uint8_t Unknown;
        uint8_t StatusRequest;
        StatusChange StatusChange;
        ValidKeyValue ValidKeyValue;
    };
};
enum class TaskResponseTag: uint8_t {
    Idle,
    Frame,
};
struct TaskResponse {
    TaskResponseTag tag;
    union {
        Frame Frame;
    };
};
TaskResponse usb3sun_ch9350_task(void);

}

#endif
