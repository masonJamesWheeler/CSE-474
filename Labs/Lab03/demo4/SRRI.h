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

#define NMELODY 72
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


/// All codes for digits 0-9 on our 7-segment displays
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

// possible states for our tasks to be in
enum state {READY, RUNNING, SLEEPING};

enum flagState {PENDING, DONE};

/**
 * @brief Puts currently running function to sleep for t ms
 * @authors Sunny Hu, Peter Gunarso
 * @param t Number of ms for task to sleep for
 */
void sleep_474(long t);

/**
 * @brief Manages sleep time and clocks each task sees. If a task cannot sleep for any longer, wakes up the task.
 * @authors Sunny Hu, Peter Gunarso
 */
void schedule_sync();

/**
 * @brief Sets the frequency of OC4A to match a given frequency freq
 * @authors Sunny Hu, Peter Gunarso
 * @param freq Desired frequency to set OC4A
 */
void setOC4AFreq(uint32_t freq);

/**
 * @brief Manages task 1 behavior, falshes an LED on and off
 * @authors Sunny Hu, Peter Gunarso
 */
void task1();

/**
 * @brief Manages task 2 behavior, plays the theme from "Close Encounters of the Third Kind" once, pauses for 4 seconds, and plays again.
 * @authors Sunny Hu, Peter Gunarso
 */
void task2();

/**
 * @brief Manages task 3 behavior, counts up by tenths of a second on 7-segment display
 * @author Peter Gunarso
 */
void task3();

/**
 * @brief Converts an up to 4 digit number into an array of 4 ints, each representing a position in the value.
 * @author Sunny Hu
 * @param digits Output parameter, where the digits of val are stored
 * @param val Value to break up
 */
void convert(int * digits, int val);

/**
 * @brief Sends 7 bits to 7-segment display to light corresponding segments.
 * @author Sunny Hu
 * @param arr Bit array to set a 7-segment display
 */
void send7(byte arr[7]);

/**
 * @brief Sets up interrupts to run on timer 3 A, at a frequency of 500hz
 * @authors Sunny Hu, Peter Gunarso
 */
void interruptSetup();

/**
 * @brief Sets up timer and ports needed to drive a speaker using OC4A
 * @author Sunny Hu 
 */
void speakerSetup();

/**
 * @brief Sets up the LED used for this lab
 * @authors Sunny Hu, Peter Gunarso
 */
void ledSetup();

/**
 * @brief Sets up DDR for our 7-segment display
 * @author Sunny Hu
 */
void displaySetup();
