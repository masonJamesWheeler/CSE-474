/**
 * @file SRRI.h
 * @brief Header file containing definitions and function prototypes for the SRRI project
 * 
 * This file contains definitions for musical notes as frequencies, the size of the melody array, an array of notes that make up a melody,
 * bit masks, timer registers, LED and speaker ports, and durations for flashing, playing, and pausing. It also contains a lookup table for 
 * the 7-segment display and function prototypes for sleeping, scheduling tasks, and setting the frequency of the output compare register.
 * 
 * @authors
 *  - Mason Wheeler
 * - Joey Pirich
 */

// Defines musical notes as frequencies
#define NOTE_C  262
#define NOTE_CS 277
#define NOTE_D  294
#define NOTE_DS 311
#define NOTE_E  330
#define NOTE_F  349
#define NOTE_FS 370
#define NOTE_G  392
#define NOTE_GS 415
#define NOTE_A  440
#define NOTE_AS 466
#define NOTE_B  494
#define NOTE_C5 523

// Defines the size of the melody array
#define NMELODY 72

// An array of notes that make up a melody
int melody[] = {
  NOTE_E, NOTE_E, 0, NOTE_E, 0, NOTE_C, NOTE_E, 0,
  NOTE_G, 0, 0,  0, NOTE_G, 0, 0, 0,
  NOTE_C5, 0, 0, NOTE_G, 0, 0, NOTE_E, 0,
  0, NOTE_A, 0, NOTE_B, 0, NOTE_AS, NOTE_A, 0,
  NOTE_G, NOTE_E, NOTE_G, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5,
  0, 0, NOTE_G, 0, 0, NOTE_E, 0, 0,
  NOTE_G, 0, 0, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5
};


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

#define TIMER_REG_A TCCR4A
#define TIMER_REG_B TCCR4B
#define TIMER_COUNTER TCNT4H
#define PRESCALER OCR4A
#define LED_DDR DDRL
#define LED_PORT PORTL
#define SPEAKER_DDR DDRH
#define SPEAKER_PORT PORTH

#define FLASH_DURATION 250
#define NFLASH 3
#define PLAY_DURATION 200
#define ADURATION 2000
#define CDURATION 10000
#define PAUSE_DURATION 4000
#define PICKUP_TIME (NMELODY * PLAY_DURATION) + PAUSE_DURATION

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

#define NTASKS 10


/**
 * @brief Suspends the execution of the calling task for a specified time
 * 
 * @param t The time to sleep in milliseconds
 */
void sleep_474(long t);

/**
 * @brief Schedules the next task to run
 * 
 */
void schedule_sync();

/**
 * @brief Sets the frequency of the output compare register 4A
 * 
 * @param freq The frequency to set the output compare register to
 */
void setOC4AFreq(uint32_t freq);

/**
 * @brief The first task to be executed
 * 
 */
void task1();

/**
 * @brief The second task to be executed
 * 
 */
void task2();

/**
 * @brief The third task to be executed
 * 
 */
void task3();

/**
 * @brief Converts an integer value to an array of 7 digits
 * 
 * @param digits The array to store the converted digits
 * @param val The integer value to convert
 */
void convert(int * digits, int val);

/**
 * @brief Sends an array of 7 bytes to the 7-segment display
 * 
 * @param arr The array of bytes to send to the display
 */
void send7(byte arr[7]);

/**
 * @brief Sets up the interrupts for the program
 * 
 */
void interruptSetup();

/**
 * @brief Sets up the speaker for the program
 * 
 */
void speakerSetup();

/**
 * @brief Sets up the LED for the program
 * 
 */
void ledSetup();

/**
 * @brief Sets up the 7-segment display for the program
 * 
 */
void displaySetup();
