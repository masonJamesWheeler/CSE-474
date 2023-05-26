/**
 * @file demo6.ino
 * @brief This file contains the code for a scheduler loop that runs multiple tasks on an Arduino board. 
 * The authors of this file are Mason Wheeler and Joey Pirich.
 */

#include "DDS.h"

int reset1 = 0;
int reset2 = 0;

tcb taskArr[NTASKS];
tcb deadTasks[NTASKS];
volatile int id;
volatile int currTask;
volatile FLAG sFlag;

/// Initializes everything
void setup() {
  id = 0;
  /// Initalize outputs
  interruptSetup();
  speakerSetup();
  displaySetup();
  ledSetup();

  /// intialize our task tracking arrays
  DDSSetup();

  task_load(task5, "task5");
  task_load(schedule_sync, "schedule_sync");
  task_start(find_dead_task("task5"));
  task_start(find_dead_task("schedule_sync"));
}

/// Scheduler Loop
void loop() {
  for (int i = 0; i < NTASKS; i++) {
    if (taskArr[i].fn_ptr != NULL && taskArr[i].state == READY) {
      // start up this task
      taskArr[i].state = RUNNING;
      currTask = i;
      (*(taskArr[i].fn_ptr))();
      taskArr[i].nTimes++;

      // tear down
      if (taskArr[i].state == RUNNING) taskArr[i].state = READY;
    }
  }
}

/**
 * @brief Timer 3 Interrupt Service Routine (ISR)
 * 
 * This ISR sets the sFlag to DONE when the Timer 3 Compare Match A (COMPA) 
 * interrupt is triggered.
 * 
 * @param none
 * @return none
 */
ISR(TIMER3_COMPA_vect) {
  sFlag = DONE; // set the sFlag to DONE
}

/**
 * @brief Finds a dead task by name
 * 
 * This function searches the deadTasks array for a task with the given name.
 * If found, it returns a pointer to the corresponding tcb struct. If not found,
 * it returns NULL.
 * 
 * @param name The name of the task to find
 * @return A pointer to the corresponding tcb struct, or NULL if not found
 */
tcb * find_dead_task(const char * name) {
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (strcmp(name, deadTasks[i].name) == 0) break; // if the name matches, break out of the loop
  }

  if (i == NTASKS) return NULL; // if we reached the end of the array, the task was not found

  return &(deadTasks[i]); // return a pointer to the corresponding tcb struct
}


/**
 * @brief Copies the contents of one tcb struct to another
 * 
 * This function copies the contents of one tcb struct to another.
 * 
 * @param dst A pointer to the destination tcb struct
 * @param src A pointer to the source tcb struct
 * @return none
 */
void copy_tcb(tcb * dst, tcb * src) {
  // Copy the function pointer
  dst->fn_ptr = src->fn_ptr;
  // Copy the name string
  strcpy(dst->name, src->name);
  // Copy the task ID
  dst->id = src->id;
  // Copy the number of times the task has run
  dst->nTimes = src->nTimes;
  // Copy the task state
  dst->state = src->state;
  // Copy the task time
  dst->time = src->time;
  // Copy the task sleep time
  dst->timeSleep = src->timeSleep;
}


/**
 * @brief Terminates the currently running task and moves it to the deadTasks array
 * 
 * This function finds an open spot in the deadTasks array and copies the contents
 * of the currently running task's tcb struct to that spot. It then sets the state
 * of both the current task and the copied task to DEAD and sets the function pointer
 * of the current task to NULL.
 * 
 * @param none
 * @return none
 */
void task_self_quit() {
  // find a spot in the dead array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (deadTasks[i].fn_ptr == NULL) break;
  }

  if (i == NTASKS) return; // if there's no space, don't do anything

  // copy the current task's tcb struct to the deadTasks array
  copy_tcb(&(deadTasks[i]), &(taskArr[currTask]));
  deadTasks[i].state = DEAD;
  taskArr[currTask].state = DEAD;
  taskArr[currTask].fn_ptr = NULL; // set the function pointer of the current task to NULL
}

/**
 * @brief Starts a task by copying its tcb struct to the taskArr array
 * 
 * This function starts a task by copying its tcb struct to the taskArr array.
 * It first checks if the task is NULL or if it is not in the DEAD state. If either
 * of these conditions are true, the function returns without doing anything. If the
 * task is valid, the function finds an open spot in the taskArr array and copies the
 * tcb struct to that spot. It then sets the state of the copied task to READY and the
 * state of the original task to DEAD. Finally, it sets the function pointer of the
 * original task to NULL.
 * 
 * @param task A pointer to the tcb struct of the task to start
 * @return none
 */
void task_start(tcb * task) {
  // if the task is NULL or not in the DEAD state, return without doing anything
  if (task == NULL || task->state != DEAD) return;

  // find a spot in the taskArr array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (taskArr[i].fn_ptr == NULL) break;
  }

  // if there's no space, return without doing anything
  if (i == NTASKS) return;

  // copy the tcb struct to the taskArr array
  copy_tcb(&(taskArr[i]), task);
  taskArr[i].state = READY;
  task->state = DEAD;
  task->fn_ptr = NULL;
}

/**
 * @brief Loads a new task into the deadTasks array
 * 
 * This function finds an open spot in the deadTasks array and loads a new task
 * into that spot. It sets the name, function pointer, ID, number of times run,
 * sleep time, time, and state of the new task.
 * 
 * @param fn_ptr A pointer to the function that the task will run
 * @param name A string containing the name of the task
 * @return none
 */
void task_load(void (*fn_ptr)(), const char * name) {
  // find the next open slot in the dead task array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (deadTasks[i].fn_ptr == NULL) break;
  }

  // if there's no space, don't do anything
  if (i == NTASKS) return;

  // load up a new task
  strcpy(deadTasks[i].name, name); // set the name of the new task
  deadTasks[i].fn_ptr = fn_ptr; // set the function pointer of the new task
  deadTasks[i].id = id; // set the ID of the new task
  id++; // increment the ID counter
  deadTasks[i].nTimes = 0; // set the number of times run to 0
  deadTasks[i].timeSleep = 0; // set the sleep time to 0
  deadTasks[i].time = 0; // set the time to 0
  deadTasks[i].state = DEAD; // set the state to DEAD
}

/**
 * @brief Puts the current task to sleep for a specified amount of time
 * 
 * This function sets the sleep time of the current task to the specified time t
 * and sets the state of the current task to SLEEPING.
 * 
 * @param t The amount of time to sleep for
 * @return none
 */
void sleep_474(long t) {
  // set the sleep time of the current task to t
  taskArr[currTask].timeSleep = t;
  // set the state of the current task to SLEEPING
  taskArr[currTask].state = SLEEPING;
  return;
}

/**
 * @brief Synchronizes the task scheduler
 * 
 * This function synchronizes the task scheduler by updating the remaining sleep time
 * of each sleeping task and waking up any tasks that have finished sleeping. It also
 * updates the time of each task and resets the sFlag.
 * 
 * @param none
 * @return none
 */
void schedule_sync() {
  // wait for sFlag to be set to PENDING
  while (sFlag == PENDING) {
    3 + 5; // do nothing
  }

  // update each task's remaining sleep time and wake up any sleeping tasks
  for (int i = 0; i < NTASKS; i++) {
    // update remaining sleep time
    if (taskArr[i].state == SLEEPING) {
      taskArr[i].timeSleep -= 2;
      // wake up any sleeping tasks
      if (taskArr[i].timeSleep < 2) {
        // reset to t = 0 in sleep array (not sure if needed)
        taskArr[i].timeSleep = 0;
        // change corresponding state from SLEEPING to READY
        taskArr[i].state = READY;
      }
    }

    // update time of each task
    taskArr[i].time += 2;
  }

  // reset sFlag
  sFlag = PENDING;
  return;
}


/**
 * @brief Task 1 function
 * 
 * This function is responsible for flashing an LED on pin 47 for a specified duration.
 * 
 * @param none
 * @return none
 */
void task1() {
  // reset everything given a reset signal
  if (reset1) {
    LED_PORT |= BIT2; // turn on LED
    taskArr[currTask].time = 0; // reset task time
    reset1 = 0; // reset reset1 flag
    return;
  }

  // flash led on pin 47 for FLASH_DURATION
  if (taskArr[currTask].time < (1 * FLASH_DURATION) + 1) {
    LED_PORT &= ~BIT2; // turn off LED
    sleep_474(250); // sleep for 250ms
    return;
  }

  if (taskArr[currTask].time < (2 * FLASH_DURATION) + 1) {
    LED_PORT |= BIT2; // turn on LED
    sleep_474(750); // sleep for 750ms
    return;
  }

  if (taskArr[currTask].time >= 1000) {
    taskArr[currTask].time = 0; // reset task time
  }

  return;
}

/**
 * @brief Task 2 function
 * 
 * This function is responsible for playing a melody using the setOC4AFreq function.
 * 
 * @param none
 * @return none
 */
void task2() {
  // reset everything given a reset signal
  if (reset2) {
    setOC4AFreq(0); // stop playing
    taskArr[currTask].time = 0; // reset task time
    reset2 = 0; // reset reset2 flag
    return;
  }

  // play tone
  for (int i = 0; i < NMELODY; i++) {
    if ( taskArr[currTask].time >= ((unsigned long) i * PLAY_DURATION) &&
      taskArr[currTask].time < (((unsigned long) i + 1) * PLAY_DURATION) ) {
      setOC4AFreq(melody[i]); // play melody
      sleep_474(PLAY_DURATION); // sleep for duration of melody
      return;
    }
  }

  // stop playing for 4 seconds
  if (taskArr[currTask].time < PICKUP_TIME) {
    setOC4AFreq(0); // stop playing
    sleep_474(PAUSE_DURATION); // sleep for pause duration
    return;
  }

  // start playing after 4 seconds
  for (int i = 0; i < NMELODY; i++) {
    if (taskArr[currTask].time >= (PICKUP_TIME + ((unsigned long) i) * PLAY_DURATION) &&
        taskArr[currTask].time < (PICKUP_TIME + ((unsigned long) i + 1) * PLAY_DURATION)) {
      setOC4AFreq(melody[i]); // play melody
      sleep_474(PLAY_DURATION); // sleep for duration of melody
      return;
    }
  }

  // reset
  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION)) {
    taskArr[currTask].time = 0; // reset task time
    task_self_quit(); // quit task
  }
}

/**
 * @file demo6.ino
 * @brief Task 4_2 function
 * 
 * This function is responsible for displaying a countdown on a 7-segment display.
 * 
 * @param none
 * @return none
 */
void task4_2() {
  static long time; // static variable to keep track of time
  static int count; // static variable to keep track of count
  static int digits[4]; // static array to store digits of the count
  static int displayStates[4] = {S0, S1, S2, S3}; // static array to store display states of the 7-segment display

  // take digits out of count
  convert(digits, (PAUSE_DURATION_T5 / 100) - count); // convert count to digits and store in digits array

  // display count on the 7seg display
  for (int h = 0; h < 5; h++) { // loop through each digit of the count
    for (int i = 0; i < 4; i++) { // loop through each 7-segment display digit
      if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
        // turn 7seg & specified digit on
        PORTB = displayStates[i]; // set the display state of the 7-segment display
        byte *disp = seven_seg_digits[digits[i]]; // get the byte array for the digit to be displayed
        send7(disp); // display the digit on the 7-segment display
        sleep_474(2); // sleep for 2ms
        return; // return to main loop
      }
    }
  }

  // reset
  if (taskArr[currTask].time >= 100) { // if the time has reached 100ms
    time += taskArr[currTask].time; // add the current task time to the total time
    taskArr[currTask].time = 0; // reset the task time
    count = count == 9999 ? 0 : count + 1; // increment the count or reset it if it has reached 9999
  }

  if (time >= PAUSE_DURATION_T5) { // if the total time has reached the pause duration
    PORTB = 0xFF; // turn off the 7-segment display
    time = 0; // reset the total time
    taskArr[currTask].time = 0; // reset the task time
    count = 0; // reset the count
    task_self_quit(); // quit the task
  }
}


/**
 * @file demo6.ino
 * @brief Task 5 function
 * 
 * This function is responsible for coordinating the execution of multiple tasks.
 * 
 * @param none
 * @return none
 */
void task5() {
  Serial.begin(9600); // initialize serial communication

  // load tasks if this is the first time running the function
  if (taskArr[currTask].nTimes == 0) {
    task_load(task1, "task1"); // load task1
    task_start(find_dead_task("task1")); // start task1
    task_load(task2, "task2"); // load task2
    task_load(task4_2, "countdown"); // load countdown task
    task_load(task5_1, "smile"); // load smile task
  }
  
  // execute task2 for TASK2_DURATION
  if (taskArr[currTask].time < TASK2_DURATION) {
    task_start(find_dead_task("task2")); // start task2
    sleep_474(TASK2_DURATION); // sleep for TASK2_DURATION
    return; // return to main loop
  }

  // execute countdown task for PAUSE_DURATION_T5
  if (taskArr[currTask].time >= TASK2_DURATION &&
      taskArr[currTask].time < (TASK2_DURATION + PAUSE_DURATION_T5)) {
    task_start(find_dead_task("countdown")); // start countdown task
    setOC4AFreq(0); // stop playing melody
    sleep_474(PAUSE_DURATION_T5); // sleep for PAUSE_DURATION_T5
    return; // return to main loop
  }

  // execute task2 for TASK2_DURATION
  if (taskArr[currTask].time >= (TASK2_DURATION + PAUSE_DURATION_T5) &&
      taskArr[currTask].time < ((2 * (TASK2_DURATION)) + PAUSE_DURATION_T5)) {
    task_start(find_dead_task("task2")); // start task2
    sleep_474(TASK2_DURATION); // sleep for TASK2_DURATION
    return; // return to main loop
  }

  // execute smile task for SMILE_DURATION
  if (taskArr[currTask].time >= ((2 * (TASK2_DURATION) + PAUSE_DURATION_T5)) &&
      taskArr[currTask].time < ((2 * (TASK2_DURATION) + PAUSE_DURATION_T5 + SMILE_DURATION))) {
    setOC4AFreq(0); // stop playing melody
    task_start(find_dead_task("smile")); // start smile task
    sleep_474(SMILE_DURATION); // sleep for SMILE_DURATION
    return; // return to main loop
  }

  // quit task
  if (taskArr[currTask].time >= ((2 * (TASK2_DURATION) + PAUSE_DURATION_T5 + SMILE_DURATION))) {
    task_self_quit(); // quit task
    return; // return to main loop
  }
}

/**
 * @file demo6.ino
 * @brief Task 5.1 function for displaying count on 7-segment display and quitting task after SMILE_DURATION
 * 
 * This function is responsible for displaying the count on the 7-segment display and quitting the task after SMILE_DURATION.
 * 
 * @param none
 * @return none
 */
void task5_1() {
  static long time; // static variable to keep track of time
  static int displayStates[4] = {S0, S1, S2, S3}; // array of 7-segment display states

  // display count on the 7seg display
  for (int h = 0; h < 5; h++) { // loop through each digit of the count
    for (int i = 0; i < 4; i++) { // loop through each 7-segment display digit
      if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
        // if the current count is within the range of the current 7-segment display digit
        // turn 7seg & specified digit on
        PORTB = displayStates[i]; // turn on the specified 7-segment display digit
        byte *disp = smile_seg[i]; // get the byte array for the specified digit
        send7(disp); // send the byte array to the 7-segment display
        sleep_474(2); // sleep for 2ms
        return; // return to main loop
      }
    }
  }

  // reset
  if (taskArr[currTask].time >= 100) { // if the current count has reached 100
    time += taskArr[currTask].time; // add the current time to the static time variable
    taskArr[currTask].time = 0; // reset the current count
  }
  if (time >= SMILE_DURATION) { // if the total time has reached SMILE_DURATION
    time = 0; // reset the total time
    PORTB = 0xFF; // turn off the 7-segment display
    task_self_quit(); // quit the task
  }
}

/**
 * @file demo6.ino
 * @brief Function for setting up the interrupt timer
 * 
 * This function sets up the interrupt timer to work in CTC mode and enables interrupts on counter A.
 * It also sets the frequency of the timer and enables interrupts.
 * 
 * @param none
 * @return none
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
 * @file demo6.ino
 * @brief Function for setting the OCR4A to make the clock cycle frequency the same as the input freq
 * 
 * This function sets the OCR4A to make the clock cycle frequency the same as the input freq.
 * 
 * @param freq The frequency to set the OCR4A to
 * @return none
 */
void setOC4AFreq(uint32_t freq) {
  // calculate the prescaler value based on the input frequency
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  // reset the timer counter
  TIMER_COUNTER = 0;
}

/**
 * @file demo6.ino
 * @brief Function for setting up the speaker
 * 
 * This function sets up the speaker to work in fast PWM mode and enables interrupts on counter A.
 * It also sets the frequency of the speaker and enables interrupts.
 * 
 * @param none
 * @return none
 */
void speakerSetup() {
  // reset settings
  TIMER_REG_A = 0;
  TIMER_REG_B = 0;

  // set speaker to work in fast PWM mode
  TIMER_REG_A |= (0 << WGM41) | (0 << WGM40);
  TIMER_REG_B |= (0 << WGM43) | (1 << WGM42);

  // set speaker output to toggle on compare match
  TIMER_REG_A |= (0 << COM4A1) | (1 << COM4A0);

  // disable all interrupts
  TIMSK4 = 0;

  // set frequency of speaker
  TIMER_REG_B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  // set speaker pin as output
  SPEAKER_DDR |= BIT3;
}

/**
 * @file demo6.ino
 * @brief Functions for setting up and controlling the display
 */

// Set up the display
void displaySetup() {
  // Set up digit selectors
  DDRB |= BIT7 | BIT6 | BIT5 | BIT4;

  // Set up 7-segment outputs
  DDRA |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;

  // Turn on all digit selectors
  PORTB |= BIT7 | BIT6 | BIT5 | BIT4;
}

/**
 * @brief Sends an array of 7 bytes to the 7-segment display
 * 
 * This function takes an array of 7 bytes and sends them to the 7-segment display.
 * Each byte represents a segment of the display, with the least significant bit
 * representing segment A and the most significant bit representing segment G.
 * 
 * @param arr The array of 7 bytes to send to the display
 * @return none
 */
void send7(byte arr[7]) {
  int data = 0;

  // Convert the array of bytes to a single integer
  for (int i = 0; i < 7; i++) {
    data |= (arr[i] << i);
  }

  // Send the integer to the display
  PORTA = data;
}

/**
 * @file demo6.ino
 * @brief Function for converting an integer to an array of its digits
 * 
 * This function takes an integer value and converts it to an array of its digits.
 * The digits are stored in the input array in reverse order, with the least significant
 * digit in the first element and the most significant digit in the last element.
 * 
 * @param digits The array to store the digits in
 * @param val The integer value to convert to an array of digits
 * @return none
 */
void convert(int * digits, int val) {
  // Calculate each digit of the integer and store it in the input array
  digits[0] = val % 10; // least significant digit
  digits[1] = (val/10) % 10;
  digits[2] = (val/100) % 10;
  digits[3] = (val/1000) % 10; // most significant digit
}

/**
 * @file demo6.ino
 * @brief Functions for setting up and controlling the LED
 */

// Set up the LED
void ledSetup() {
  // Set output pin for task1
  LED_DDR |= BIT2; // Set bit 2 of LED_DDR to 1 to make it an output pin
}


/**
 * @file demo6.ino
 * @brief Functions for setting up and controlling the DDS (Dynamic Digital Signage) system
 */

/**
 * @brief Sets up the DDS system by initializing the task array and dead task array
 * 
 * This function initializes the task array and dead task array by setting all of their
 * elements to default values. Each element in the task array represents a task that can
 * be executed by the DDS system, while each element in the dead task array represents a
 * task that has been completed and is no longer active.
 * 
 * @param none
 * @return none
 */
void DDSSetup() {
  // populate task array
  for (int i = 0; i < NTASKS; i++) {
    // set all elements of task array to default values
    taskArr[i].fn_ptr = NULL;
    taskArr[i].id = 0;
    taskArr[i].nTimes = 0;
    taskArr[i].timeSleep = 0;
    taskArr[i].time = 0;
    taskArr[i].state = DEAD;

    // set all elements of dead task array to default values
    deadTasks[i].fn_ptr = NULL;
    deadTasks[i].id = 0;
    deadTasks[i].nTimes = 0;
    deadTasks[i].timeSleep = 0;
    deadTasks[i].time = 0;
    deadTasks[i].state = DEAD;
  }
}
