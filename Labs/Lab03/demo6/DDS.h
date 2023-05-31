/**
 * @file DDS.h
 * @brief Header file for DDS project
 * @authors 
 *  - Mason Wheeler
 * - Joey Pirich
 */


// Define bit masks for each bit in a byte
#define BIT0 1<<0
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7

// Define bit masks for each segment of a 7-segment display
#define S0 0b1110 << 4
#define S1 0b1101 << 4
#define S2 0b1011 << 4
#define S3 0b0111 << 4

// Define register names for the timer and LED pins
#define TIMER_REG_A TCCR4A
#define TIMER_REG_B TCCR4B
#define TIMER_COUNTER TCNT4H
#define PRESCALER OCR4A
#define LED_DDR DDRL
#define LED_PORT PORTL
#define SPEAKER_DDR DDRH
#define SPEAKER_PORT PORTH

// Define durations for flashing, playing, and pausing
#define FLASH_DURATION 250
#define PLAY_DURATION 200
#define PAUSE_DURATION 4000
#define PICKUP_TIME (NMELODY * PLAY_DURATION) + PAUSE_DURATION
#define TASK2_DURATION (2 * (NMELODY * PLAY_DURATION) + PAUSE_DURATION)
#define SMILE_DURATION 2000
#define PAUSE_DURATION_T5 3000

// Define frequencies for each note in the melody
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

// Define the number of notes in the melody
#define NMELODY long(72)

// Define the melody as an array of notes
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

// Define the number of tasks
#define NTASKS 10


// Look-up table for 7-segment display
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

// Look-up table for smiley face
byte smile_seg[4][7] = { { 0,1,0,0,1,0,1 },  // = right
                         { 1,0,0,1,0,0,0 },  // = eye
                         { 1,0,0,1,0,0,0 },  // = eye
                         {0,0,1,0,0,1,1 },  // = left
                       };

typedef enum {READY, RUNNING, SLEEPING, DEAD} STATE;
typedef enum flagState {PENDING, DONE} FLAG;

/**
 * @brief Struct to hold all of a Tasks running state
 * 
 */
typedef struct TCB {
  /// Task name
  char name[20];
  /// Task function
  void (*fn_ptr)();
  /// Task id
  int id;
  /// Number of times task has been run
  int nTimes;
  /// Amount of time in ms the task should sleep for
  int timeSleep;
  /// Current clock time this task sees
  long time;
  /// Running state of this task
  STATE state;
} tcb;

/**
 * @brief Sleeps the current task for a specified amount of time
 * 
 * @param t The amount of time to sleep in milliseconds
 */
void sleep_474(long t);

/**
 * @brief Synchronizes the task scheduler with the timer
 * 
 */
void schedule_sync();

/**
 * @brief Copies the contents of one TCB to another
 * 
 * @param dst The destination TCB
 * @param src The source TCB
 */
void copy_tcb(tcb * dst, tcb * src);

/**
 * @brief Terminates the current task
 * 
 */
void task_self_quit();

/**
 * @brief Starts a task
 * 
 * @param task The task to start
 */
void task_start(tcb * task);

/**
 * @brief Loads a task with a given function and name
 * 
 * @param fn_ptr The function to load
 * @param name The name of the task
 */
void task_load(void (*fn_ptr)(), const char * name);

/**
 * @brief Finds a dead task with a given name
 * 
 * @param name The name of the task to find
 * @return A pointer to the dead task, or NULL if not found
 */
tcb * find_dead_task(const char * name);

/**
 * @brief Sets the frequency of the DDS output
 * 
 * @param freq The frequency to set in Hz
 */
void setOC4AFreq(uint32_t freq);

/**
 * @brief Sends an array of bytes to the 7-segment display
 * 
 * @param arr The array of bytes to send
 */
void send7(byte arr[7]);

/**
 * @brief Converts an integer value to an array of digits
 * 
 * @param digits An array to store the digits in
 * @param val The value to convert
 */
void convert(int * digits, int val);

/**
 * @brief Task 1 function
 * 
 */
void task1();

/**
 * @brief Task 2 function
 * 
 */
void task2();

/**
 * @brief Task 4.2 function
 * 
 */
void task4_2();

/**
 * @brief Task 5 function
 * 
 */
void task5();

/**
 * @brief Task 5.1 function
 * 
 */
void task5_1();

/**
 * @brief Sets up interrupts
 * 
 */
void interruptSetup();

/**
 * @brief Sets up the speaker
 * 
 */
void speakerSetup();

/**
 * @brief Sets up the 7-segment display
 * 
 */
void displaySetup();

/**
 * @brief Sets up the LED
 * 
 */
void ledSetup();

/**
 * @brief Sets up the DDS
 * 
 */
void DDSSetup();
