/**
 * @file demo4.ino
 * @brief This file contains the code for a scheduler that manages multiple tasks on an Arduino board. 
 * @authors The authors of this file are Mason Wheeler and Joey Pirich.
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
  displaySetup();

  // populate task array
  for (int i = 0; i < NTASKS; i++) {
    taskArr[i] = NULL;
    timeArr[i] = 0;
    stateArr[i] = READY;
    sleepArr[i] = 0;
  }
  taskArr[0] = task1;
  taskArr[1] = task2;
  taskArr[2] = task3;
  taskArr[NTASKS-2] = schedule_sync;
  // Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

/// Scheduler Loop
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
 * @brief Timer 3 Compare A Interrupt Service Routine (ISR)
 * 
 * This ISR sets the sFlag to DONE when the Timer 3 Compare A interrupt is triggered.
 * 
 * @param TIMER3_COMPA_vect Interrupt vector for Timer 3 Compare A
 * 
 * @return void
 */
ISR(TIMER3_COMPA_vect) {
  sFlag = DONE;
}


/**
 * @brief Sleeps the current task for a specified amount of time
 * 
 * This function sets the sleep time of the current task to the specified time and changes its state to SLEEPING.
 * 
 * @param t The amount of time to sleep the current task for
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
 * @brief Synchronizes the scheduler with the timer
 * 
 * This function updates the remaining sleep time of each sleeping task and wakes up any sleeping tasks that have finished sleeping.
 * It also updates the time elapsed for each task and resets the sFlag.
 * 
 * @param None
 * 
 * @return void
 */
void schedule_sync() {
  // wait for sFlag to be set to DONE
  while (sFlag == PENDING) {
    3 + 5; // do nothing
  }

  // update each task's sleep time and state
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

    // update time elapsed for each task
    timeArr[i] += 2;
  }

  // reset sFlag
  sFlag = PENDING;
  return;
}


/**
 * @brief Sets the OCR4A to make the clock cycle frequency the same as the input frequency
 * 
 * This function calculates the prescaler value based on the input frequency and sets the OCR4A register accordingly.
 * 
 * @param freq The desired frequency of the clock cycle
 * 
 * @return void
 */
void setOC4AFreq(uint32_t freq) {
  // calculate the prescaler value based on the input frequency
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  // reset the timer counter
  TIMER_COUNTER = 0;
}

/**
 * @brief Flashes an LED on pin 47 for a specified duration
 * 
 * This function flashes an LED on pin 47 for a specified duration. It also resets the time elapsed for the task if a reset signal is received.
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

  if (timeArr[currTask] < (2 * FLASH_DURATION) + 1) {
    LED_PORT &= ~BIT2;
    sleep_474(750);
    return;
  }

  if (timeArr[currTask] >= 1000) {
    timeArr[currTask]= 0;
  }

  return;
}
/**
 * @file demo4.ino
 * @brief This file contains the implementation of the task2 function.
 * 
 * This function plays a melody using the tone function and stops playing for 4 seconds before starting again.
 * 
 */

/**
 * @brief Plays a melody using the tone function and stops playing for 4 seconds before starting again.
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
 * @file demo4.ino
 * @brief This file contains the implementation of the convert function.
 * 
 * This function takes an integer value and separates its digits into an array.
 * 
 */

/**
 * @brief Separates the digits of an integer value into an array.
 * 
 * @param digits An integer array to store the digits of the value.
 * @param val The integer value to separate into digits.
 * 
 * @return void
 */
void convert(int * digits, int val) {
  // Separate the digits of the value and store them in the array
  digits[0] = val % 10;
  digits[1] = (val/10) % 10;
  digits[2] = (val/100) % 10;
  digits[3] = (val/1000) % 10;
}

/**
 * @file demo4.ino
 * @brief This file contains the implementation of task3 function.
 * 
 * This function displays a count on a 7-segment display.
 * 
 */

/**
 * @brief Displays a count on a 7-segment display.
 * 
 * @param None
 * 
 * @return void
 */
void task3() {
  static int count; // static variable to keep track of count
  static int digits[4]; // static array to store digits of count
  static int displayStates[4] = {S0, S1, S2, S3}; // static array to store display states

  // take digits out of count
  convert(digits, count); // call convert function to separate digits of count

  // display count on the 7seg display
  for (int h = 0; h < 5; h++) { // loop through 5 times to display each digit
    for (int i = 0; i < 4; i++) { // loop through 4 times to display each segment of the digit
      int pin = 10 + i; // calculate the pin number for the segment
      if ((timeArr[currTask] / 5) >= (4 * h) + i && (timeArr[currTask] / 5) < (4 * h) + (i + 1)) {
        // if it's time to display the segment, turn on the 7-segment display and the specified digit
        PORTB = displayStates[i];
        byte *disp = seven_seg_digits[digits[i]];
        send7(disp);
        sleep_474(2);
        return;
      }
    }
  }

  // reset
  if (timeArr[currTask] >= 100) { // if it's time to reset
    timeArr[currTask] = 0; // reset the time
    count = count == 9999 ? 0 : count + 1; // increment the count or reset it if it's at its maximum value
  }
}

/**
 * @file demo4.ino
 * @brief This file contains the implementation of the send7 function.
 * 
 * This function takes an array of 7 bytes and sends them to the 7-segment display.
 * 
 */

/**
 * @brief Sends an array of 7 bytes to the 7-segment display.
 * 
 * @param arr An array of 7 bytes to send to the 7-segment display.
 * 
 * @return void
 */
void send7(byte arr[7]) {
  int data = 0; // initialize a variable to store the data to be sent to the 7-segment display
  for (int i = 0; i < 7; i++) { // loop through each byte in the array
    data |= (arr[i] << i); // set the corresponding bit in the data variable to the value of the byte
  }
  PORTA = data; // send the data to the 7-segment display
}


/**
 * @brief Sets up the timer to generate interrupts at a specified frequency.
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
 * @file demo4.ino
 * @brief This file contains the implementation of the speakerSetup function.
 * 
 * This function sets up the timer to generate a square wave on the speaker pin.
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

  // set the prescaler of the timer,
  // which follows the formula:
  //
  //  f_out = f_i/o / 2 * N * (PRESCALER + 1)
  //
  // because f_i/o = 16Mhz, we'll just set N = 1
  TIMER_REG_B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  // OC4A is tied to pin 6, which is controlled by PH3
  // set pin 6 as an output pin
  SPEAKER_DDR |= BIT3;
}


/**
 * @brief Sets up the pins for the 7-segment display and digit selectors.
 * 
 * @param None
 * 
 * @return void
 */
void displaySetup() {
  // set up digit selectors
  DDRB |= BIT7 | BIT6 | BIT5 | BIT4; // set pins 4-7 on PORTB as output pins

  // set up 7-seg outputs
  DDRA |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0; // set pins 0-7 on PORTA as output pins

  PORTB |= BIT7 | BIT6 | BIT5 | BIT4; // set pins 4-7 on PORTB to HIGH
}


/**
 * @file demo4.ino
 * @brief This file contains the implementation of the ledSetup function.
 * 
 * This function sets up the output pin for the LED used in task1.
 * 
 * @param None
 * 
 * @return void
 */
void ledSetup() {
  // set output pin for LED used in task1
  LED_DDR |= BIT2;
}

