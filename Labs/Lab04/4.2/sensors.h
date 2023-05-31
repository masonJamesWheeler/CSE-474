/**
 * @file sensors.h
 * @brief Header file containing function prototypes and macros for sensor reading
 * @details This file contains the function prototypes and macros needed for reading from three sensors and performing an action if any of them is high.
 * @author 
 */
// Add these definitions at the top
#define BLINK_DELAY 500  // Blink delay in milliseconds
#define JOY_THRESHOLD 200  // Threshold for joystick movement

#define SENSOR2_PIN 3
#define SENSOR3_PIN 4

// Add these definitions for the joystick pins
#define JOY_X A0  // X-axis pin
#define JOY_Y A1  // Y-axis pin
#define JOY_SW 6  // Switch pin

#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ! "
#define LCD_COLS 16
#define LCD_ROWS 2

// LCD pin connections
#define RS 30  
#define EN 31 
#define D4 5
#define D5 32 
#define D6 3
#define D7 2

// Clock Switch
#define CLOCK_SWITCH 39

// Rotary Encoder Pins
#define ROTARY_CLK 47
#define ROTARY_DT 45
#define ROTARY_SW 43

#define BUTTON_PIN A2  
#define BUZZER_PIN 7  
#define LED_PIN 49    

// Definitions for the 7-segment display
#define BIT0 1<<0
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7

#define S0 0b1110 << 4
#define S1 0b1101 << 4
#define S2 0b1011 << 4
#define S3 0b0111 << 4

// The lookup table for the 7-segment display
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

// define tasks for reading sensors
void TaskJoyStick(void * pvParameters);
void TaskLCD(void * pvParameters);
void TaskCountdown(void * pvParameters);

// functions for manual LCD control
void lcdCommand(uint8_t cmd);
void lcdData(uint8_t data);
void lcdInit();
void lcdClear();
void lcdSetCursor(uint8_t col, uint8_t row);
void lcdPrint(const char* str);