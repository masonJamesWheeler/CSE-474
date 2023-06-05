/**
 * @file sensors.h
 * @brief Header file containing function prototypes and macros for sensor reading
 * @details This file contains the function prototypes and macros needed for reading from three sensors and performing an action if any of them is high.
 * @authors
 * - Mason Wheeler
 * - Joey Pirich
 */

// Define the delay for blinking (in milliseconds) and the threshold for joystick movement.
#define BLINK_DELAY 500  
#define JOY_THRESHOLD 200  

// Define the pins for the second and third sensors.
#define SENSOR2_PIN 3
#define SENSOR3_PIN 4

// Define the pins for the joystick. JOY_X and JOY_Y are the axes, and JOY_SW is the switch.
#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 6  

// Define the alphabet for scrolling through letters, and the dimensions of the LCD.
#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ! "
#define LCD_COLS 16
#define LCD_ROWS 2

// Define the pin connections for the LCD.
#define RS 30
#define EN 31
#define D4 5
#define D5 32
#define D6 3
#define D7 2

// Define the pin for the clock switch.
#define CLOCK_SWITCH 39

// Define the pins for the rotary encoder. ROTARY_CLK and ROTARY_DT are the rotary, and ROTARY_SW is the switch.
#define ROTARY_CLK 47
#define ROTARY_DT 45
#define ROTARY_SW 43

// Define the pins for a button, a buzzer, and an LED.
#define BUTTON_PIN A2
#define BUZZER_PIN 7
#define LED_PIN 49

// Define the bits for the 7-segment display.
#define BIT0 1<<0
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7

// Define the segments for the 7-segment display.
#define S0 0b1110 << 4
#define S1 0b1101 << 4
#define S2 0b1011 << 4
#define S3 0b0111 << 4

// Define the OffBoard LED
#define OFFBOARD_LED_PIN 37

// Define the lookup table for the 7-segment display. This table maps each digit to its representation on the display.
byte seven_seg_digits[10][7] = { { 1,1,1,1,1,1,0 },  // = 0
                                 { 0,1,1,0,0,0,0 },  // = 1
                                 { 1,1,0,1,1,0,1 },  // = 2
                                 { 1,1,1,1,0,0,1 },  // = 3
                                 { 0,1,1,0,0,1,1 },  // = 4
                                 { 1,0,1,1,0,1,1 },  // = 5
                                 { 1,0,1,1,1,1,1 },  // = 6
                                 { 1,1,1,0,0,0,0 },  // = 7
                                 { 1,1,1,1,1,1,1 },  // = 8
                                 { 1,1,1,0,0,1,1 }   // = 9
                               };

/**
 * @brief Sends a command to the LCD.
 * 
 * @param cmd The command to send.
 * 
 * @return void.
 */
void lcdCommand(uint8_t cmd);

/**
 * @brief Sends data to the LCD.
 * 
 * @param data The data to send.
 * 
 * @return void.
 */
void lcdData(uint8_t data);

/**
 * @brief Initializes the LCD.
 * 
 * @return void.
 */
void lcdInit();

/**
 * @brief Task to read joystick data.
 * 
 * @param pvParameters A pointer to task parameters.
 * 
 * @return void.
 */
void TaskJoyStick(void * pvParameters);

/**
 * @brief Task to control the LCD.
 * 
 * @param pvParameters A pointer to task parameters.
 * 
 * @return void.
 */
void TaskLCD(void * pvParameters);

/**
 * @brief Task to handle countdown operation.
 * 
 * @param pvParameters A pointer to task parameters.
 * 
 * @return void.
 */
void TaskCountdown(void * pvParameters);


/**
 * @brief Clears the LCD.
 * 
 * @return void.
 */
void lcdClear();

/**
 * @brief Sends a command to the LCD.
 * 
 * @param cmd The command to send.
 * 
 * @return void.
 */
void lcdSetCursor(uint8_t col, uint8_t row);

/**
 * @brief Prints a string to the LCD.
 * 
 * @param str The string to print.
 * 
 * @return void.
 */
void lcdPrint(const char* str);

/**
 * @brief Initializes the joystick, sensors, buttons, buzzer, LED, rotary encoder, 7-segment display, and LCD screen.
 * 
 * @return void.
 */
void setup();

/**
 * @brief Task function that reads input from the joystick and updates the selected option or letter.
 * 
 * @param pvParameters void pointer to task parameters.
 * 
 * @return void.
 */
void TaskJoyStick(void *pvParameters);

/**
 * @brief Task function that displays information on an LCD screen and handles user input from a joystick.
 * 
 * @param pvParameters void pointer to task parameters.
 * 
 * @return void.
 */
void TaskLCD(void *pvParameters);

/**
 * @brief Task function that counts down from a specified time and displays the remaining time on a 7-segment display.
 * 
 * @param pvParameters void pointer to task parameters.
 * 
 * @return void.
 */
void TaskCountdown(void *pvParameters);

/**
 * @brief Task function that controls a buzzer and LED based on the state of a sensor.
 * 
 * @param pvParameters void pointer to task parameters.
 * 
 * @return void.
 */
void TaskBuzzerAndLED(void *pvParameters);

/**
 * @brief Task function that reads input from a rotary encoder and updates the selected option or letter.
 * 
 * @param pvParameters void pointer to task parameters.
 * 
 * @return void.
 */
void TaskRotaryEncoder(void *pvParameters);
