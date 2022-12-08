// #define WAIT_PIN 22      // GP# number (optional) to wait for ground on boot
// #define WAIT_SERIAL      // wait for serial input over USB CDC on boot (buggy)
#define USB_DP 2            // GP# number for USB D+
#define USB_DM (USB_DP+1)   // GP# number for USB D- (always D+ GP# + 1)
#define BUZZER_PIN 28       // positive pin of passive piezo buzzer

// the following must be GP# numbers valid for...
#define DISPLAY_SCL 17      // ...I2C0 SCL: SCL pin of SSD1306
#define DISPLAY_SDA 16      // ...I2C0 SDA: SDA pin of SSD1306
#define DISPLAY_ROTATION 2  // (not a GP#)  0 | 1 | 2 | 3

// the following must be GP# numbers valid for...
                            //              pin 1: 0 V (purple)
                            //              pin 2: 0 V (brown)
                            //              pin 3: +5 Vdc (blue)
#define SUN_MTX 8           // ...UART1 TX: pin 4: mouse tx (gray)
#define SUN_MRX 9           // ...UART1 RX: n/c: mouse rx
#define SUN_KRX 13          // ...UART0 RX: pin 5: keyboard rx (red)
#define SUN_KTX 12          // ...UART0 TX: pin 6: keyboard tx (green)
                            //              pin 7: 0 V (yellow)
                            //              pin 8: +5 Vdc (orange)

// fake Sun host for loopback testing (disables mouse support)
// #define FAKE_SUN_ENABLE

// the following must be GP# numbers valid for...
#define FAKE_SUN_KRX 4      // ...UART1 TX: connect to SUN_KRX
#define FAKE_SUN_KTX 5      // ...UART1 RX: connect to SUN_KTX
