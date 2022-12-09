#define WAIT_PIN 22      // GP# number (optional) to wait for ground on boot
// #define WAIT_SERIAL      // wait for serial input over USB CDC on boot (buggy)
#define USB_DP 18            // GP# number for USB D+
#define USB_DM (USB_DP+1)   // GP# number for USB D- (always D+ GP# + 1)
#define BUZZER_PIN 28       // GP# number for positive of passive piezo buzzer
#define DISPLAY_ROTATION 0  // 0|1|2|3 where 0 means GND pin is bottom left
#define SUN_MBAUD 9600      // 1200|2400|4800|9600 where higher is smoother and more responsive

// the following must be GP# numbers valid for...
#define DISPLAY_SCL 17      // ...I2C0 SCL: SCL pin of SSD1306
#define DISPLAY_SDA 16      // ...I2C0 SDA: SDA pin of SSD1306

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

// send output over Serial1 instead of Serial (disables Sun keyboard interface)
#define PICOPROBE_ENABLE
#if !defined(PICOPROBE_ENABLE)
#define SUNK_ENABLE
#endif
#define PICOPROBE_BAUD 115200 // values other than 9600 may require PuTTY/minicom

// fake Sun host for loopback testing (disables Sun mouse interface)
// #define FAKE_SUN_ENABLE
#if !defined(FAKE_SUN_ENABLE)
#define SUNM_ENABLE
#endif

// the following must be GP# numbers valid for...
#define FAKE_SUN_KRX 8      // ...UART1 TX: connect to SUN_KRX
#define FAKE_SUN_KTX 9      // ...UART1 RX: connect to SUN_KTX
#define PICOPROBE_TX 0      // ...UART0 TX: connect to picoprobe GP5
#define PICOPROBE_RX 1      // ...UART0 RX: connect to picoprobe GP4
