/**
 * @file demo2.ino
 * @brief This file was authored by Mason Wheeler and Joey Pirich.
 * 
 * This file contains the implementation of a scheduler loop that runs multiple tasks in a cooperative multitasking fashion.
 * It also includes functions for putting tasks to sleep and waking them up after a specified amount of time.
 * 
 */
#include "SRRI.h"

int reset1 = 0;
int reset2 = 0;

void (* taskArr[NTASKS]) ();
volatile int sFlag = PENDING;
volatile int currTask;
volatile int sleepArr[NTASKS];
volatile int stateArr[NTASKS];
volatile long timeArr[NTASKS];

/// Initializes all necessary i/o parts
void setup() {
  // get all our outputs set up
  interruptSetup();
  speakerSetup();
  ledSetup();

  // populate task array
  for (int i = 0; i < NTASKS; i++) {
    taskArr[i] = NULL;
    timeArr[i] = 0;
    stateArr[i] = READY;
    sleepArr[i] = 0;
  }
  taskArr[0] = task1;
  taskArr[1] = task2;
  taskArr[NTASKS-2] = schedule_sync;
  Serial.begin(9600);
}

/// Scheduler loop
void loop() {
  for (int i = 0; i < NTASKS; i++) {
    if (taskArr[i] != NULL && stateArr[i] == READY) {
      // start up this task
      stateArr[i] = RUNNING;
      currTask = i;
      (*taskArr[i])();


      // tear down
      if (stateArr[i] == RUNNING) stateArr[i] = READY;
    }
  }
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the Timer 3 Interrupt function.
 * 
 * This function is called by the Timer 3 Interrupt and sets the sFlag to DONE.
 * 
 */

/**
 * @brief Timer 3 Interrupt function.
 * 
 * This function is called by the Timer 3 Interrupt and sets the sFlag to DONE.
 * 
 */
ISR(TIMER3_COMPA_vect) {
  sFlag = DONE; // Set the sFlag to DONE
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the sleep_474 function.
 * 
 * This function puts the current task to sleep for a specified amount of time.
 * 
 */

/**
 * @brief Puts the current task to sleep for a specified amount of time.
 * 
 * @param t The amount of time to sleep for, in milliseconds.
 * 
 * @return void
 */
void sleep_474(long t) {
  // sleep array @ [function index], set value to t
  sleepArr[currTask] = t;
  // state array @ [function index], set state to SLEEPING
  stateArr[currTask] = SLEEPING;
  return;
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the schedule_sync function.
 * 
 * This function updates the remaining sleep time for each sleeping task and wakes up any sleeping tasks
 * whose remaining sleep time has elapsed. It also updates the timeArr for each task and resets the sFlag.
 * 
 */

/**
 * @brief Updates the remaining sleep time for each sleeping task and wakes up any sleeping tasks
 * whose remaining sleep time has elapsed. It also updates the timeArr for each task and resets the sFlag.
 * 
 * @return void
 */
void schedule_sync() {
  // wait for sFlag to be set to DONE
  while (sFlag == PENDING) {
    3 + 5;
  }

  // update sleep time and wake up sleeping tasks
  for (int i = 0; i < NTASKS; i++) {
    // update remaining sleep time
    if (stateArr[i] == SLEEPING) {
      sleepArr[i] -= 2;
      // wake up any sleeping tasks
      if (sleepArr[i] < 2) {
        // reset to t = 0 in sleep array
        sleepArr[i] = 0;
        // change corresponding state from SLEEPING to READY
        stateArr[i] = READY;
      }
    }

    // update timeArr for each task
    timeArr[i] += 2;
  }

  // reset sFlag
  sFlag = PENDING;
  return;
}
/**
 * @file demo2.ino
 * @brief This file contains the implementation of the setOC4AFreq function.
 * 
 * This function sets the OCR4A to make the clock cycle frequency the same as the input frequency.
 * 
 */

/**
 * @brief Sets the OCR4A to make the clock cycle frequency the same as the input frequency.
 * 
 * @param freq The desired frequency of the clock cycle.
 * 
 * @return void
 */
void setOC4AFreq(uint32_t freq) {
  // Calculate the prescaler value based on the desired frequency
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  
  // Reset the timer counter
  TIMER_COUNTER = 0;
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the task1 function.
 * 
 * This function flashes an LED on pin 47 for a certain duration, and then turns it off for a certain duration.
 * 
 */

/**
 * @brief Flashes an LED on pin 47 for a certain duration, and then turns it off for a certain duration.
 * 
 * @param None
 * 
 * @return void
 */
void task1() {
  // reset everything given a reset signal
  if (reset1) {
    LED_PORT |= BIT2;
    timeArr[currTask] = 0;
    reset1 = 0;
    return;
  }

  // flash led on pin 47 for FLASH_DURATION
  if (timeArr[currTask] < (1 * FLASH_DURATION) + 1) {
    LED_PORT |= BIT2;
    sleep_474(250);
    return;
  }

  // turn off led for PAUSE_DURATION
  if (timeArr[currTask] < (2 * FLASH_DURATION) + 1) {
    LED_PORT &= ~BIT2;
    sleep_474(750);
    return;
  }

  // reset timeArr after 1000ms
  if (timeArr[currTask] >= 1000) {
    timeArr[currTask]= 0;
  }

  return;
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the task2 function.
 * 
 * This function plays a melody using the setOC4AFreq function.
 * 
 */

/**
 * @brief Plays a melody using the setOC4AFreq function.
 * 
 * @param None
 * 
 * @return void
 */
void task2() {
  // reset everything given a reset signal
  if (reset2) {
    setOC4AFreq(0);
    timeArr[currTask] = 0;
    reset2 = 0;
    return;
  }

  // play tone
  for (int i = 0; i < NMELODY; i++) {
    if ( timeArr[currTask] >= ((unsigned long) i * PLAY_DURATION) &&
      timeArr[currTask] < (((unsigned long) i + 1) * PLAY_DURATION) ) {
      setOC4AFreq(melody[i]);
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // stop playing for 4 seconds
  if (timeArr[currTask] < PICKUP_TIME) {
    setOC4AFreq(0);
    sleep_474(PAUSE_DURATION);
    return;
  }

  // start playing after 4 seconds
  for (int i = 0; i < NMELODY; i++) {
    if (timeArr[currTask] >= (PICKUP_TIME + ((unsigned long) i) * PLAY_DURATION) &&
        timeArr[currTask] < (PICKUP_TIME + ((unsigned long) i + 1) * PLAY_DURATION)) {
      setOC4AFreq(melody[i]);
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // reset
  if (timeArr[currTask] >= (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION)) {
    timeArr[currTask] = 0;
  }
}
/**
 * @file demo2.ino
 * @brief This file contains the implementation of the interruptSetup function.
 */

/**
 * @brief Sets up the timer interrupt for the speaker.
 * 
 * This function sets up the timer interrupt for the speaker by configuring the timer
 * to work in CTC mode, enabling interrupts on counter A, setting the frequency of the timer,
 * and enabling interrupts.
 * 
 * @param None
 * 
 * @return void
 */
void interruptSetup() {
  // reset settings
  TCCR3A = 0;
  TCCR3B = 0;

  // set timer to work in CTC mode
  TCCR3A |= (0 << WGM31) | (0 << WGM30);
  TCCR3B |= (0 << WGM33) | (1 << WGM32);
  TCCR3A |= (0 << COM3A1) | (1 << COM3A0);

  // enable interrupt on counter A
  TIMSK3 |= (1 << OCIE3A);

  // set frequency of timer
  TCCR3B |= (0 << CS32) | (0 << CS31) | (1 << CS30);
  // TCCR3B |= (1 << CS32) | (0 << CS31) | (1 << CS30); // 1024
  // OCR3A =  7812; // should make around 1 hz
  OCR3A = 32000;
  TCNT3H = 0;

  // enable interrupts
  SREG |= (1<<7);
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the speakerSetup function.
 */

/**
 * @brief Sets up the speaker timer for generating sound.
 * 
 * This function sets up the speaker timer to work in CTC mode, toggle mode, and with a specific
 * prescaler. It also sets the output pin for the speaker.
 * 
 * @param None
 * 
 * @return void
 */
void speakerSetup() {
  // clear timer settings
  TIMER_REG_A = 0;
  TIMER_REG_B = 0;

  // set our timer to work in CTC mode
  TIMER_REG_A |= (0 << WGM41) | (0 << WGM40);
  TIMER_REG_B |= (0 << WGM43) | (1 << WGM42);

  // set timer to work in toggle mode
  TIMER_REG_A |= (0 << COM4A1) | (1 << COM4A0);

  // disable interrupts on the timer
  TIMSK4 = 0;

  TIMER_REG_B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  // OC4A is tied to pin 6, which is controlled by PH3
  // set pin 6 as an output pin
  SPEAKER_DDR |= BIT3;
}

/**
 * @file demo2.ino
 * @brief This file contains the implementation of the ledSetup function.
 */

/**
 * @brief Sets up the LED output pin for task1.
 * 
 * This function sets up the LED output pin for task1 by setting the corresponding
 * pin in the LED_DDR register to be an output pin.
 * 
 * @param None
 * 
 * @return void
 */
void ledSetup() {
  // set output pins for task1
  LED_DDR |= BIT2; // set bit 2 in LED_DDR to 1 to make it an output pin
}
