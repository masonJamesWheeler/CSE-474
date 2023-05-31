/**
 * @file DDS.h
 * @brief Header file for DDS project
 * @authors 
 *  - Mason Wheeler
 * - Joey Pirich
 */

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
#define PLAY_DURATION 200
#define ADURATION 2000
#define CDURATION 10000
#define PAUSE_DURATION 4000
#define PICKUP_TIME long ((NMELODY * PLAY_DURATION) + PAUSE_DURATION)

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

#define NMELODY long(72)
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

#define NTASKS 10

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
 * @brief Suspends the execution of the calling task for a specified amount of time
 * 
 * @param t Time in milliseconds to sleep for
 */
void sleep_474(long t);

/**
 * @brief Synchronizes the tasks with the system clock
 * 
 */
void schedule_sync();

/**
 * @brief Copies the contents of one TCB to another
 * 
 * @param dst Destination TCB to copy to
 * @param src Source TCB to copy from
 */
void copy_tcb(tcb * dst, tcb * src);

/**
 * @brief Terminates the calling task
 * 
 */
void task_self_quit();

/**
 * @brief Starts a new task
 * 
 * @param task Pointer to the TCB of the task to start
 */
void task_start(tcb * task);

/**
 * @brief Loads a task into the system
 * 
 * @param fn_ptr Pointer to the function to be executed by the task
 * @param name Name of the task
 */
void task_load(void (*fn_ptr)(), const char * name);

/**
 * @brief Finds a dead task by name
 * 
 * @param name Name of the task to find
 * @return tcb* Pointer to the TCB of the dead task, or NULL if not found
 */
tcb * find_dead_task(const char * name);

/**
 * @brief Sets the frequency of the output compare register 4A
 * 
 * @param freq Frequency to set in Hz
 */
void setOC4AFreq(uint32_t freq);

/**
 * @brief Task 2 function
 * 
 */
void task2();

/**
 * @brief Task 4 function
 * 
 */
void task4();

/**
 * @brief Task 4.1 function
 * 
 */
void task4_1();

/**
 * @brief Task 4.2 function
 * 
 */
void task4_2();

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
 * @brief Sets up the display
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
