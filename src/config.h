#define DISPLAY_ROTATION 0  // 0|1|2|3 where 0 means GND pin is bottom left

#define DEBUG_LOGGING           // allow any logging
#define DEBUG_OVER_CDC          // log to Serial (USB CDC); excludes TinyUSB debugging
#define DEBUG_OVER_UART         // log to Serial1 (UART0); includes TinyUSB debugging
                                // (ignored in pinout v1 when SUNK_ENABLE is defined)
#define DEBUG_UART_BAUD 115200  // only 115200 works with stock picoprobe firmware

// #define DEBUG_TIMINGS       // log time spent on critical operations
// #define BUZZER_VERBOSE      // log buzzer state changes for debugging
// #define SUNK_VERBOSE        // log keyboard tx for debugging
// #define SUNM_VERBOSE        // log mouse tx for debugging
// #define UHID_VERBOSE        // log hid reports for debugging
// #define UHID_LED_TEST       // blink leds on all keyboards

// #define WAIT_PIN 22      // GP# number (optional) to wait for ground on boot
// #define WAIT_SERIAL      // wait for serial input over USB CDC on boot (buggy)
#define USB0_DP 2           // GP# number for USB root port 0 D+
#define USB0_DM (USB_DP+1)  // GP# number for USB root port 0 D- (always D+ GP# + 1)
#define USB1_DP 4           // GP# number for USB root port 1 D+
#define USB1_DM (USB_DP+1)  // GP# number for USB root port 1 D- (always D+ GP# + 1)
#define BUZZER_PIN 28       // GP# number for positive of passive piezo buzzer
#define PINOUT_V2_PIN 7     // GP# number for detecting pinout v2 (rev A4+)

// the following must be GP# numbers valid for...
#define DEBUG_UART_TX 0     // ...UART0 TX: connect to picoprobe GP5
#define DEBUG_UART_RX 1     // ...UART0 RX: connect to picoprobe GP4
#define DISPLAY_ENABLE 12   // ...any: external reset for SSD1306
#define DISPLAY_SCL 17      // ...I2C0 SCL: SCL pin of SSD1306
#define DISPLAY_SDA 16      // ...I2C0 SDA: SDA pin of SSD1306

// pinout v1/v2 config
// the following must be GP# numbers valid for...
                            // pin 1: 0 V (purple)
                            // pin 2: 0 V (brown)
                            // pin 3: +5 Vdc (blue)
                            // n/c: mouse rx
#define SUN_MRX_V1 9        // ...UART1 RX (required by SerialUART api)
                            // pin 4: mouse tx (gray)
#define SUN_MTX_V1 8        // ...UART1 TX
#define SUN_MTX_V2 6        // ...any
                            // pin 5: keyboard rx (red)
#define SUN_KRX_V1 13       // ...UART0 RX
#define SUN_KRX_V2 9        // ...UART1 RX
                            // pin 6: keyboard tx (green)
#define SUN_KTX_V1 12       // ...UART0 TX
#define SUN_KTX_V2 8        // ...UART1 TX
                            // pin 7: power key (yellow)
#define POWER_KEY 15        // ...any
                            // pin 8: +5 Vdc (orange)
#define DEBUG_UART          Serial1     // UART0
#define SUNK_UART_V1        Serial1     // UART0
#define SUNK_UART_V2        Serial2     // UART1
#define SUNM_UART_V1        Serial2     // UART1

#define SUNK_ENABLE
#define SUNM_ENABLE

#if defined(DEBUG_LOGGING)
#define Sprint(...) do { pinout.debugPrint(__VA_ARGS__); } while (0)
#define Sprintln(...) do { pinout.debugPrintln(__VA_ARGS__); } while (0)
#define Sprintf(...) do { pinout.debugPrintf(__VA_ARGS__); } while (0)
#else
#define Sprint(...) do {} while (0)
#define Sprintln(...) do {} while (0)
#define Sprintf(...) do {} while (0)
#endif
