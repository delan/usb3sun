#ifndef USB3SUN_HAL_H
#define USB3SUN_HAL_H

#include <cstdbool>
#include <cstddef>
#include <cstdint>

extern "C" {

typedef uint8_t usb3sun_pin;

// based on tinyusb’s tuh_hid_report_info_t
typedef struct {
  uint8_t report_id;
  uint8_t usage;
  uint16_t usage_page;
} usb3sun_hid_report_info;

#ifdef USB3SUN_HAL_ARDUINO_PICO
  #include <pico/mutex.h>
  #include <hardware/sync.h>
  typedef mutex_t usb3sun_mutex;
  #define USB3SUN_MUTEX __attribute__((section(".mutex_array")))
  #define usb3sun_dmb() __dmb()
#elifdef USB3SUN_HAL_LINUX_NATIVE
  struct usb3sun_mutex {};
  #define USB3SUN_MUTEX // empty
  #define usb3sun_dmb() do {} while (0)

  extern "C++" {
    #include <iostream>
    #include <optional>
    #include <variant>
    #include <vector>
    struct PinoutV2Op { static const uint64_t id = 1 << 0; };
    struct SunkInitOp { static const uint64_t id = 1 << 1; };
    struct SunkReadOp { static const uint64_t id = 1 << 2; };
    struct SunkWriteOp { static const uint64_t id = 1 << 3; std::vector<uint8_t> data; };
    struct SunmInitOp { static const uint64_t id = 1 << 4; uint32_t baud; };
    struct SunmWriteOp { static const uint64_t id = 1 << 5; std::vector<uint8_t> data; };
    struct GpioReadOp { static const uint64_t id = 1 << 6; usb3sun_pin pin; bool value; };
    struct GpioWriteOp { static const uint64_t id = 1 << 7; usb3sun_pin pin; bool value; };
    struct UhidRequestReportOp { static const uint64_t id = 1 << 8; uint8_t dev_addr, instance; };
    struct BuzzerStartOp { static const uint64_t id = 1 << 9; uint32_t pitch; };
    struct FsReadOp { static const uint64_t id = 1 << 10; std::string path; size_t expected_len; std::optional<std::vector<uint8_t>> data; };
    struct FsWriteOp { static const uint64_t id = 1 << 11; std::string path; std::vector<uint8_t> data; };
    struct RebootOp { static const uint64_t id = 1 << 12; };
    struct AlarmOp { static const uint64_t id = 1 << 13; uint32_t ms; };
    struct SunkSnifferInitOp { static const uint64_t id = 1 << 14; };
    struct SunkSniffTxOp { static const uint64_t id = 1 << 15; };
    using Op = std::variant<
      PinoutV2Op,
      SunkInitOp,
      SunkReadOp,
      SunkWriteOp,
      SunmInitOp,
      SunmWriteOp,
      GpioReadOp,
      GpioWriteOp,
      UhidRequestReportOp,
      BuzzerStartOp,
      FsReadOp,
      FsWriteOp,
      RebootOp,
      AlarmOp,
      SunkSnifferInitOp,
      SunkSniffTxOp>;
    struct Entry {
      uint64_t micros;
      Op op;
    };
    std::ostream &operator<<(std::ostream &s, const std::vector<uint8_t> &v);
    std::ostream &operator<<(std::ostream &s, const Op &o);
    std::ostream &operator<<(std::ostream &s, const Entry &v);
    template <typename T>
    std::ostream &operator<<(std::ostream &s, const std::optional<T> &v) {
      if (!v.has_value()) {
        return s << "{}";
      }
      return s << "{" << *v << "}";
    }
#define DERIVE_OP(_name, _eq, _display) \
    static inline bool operator==(const _name &p, const _name &q) { return _eq; } \
    static inline bool operator!=(const _name &p, const _name &q) { return !(p == q); }; \
    static inline std::ostream &operator<<(std::ostream &s, const _name &o) { return s << _display; };
    DERIVE_OP(PinoutV2Op, ((void) p, (void) q, true), ((void) o, "pinout_v2"));
    DERIVE_OP(SunkInitOp, ((void) p, (void) q, true), ((void) o, "sunk_init"));
    DERIVE_OP(SunkReadOp, ((void) p, (void) q, true), ((void) o, "sunk_read"));
    DERIVE_OP(SunkWriteOp, p.data == q.data, "sunk_write " << o.data);
    DERIVE_OP(SunmInitOp, p.baud == q.baud, "sunm_init " << o.baud);
    DERIVE_OP(SunmWriteOp, p.data == q.data, "sunm_write " << o.data);
    DERIVE_OP(GpioReadOp, p.pin == q.pin && p.value == q.value, "gpio_read " << (unsigned)o.pin << " " << o.value);
    DERIVE_OP(GpioWriteOp, p.pin == q.pin && p.value == q.value, "gpio_write " << (unsigned)o.pin << " " << o.value);
    DERIVE_OP(UhidRequestReportOp, p.dev_addr == q.dev_addr && p.instance == q.instance, "uhid_request_report " << (unsigned)o.dev_addr << " " << (unsigned)o.instance);
    DERIVE_OP(BuzzerStartOp, p.pitch == q.pitch, "buzzer_start " << o.pitch);
    DERIVE_OP(FsReadOp, p.path == q.path && p.expected_len == q.expected_len && p.data == q.data, "fs_read " << o.path << " " << o.expected_len << " " << o.data);
    DERIVE_OP(FsWriteOp, p.path == q.path && p.data == q.data, "fs_write " << o.path << " " << o.data);
    DERIVE_OP(RebootOp, ((void) p, (void) q, true), ((void) o, "reboot"));
    DERIVE_OP(AlarmOp, p.ms == q.ms, "alarm " << o.ms);
    DERIVE_OP(SunkSnifferInitOp, ((void) p, (void) q, true), ((void) o, "sunk_sniffer_init"));
    DERIVE_OP(SunkSniffTxOp, ((void) p, (void) q, true), ((void) o, "sunk_sniff_tx"));
    void usb3sun_test_init(uint64_t history_filter_mask);
    void usb3sun_mock_gpio_read(usb3sun_pin pin, bool value);
    void usb3sun_mock_sunk_read(const char *data, size_t len);
    bool usb3sun_mock_sunk_read_has_input(void);
    void usb3sun_mock_usb_vid_pid(bool result, uint16_t vid, uint16_t pid);
    void usb3sun_mock_uhid_parse_report_descriptor(const std::vector<usb3sun_hid_report_info> &infos);
    void usb3sun_mock_uhid_interface_protocol(uint8_t if_protocol);
    void usb3sun_mock_uhid_request_report_result(bool result);
    void usb3sun_mock_fs_read(bool (*mock)(const char *path, char *data, size_t data_len, size_t &actual_len));
    void usb3sun_mock_display_output(int fd);
    const std::vector<Entry> &usb3sun_test_get_history(void);
    void usb3sun_test_clear_history(void);
    void usb3sun_test_exit_on_reboot(void);
    void usb3sun_test_terminal_demo_mode(bool enabled);
  }
#endif

size_t usb3sun_pinout_version(void);
void usb3sun_pinout_v2(void);

void usb3sun_sunk_init(void);
int usb3sun_sunk_read(void);
size_t usb3sun_sunk_write(uint8_t *data, size_t len);

void usb3sun_sunk_sniffer_init(void);
int usb3sun_sunk_sniff_tx(void);

void usb3sun_sunm_init(uint32_t baud);
size_t usb3sun_sunm_write(uint8_t *data, size_t len);

void usb3sun_usb_init(void);
void usb3sun_usb_task(void);
bool usb3sun_usb_vid_pid(uint8_t dev_addr, uint16_t *vid, uint16_t *pid);
bool usb3sun_uhid_request_report(uint8_t dev_addr, uint8_t instance);
uint8_t usb3sun_uhid_interface_protocol(uint8_t dev_addr, uint8_t instance);
size_t usb3sun_uhid_parse_report_descriptor(usb3sun_hid_report_info *result, size_t result_len, const uint8_t *descriptor, size_t descriptor_len);
bool usb3sun_uhid_set_led_report(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t &led_report);

void usb3sun_ch9350_init(void);
int usb3sun_ch9350_read(void);
size_t usb3sun_ch9350_write(uint8_t *data, size_t len);

void usb3sun_debug_init(int (*printf)(const char *format, ...));
int usb3sun_debug_uart_read(void);
int usb3sun_debug_cdc_read(void);
bool usb3sun_debug_write(const char *data, size_t len);
void usb3sun_allow_debug_over_cdc(void);
void usb3sun_allow_debug_over_uart(void);

bool usb3sun_fs_init(void);
bool usb3sun_fs_wipe(void);
// returns true iff the read succeeded, otherwise data is undefined.
bool usb3sun_fs_read(const char *path, char *data, size_t len);
bool usb3sun_fs_write(const char *path, const char *data, size_t len);

void usb3sun_mutex_lock(usb3sun_mutex *mutex);
void usb3sun_mutex_unlock(usb3sun_mutex *mutex);

bool usb3sun_fifo_push(uint32_t value);
bool usb3sun_fifo_pop(uint32_t *result);

void usb3sun_reboot(void);
uint64_t usb3sun_micros(void);
void usb3sun_sleep_micros(uint64_t micros);
uint32_t usb3sun_clock_speed(void);
void usb3sun_panic(const char *format, ...);
void usb3sun_alarm(uint32_t ms, void (*callback)(void));

bool usb3sun_gpio_read(usb3sun_pin pin);
void usb3sun_gpio_write(usb3sun_pin pin, bool value);
void usb3sun_gpio_set_as_inverted(usb3sun_pin pin);
void usb3sun_gpio_set_as_output(usb3sun_pin pin);
void usb3sun_gpio_set_as_input_pullup(usb3sun_pin pin);
void usb3sun_gpio_set_as_input_pulldown(usb3sun_pin pin);

void usb3sun_i2c_set_pinout(usb3sun_pin scl, usb3sun_pin sda);

void usb3sun_buzzer_start(uint32_t pitch);

void usb3sun_display_init(void);
void usb3sun_display_flush(void);
void usb3sun_display_clear(void);
void usb3sun_display_rect(
    int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t border_radius, bool inverted, bool filled);
// every > 1 yields dotted lines (extension to Adafruit GFX Library).
void usb3sun_display_hline(int16_t x, int16_t y, int16_t w, bool inverted, int16_t every = 1);
// every > 1 yields dotted lines (extension to Adafruit GFX Library).
void usb3sun_display_vline(int16_t x, int16_t y, int16_t h, bool inverted, int16_t every = 1);
// based on Adafruit GFX library with default “classic” font in cp437-correct mode, plus extensions:
// - no background is painted by default, unless you pass opaque = true
// - the byte FFh yields a space with no background, even if opaque = true
void usb3sun_display_text(int16_t x, int16_t y, bool inverted, const char *text, bool opaque = false);

}

#endif
