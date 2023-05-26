#include "DDS.h"

int reset1 = 0;
int reset2 = 0;

tcb taskArr[NTASKS];
tcb deadTasks[NTASKS];
volatile int id;
volatile int currTask;
volatile FLAG sFlag;

volatile int currFreq;

/// Initialize outputs
void setup() {
  id = 0;
  // get all our outputs set up
  interruptSetup();
  speakerSetup();
  displaySetup();

  // intialize our task tracking arrays
  DDSSetup();

  task_load(task4, "task4");
  task_load(schedule_sync, "schedule_sync");
  task_start(find_dead_task("task4"));
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
 * @file demo5.ino
 * @brief Interrupt Service Routine for TIMER3_COMPA_vect
 * 
 * This ISR sets the sFlag to DONE when TIMER3_COMPA_vect is triggered.
 * 
 * @return void
 */
ISR(TIMER3_COMPA_vect) {
  sFlag = DONE; // set sFlag to DONE
}


/**
 * @file demo5.ino
 * @brief Function to find a dead task by name
 * 
 * This function searches the deadTasks array for a task with the given name.
 * If found, it returns a pointer to the corresponding tcb struct.
 * 
 * @param name The name of the task to find
 * @return A pointer to the corresponding tcb struct, or NULL if not found
 */
tcb * find_dead_task(const char * name) {
  int i;
  // loop through the deadTasks array to find the task with the given name
  for (i = 0; i < NTASKS; i++) {
    if (strcmp(name, deadTasks[i].name) == 0) break;
  }

  // if the task is not found, return NULL
  if (i == NTASKS) return NULL;

  // otherwise, return a pointer to the corresponding tcb struct
  return &(deadTasks[i]);
}

/**
 * @file demo5.ino
 * @brief Function to copy a tcb struct from source to destination
 * 
 * This function copies the contents of a tcb struct from the source to the destination.
 * 
 * @param dst A pointer to the destination tcb struct
 * @param src A pointer to the source tcb struct
 * @return void
 */
void copy_tcb(tcb * dst, tcb * src) {
  // copy the function pointer
  dst->fn_ptr = src->fn_ptr;
  // copy the name string
  strcpy(dst->name, src->name);
  // copy the task ID
  dst->id = src->id;
  // copy the number of times the task has run
  dst->nTimes = src->nTimes;
  // copy the task state
  dst->state = src->state;
  // copy the task time
  dst->time = src->time;
  // copy the task sleep time
  dst->timeSleep = src->timeSleep;
}

/**
 * @file demo5.ino
 * @brief Function to quit the current task and move it to the dead task array
 * 
 * This function finds a spot in the dead task array and copies the current task's tcb struct to that spot.
 * It then sets the state of both the current task and the copied task to DEAD and sets the function pointer of the current task to NULL.
 * 
 * @param None
 * @return void
 */
void task_self_quit() {
  // find a spot in the dead array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (deadTasks[i].fn_ptr == NULL) break;
  }

  // if there's no space, don't do anything
  if (i == NTASKS) return;

  // copy the current task's tcb struct to the dead task array
  copy_tcb(&(deadTasks[i]), &(taskArr[currTask]));
  deadTasks[i].state = DEAD;
  taskArr[currTask].state = DEAD;
  taskArr[currTask].fn_ptr = NULL;
}

/**
 * @file demo5.ino
 * @brief Function to start a task
 * 
 * This function starts a task by copying its tcb struct to an open slot in the taskArr array.
 * It sets the state of the copied task to READY and the state of the original task to DEAD.
 * 
 * @param task A pointer to the tcb struct of the task to start
 * @return void
 */
void task_start(tcb * task) {
  // if the task is NULL or not in the DEAD state, return
  if (task == NULL || task->state != DEAD) return;

  // find an open slot in the taskArr array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (taskArr[i].fn_ptr == NULL) break;
  }

  // if there's no space, return
  if (i == NTASKS) return;

  // copy the tcb struct of the task to the open slot in the taskArr array
  copy_tcb(&(taskArr[i]), task);
  taskArr[i].state = READY;
  task->state = DEAD;
  task->fn_ptr = NULL;
}

/**
 * @file demo5.ino
 * @brief Function to load a new task into the dead task array
 * 
 * This function loads a new task into the dead task array by finding the next open slot in the array and copying the task's tcb struct to that slot.
 * It sets the task's name, function pointer, ID, number of times run, sleep time, time, and state.
 * 
 * @param fn_ptr A pointer to the function to be executed by the task
 * @param name A pointer to the name of the task
 * @return void
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
  strcpy(deadTasks[i].name, name); // set the name of the task
  deadTasks[i].fn_ptr = fn_ptr; // set the function pointer of the task
  deadTasks[i].id = id; // set the ID of the task
  id++; // increment the ID counter
  deadTasks[i].nTimes = 0; // set the number of times run to 0
  deadTasks[i].timeSleep = 0; // set the sleep time to 0
  deadTasks[i].time = 0; // set the time to 0
  deadTasks[i].state = DEAD; // set the state to DEAD
}

/**
 * @file demo5.ino
 * @brief Function to put a task to sleep for a specified amount of time
 * 
 * This function sets the sleep time of the current task to the specified time and changes its state to SLEEPING.
 * 
 * @param t The amount of time to sleep for
 * @return void
 */
void sleep_474(long t) {
  // set the sleep time of the current task to t
  taskArr[currTask].timeSleep = t;
  // change the state of the current task to SLEEPING
  taskArr[currTask].state = SLEEPING;
  return;
}

/**
 * @file demo5.ino
 * @brief Function to synchronize the task scheduler
 * 
 * This function synchronizes the task scheduler by updating the remaining sleep time of sleeping tasks and waking up any sleeping tasks whose sleep time has elapsed.
 * 
 * @param None
 * @return void
 */
void schedule_sync() {
  // wait until sFlag is no longer PENDING
  while (sFlag == PENDING) {
    3 + 5; // do nothing
  }

  // iterate through all tasks in the taskArr array
  for (int i = 0; i < NTASKS; i++) {
    // update remaining sleep time for sleeping tasks
    if (taskArr[i].state == SLEEPING) {
      taskArr[i].timeSleep -= 2; // decrement sleep time by 2
      // wake up any sleeping tasks whose sleep time has elapsed
      if (taskArr[i].timeSleep < 2) {
        taskArr[i].timeSleep = 0; // reset sleep time to 0
        taskArr[i].state = READY; // change state from SLEEPING to READY
      }
    }

    taskArr[i].time += 2; // increment task time by 2
  }

  // reset sFlag to PENDING
  sFlag = PENDING;
  return;
}

/**
 * @file demo5.ino
 * @brief Task 2 function to play a melody and countdown timer
 * 
 * This function plays a melody and a countdown timer using the setOC4AFreq function and the sleep_474 function.
 * 
 * @param None
 * @return void
 */
void task2() {
  // Reset the melody and task time if reset2 flag is set
  if (reset2) {
    setOC4AFreq(0);
    taskArr[currTask].time = 0;
    reset2 = 0;
    return;
  }

  // Play the melody
  for (int i = 0; i < NMELODY; i++) {
    if ( taskArr[currTask].time >= ((unsigned long) i * PLAY_DURATION) &&
      taskArr[currTask].time < (((unsigned long) i + 1) * PLAY_DURATION) ) {
      setOC4AFreq(melody[i]);
      currFreq = melody[i];
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // Stop playing for 4 seconds
  if (taskArr[currTask].time < PICKUP_TIME) {
    setOC4AFreq(0);
    currFreq = 0;

    sleep_474(PAUSE_DURATION);
    return;
  }

  // Start playing after 4 seconds
  for (int i = 0; i < NMELODY; i++) {
    if (taskArr[currTask].time >= (PICKUP_TIME + ((unsigned long) i) * PLAY_DURATION) &&
        taskArr[currTask].time < (PICKUP_TIME + ((unsigned long) i + 1) * PLAY_DURATION)) {
      setOC4AFreq(melody[i]);
      currFreq = melody[i];
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // Play the countdown timer after the second melody
  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION) &&
      taskArr[currTask].time < (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION + PAUSE_DURATION)) {
    task_start(find_dead_task("countdown"));
    sleep_474(PAUSE_DURATION);
    return;
  }

  // Reset the time and loop
  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION + PAUSE_DURATION)) {
    currFreq = 0;
    taskArr[currTask].time = 0;
    return;
  }
}

/**
 * @file demo5.ino
 * @brief Task 4 function to play two melodies and a countdown timer
 * 
 * This function plays two melodies and a countdown timer using the setOC4AFreq function and the sleep_474 function.
 * 
 * @param None
 * @return void
 */
void task4() {
  // launch task 2 to run on the first call to task4
  if (taskArr[currTask].nTimes == 0) {
    task_load(task2, "task2");
    task_start(find_dead_task("task2"));
    task_load(task4_1, "display_freqs");
    task_load(task4_2, "countdown");
  }

  // Play the first melody and display the frequency on the 7-segment display
  if (taskArr[currTask].time < NMELODY * PLAY_DURATION) {
    task_start(find_dead_task("display_freqs"));
    sleep_474(NMELODY * PLAY_DURATION);
    return;
  }

  // Start the countdown task after the first melody
  if (taskArr[currTask].time >= NMELODY * PLAY_DURATION &&
      taskArr[currTask].time < (PLAY_DURATION*NMELODY) + PAUSE_DURATION) {
    task_start(find_dead_task("countdown"));
    sleep_474(PAUSE_DURATION);
    return;
  }

  // Play the second melody and display the frequency on the 7-segment display
  if (taskArr[currTask].time >= NMELODY * PLAY_DURATION + PAUSE_DURATION &&
      taskArr[currTask].time < 2 * NMELODY * PLAY_DURATION + PAUSE_DURATION) {
    task_start(find_dead_task("display_freqs"));
    sleep_474(NMELODY * PLAY_DURATION);
    return;
  }

  // Start the countdown task after the second melody
  if (taskArr[currTask].time >= 2 * NMELODY * PLAY_DURATION + PAUSE_DURATION &&
      taskArr[currTask].time < 2 * NMELODY * PLAY_DURATION + 2 * PAUSE_DURATION) {
    setOC4AFreq(0);
    currFreq = 0;
    task_start(find_dead_task("countdown"));
    sleep_474(PAUSE_DURATION);
    return;
  }

  // Reset the time and loop
  if (taskArr[currTask].time >= (2 * NMELODY * PLAY_DURATION) + (2 * PAUSE_DURATION)) {
    taskArr[currTask].time = 0;
  }
}
/**
 * @file demo5.ino
 * @brief Task 4 function to display the frequency on the 7-segment display
 * 
 * This function displays the frequency on the 7-segment display using the convert function, the seven_seg_digits array, 
 * the send7 function, and the sleep_474 function.
 * 
 * @param None
 * @return void
 */
void task4_1() {
  static int time; // static variable to keep track of time
  static int digits[4]; // static array to store digits
  static int displayStates[4] = {S0, S1, S2, S3}; // static array to store display states

  // take digits out of currFreq
  convert(digits, currFreq); // convert the current frequency to digits

  // display count on the 7seg display
  for (int h = 0; h < 5; h++) { // loop through the 7-segment display
    for (int i = 0; i < 4; i++) {
      int pin = 10 + i; // set the pin number
      if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
        // turn 7seg & specified digit on
        PORTB = displayStates[i]; // set the display state
        byte *disp = seven_seg_digits[digits[i]]; // get the digit to display
        send7(disp); // send the digit to the 7-segment display
        sleep_474(2); // sleep for 2ms
        return; // exit the function
      }
    }
  }

  // reset
  if (taskArr[currTask].time >= 100) { // if the time is greater than or equal to 100ms
    time += taskArr[currTask].time; // add the current time to the total time
    taskArr[currTask].time = 0; // reset the task time
  }
  if (time >= NMELODY * PLAY_DURATION) { // if the total time is greater than or equal to the duration of the melody
    time = 0; // reset the total time
    PORTB = 0xFF; // turn off the 7-segment display
    task_self_quit(); // quit the task
  }
}

/**
 * @file demo5.ino
 * @brief Task 4 function to display a countdown timer on the 7-segment display
 * 
 * This function displays a countdown timer on the 7-segment display using the convert function, the seven_seg_digits array, 
 * the send7 function, and the sleep_474 function.
 * 
 * @param None
 * @return void
 */
void task4_2() {
  static long time; // static variable to keep track of time
  static int count; // static variable to keep track of count
  static int digits[4]; // static array to store digits
  static int displayStates[4] = {S0, S1, S2, S3}; // static array to store display states

  // take digits out of count
  convert(digits, (PAUSE_DURATION / 100) - count); // convert the count to digits

  // display count on the 7seg display
  for (int h = 0; h < 5; h++) { // loop through the 7-segment display
    for (int i = 0; i < 4; i++) {
      int pin = 10 + i; // set the pin number
      if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
        // turn 7seg & specified digit on
        PORTB = displayStates[i]; // set the display state
        byte *disp = seven_seg_digits[digits[i]]; // get the digit to display
        send7(disp); // send the digit to the 7-segment display
        sleep_474(2); // sleep for 2ms
        return; // exit the function
      }
    }
  }

  // reset
  if (taskArr[currTask].time >= 100) { // if the time is greater than or equal to 100ms
    time += taskArr[currTask].time; // add the current time to the total time
    taskArr[currTask].time = 0; // reset the task time
    count = count == 9999 ? 0 : count + 1; // increment the count or reset it if it reaches 9999
  }

  if (time >= PAUSE_DURATION) { // if the total time is greater than or equal to the pause duration
    PORTB = 0xFF; // turn off the 7-segment display
    time = 0; // reset the total time
    taskArr[currTask].time = 0; // reset the task time
    count = 0; // reset the count
    task_self_quit(); // quit the task
  }
}



/**
 * @file demo5.ino
 * @brief Function to set up the interrupt for the timer
 * 
 * This function sets up the interrupt for the timer to work in CTC mode and enables the interrupt on counter A.
 * 
 * @param None
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
 * @file demo5.ino
 * @brief Sets the OCR4A to make the clock cycle frequency the same as the input freq
 * 
 * This function sets the OCR4A to make the clock cycle frequency the same as the input freq.
 * It calculates the prescaler value based on the input frequency and sets the timer counter to 0.
 * 
 * @param freq The frequency to set the OCR4A to
 * @return void
 */
void setOC4AFreq(uint32_t freq) {
  // Calculate the prescaler value based on the input frequency
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  
  // Set the timer counter to 0
  TIMER_COUNTER = 0;
}


/**
 * @file demo5.ino
 * @brief Function to set up the speaker for playing sound
 * 
 * This function sets up the speaker for playing sound by configuring the timer registers and the prescaler value.
 * 
 * @param None
 * @return void
 */
void speakerSetup() {
  // reset timer registers
  TIMER_REG_A = 0;
  TIMER_REG_B = 0;

  // set timer to work in Fast PWM mode
  TIMER_REG_A |= (0 << WGM41) | (0 << WGM40);
  TIMER_REG_B |= (0 << WGM43) | (1 << WGM42);

  // set output compare mode to toggle on match
  TIMER_REG_A |= (0 << COM4A1) | (1 << COM4A0);

  // disable all interrupts
  TIMSK4 = 0;

  // set prescaler value to 1
  TIMER_REG_B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  // set speaker pin as output
  SPEAKER_DDR |= BIT3;
}

/**
 * @file demo5.ino
 * @brief Function to set up the display and send data to the 7-segment display
 * 
 * This function sets up the digit selectors and 7-segment outputs for the display.
 * It also sends data to the 7-segment display by converting an array of bytes to an integer and setting the PORTA register to that value.
 * 
 * @param arr The array of bytes to send to the 7-segment display
 * @return void
 */
void send7(byte arr[7]) {
  int data = 0;
  for (int i = 0; i < 7; i++) {
    data |= (arr[i] << i); // shift the bits of arr[i] to the left by i and OR them with data
  }
  PORTA = data; // set the PORTA register to the value of data
}

/**
 * @file demo5.ino
 * @brief Function to set up the display
 * 
 * This function sets up the digit selectors and 7-segment outputs for the display.
 * 
 * @param None
 * @return void
 */
void displaySetup() {
  // set up digit selectors
  DDRB |= BIT7 | BIT6 | BIT5 | BIT4;

  // set up 7-seg outputs
  DDRA |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;

  PORTB |= BIT7 | BIT6 | BIT5 | BIT4; // set the PORTB register to enable the digit selectors
}


/**
 * @file demo5.ino
 * @brief Function to convert an integer to an array of its digits
 * 
 * This function takes an integer value and converts it to an array of its digits.
 * 
 * @param digits An array to store the digits of the integer
 * @param val The integer value to convert to an array of digits
 * @return void
 */
void convert(int * digits, int val) {
  digits[0] = val % 10; // get the last digit of the integer
  digits[1] = (val/10) % 10; // get the second to last digit of the integer
  digits[2] = (val/100) % 10; // get the third to last digit of the integer
  digits[3] = (val/1000) % 10; // get the fourth to last digit of the integer
}


/**
 * @file demo5.ino
 * @brief Function to set up the DDS task array
 * 
 * This function initializes the DDS task array by setting all of its elements to default values.
 * 
 * @param None
 * @return void
 */
void DDSSetup() {
  // populate task array
  for (int i = 0; i < NTASKS; i++) {
    taskArr[i].fn_ptr = NULL; // set function pointer to NULL
    taskArr[i].id = 0; // set task ID to 0
    taskArr[i].nTimes = 0; // set number of times to run task to 0
    taskArr[i].timeSleep = 0; // set time to sleep between task runs to 0
    taskArr[i].time = 0; // set time of last run to 0
    taskArr[i].state = DEAD; // set task state to DEAD
    deadTasks[i].fn_ptr = NULL; // set function pointer of dead task to NULL
    deadTasks[i].id = 0; // set ID of dead task to 0
    deadTasks[i].nTimes = 0; // set number of times to run dead task to 0
    deadTasks[i].timeSleep = 0; // set time to sleep between dead task runs to 0
    deadTasks[i].time = 0; // set time of last run of dead task to 0
    deadTasks[i].state = DEAD; // set state of dead task to DEAD
  }
}
