/**
 * @file demo3.ino
 * @brief This file contains the implementation of a multitasking system using DDS and Timer 3 Interrupt.
 * 
 * This file defines the necessary constants, variables, and functions to implement a multitasking system using DDS and Timer 3 Interrupt. The system is capable of running up to 10 tasks, each with its own state and function pointer. The DDS is used to generate a square wave signal with a frequency of 1 kHz, which is used to trigger the Timer 3 Interrupt. The Timer 3 Interrupt sets a flag that is used to synchronize the execution of the tasks. The system also includes functions to load, start, and find dead tasks, as well as interrupt, speaker, and LED setup functions.
 * 
 */

#include "DDS.h"

// Define the number of tasks
#define NTASKS 10

// Define the possible states of a task
#define DEAD 0
#define READY 1
#define RUNNING 2
#define SLEEPING 3

volatile FLAG sFlag;

// Define the taskArr and deadTasks arrays
tcb taskArr[NTASKS];
tcb deadTasks[NTASKS];

// Define the id and currTask variables
volatile int id;
volatile int currTask;

// Define the reset1 and reset2 variables
int reset1 = 0;
int reset2 = 0;

// Setup function
void setup() {
  // Initialize the id variable
  id = 0;

  // Setup the interrupts, speaker, and LED
  interruptSetup();
  speakerSetup();
  ledSetup();

  // Setup the DDS
  DDSSetup();

  // Load the tasks
  task_load(task1, "task 1");
  task_load(task2, "task 2");
  task_load(schedule_sync, "schedule_sync");

  // Start the tasks
  task_start(find_dead_task("task 1"));
  task_start(find_dead_task("task 2"));
  task_start(find_dead_task("schedule_sync"));
}

// Loop function
void loop() {
  // Iterate through the taskArr array
  for (int i = 0; i < NTASKS; i++) {
    // Check if the task is ready to run
    if (taskArr[i].fn_ptr != NULL && taskArr[i].state == READY) {
      // Start the task
      taskArr[i].state = RUNNING;
      currTask = i;
      taskArr[i].nTimes++;
      (*(taskArr[i].fn_ptr))();

      // Tear down the task
      if (taskArr[i].state == RUNNING) taskArr[i].state = READY;
    }
  }
}

/**
 * @file demo3.ino
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
 * @file demo3.ino
 * @brief This file contains the implementation of the find_dead_task function.
 * 
 * This function searches the deadTasks array for a task with the given name and returns a pointer to its TCB.
 * 
 */

/**
 * @brief Finds a dead task with the given name and returns a pointer to its TCB.
 * 
 * This function searches the deadTasks array for a task with the given name and returns a pointer to its TCB.
 * 
 * @param name The name of the task to search for.
 * @return A pointer to the TCB of the dead task with the given name, or NULL if no such task is found.
 */
tcb * find_dead_task(const char * name) {
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (strcmp(name, deadTasks[i].name) == 0) break;
  }

  if (i == NTASKS) return NULL;

  return &(deadTasks[i]);
}

/**
 * @brief Copies the contents of one TCB to another.
 * 
 * This function copies the contents of one TCB (Task Control Block) to another. It copies the function pointer, name,
 * ID, number of times executed, state, time, and timeSleep from the source TCB to the destination TCB.
 * 
 * @param dst Pointer to the destination TCB.
 * @param src Pointer to the source TCB.
 * @return void
 */
void copy_tcb(tcb * dst, tcb * src) {
  // Copy the function pointer
  dst->fn_ptr = src->fn_ptr;

  // Copy the name
  strcpy(dst->name, src->name);

  // Copy the ID
  dst->id = src->id;

  // Copy the number of times executed
  dst->nTimes = src->nTimes;

  // Copy the state
  dst->state = src->state;

  // Copy the time
  dst->time = src->time;

  // Copy the sleep time
  dst->timeSleep = src->timeSleep;
}

/**
 * @brief Removes the current task from the task array and adds it to the dead task array.
 * 
 * This function removes the current task from the task array and adds it to the dead task array. It first finds an
 * open slot in the dead task array and copies the current task's tcb to that slot. It then clears all values in the
 * task array for the current task. Finally, it sets the current task's state to DEAD.
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

  // If there's no space, don't do anything
  if (i == NTASKS) return;

  // Copy the current task's tcb to the dead task array
  copy_tcb(&(deadTasks[i]), &(taskArr[currTask]));
  deadTasks[i].state = DEAD;

  // Clear all values in the task array for the current task
  taskArr[currTask].fn_ptr = NULL;
  strcpy(taskArr[currTask].name, "");
  taskArr[currTask].id = 0;
  taskArr[currTask].nTimes = 0;
  taskArr[currTask].time = 0;
  taskArr[currTask].timeSleep = 0;
  taskArr[currTask].state = DEAD;
}


/**
 * @brief Starts a task by copying it from the dead task array to the task array and setting its state to READY.
 * 
 * This function starts a task by copying it from the dead task array to the task array and setting its state to READY.
 * It first checks if the given task is NULL or if its state is not DEAD. If either of these conditions is true, the
 * function returns without doing anything. Otherwise, it finds the next open slot in the task array and copies the
 * task to that slot. It then sets the task's state to READY and sets the original task's state to DEAD.
 * 
 * @param task A pointer to the task control block (tcb) of the task to start.
 * @return void
 */
void task_start(tcb * task) {
  // Check if the task is NULL or if its state is not DEAD
  if (task == NULL || task->state != DEAD) return;

  // Find the next open slot in the task array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (taskArr[i].fn_ptr == NULL) break;
  }

  // If there's no space, don't do anything
  if (i == NTASKS) return;

  // Copy the task to the task array and set its state to READY
  copy_tcb(&(taskArr[i]), task);
  taskArr[i].state = READY;

  // Set the original task's state to DEAD
  task->state = DEAD;
}


/**
 * @brief Loads a new task into the dead task array.
 * 
 * This function loads a new task into the dead task array. It finds the next open slot in the dead task array,
 * and if there is space, it loads up a new task with the given function pointer and name. It also initializes
 * the task's ID, number of times run, time slept, time run, and state.
 * 
 * @param fn_ptr A function pointer to the task's function.
 * @param name A string representing the name of the task.
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
  strcpy(deadTasks[i].name, name);
  deadTasks[i].fn_ptr = fn_ptr;
  deadTasks[i].id = id;
  id++;
  deadTasks[i].nTimes = 0;
  deadTasks[i].timeSleep = 0;
  deadTasks[i].time = 0;
  deadTasks[i].state = DEAD;
}

/**
 * @brief Puts the current task to sleep for a specified amount of time.
 * 
 * This function puts the current task to sleep for a specified amount of time. It sets the current task's
 * timeSleep value to the specified time and its state to SLEEPING. When the task is woken up, it will resume
 * execution from where it left off.
 * 
 * @param t The amount of time to sleep, in milliseconds.
 * @return void
 */
void sleep_474(long t) {
  // sleep array @ [function index], set value to t
  taskArr[currTask].timeSleep = t;
  // state array @ [function index], set state to SLEEPING
  taskArr[currTask].state = SLEEPING;
  return;
}


/**
 * @brief Synchronizes the task scheduler with the system clock.
 * 
 * This function synchronizes the task scheduler with the system clock by updating the remaining sleep time
 * for any sleeping tasks and waking up any sleeping tasks whose remaining sleep time has elapsed. It also
 * updates the time for all tasks and resets the sFlag.
 * 
 * @param None
 * @return void
 */
void schedule_sync() {
  // wait until sFlag is no longer PENDING
  while (sFlag == PENDING) {
    3 + 5;
  }

  // update remaining sleep time for any sleeping tasks and wake up any sleeping tasks whose remaining sleep time has elapsed
  for (int i = 0; i < NTASKS; i++) {
    if (taskArr[i].state == SLEEPING) {
      // decrement remaining sleep time by 2
      taskArr[i].timeSleep -= 2;

      // wake up any sleeping tasks whose remaining sleep time has elapsed
      if (taskArr[i].timeSleep < 2) {
        // reset remaining sleep time to 0
        taskArr[i].timeSleep = 0;

        // change corresponding task state from SLEEPING to READY
        taskArr[i].state = READY;
      }
    }

    // update time for all tasks
    taskArr[i].time += 2;
  }

  // reset sFlag to PENDING
  sFlag = PENDING;
  return;
}



/**
 * @brief Sets the OCR4A to make the clock cycle frequency the same as the input frequency.
 * 
 * This function sets the OCR4A to make the clock cycle frequency the same as the input frequency.
 * It calculates the prescaler value based on the input frequency and sets the TIMER_COUNTER to 0.
 * 
 * @param freq The input frequency to set the OCR4A to.
 * @return void
 */
void setOC4AFreq(uint32_t freq) {
  // calculate the prescaler value based on the input frequency
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  // set the TIMER_COUNTER to 0
  TIMER_COUNTER = 0;
}



/**
 * @brief Flashes an LED on pin 47 for a certain duration.
 * 
 * This function flashes an LED on pin 47 for a certain duration. If a reset signal is received,
 * it resets the time and turns off the LED. Otherwise, it flashes the LED for a certain duration
 * and then turns it off for another duration.
 * 
 * @return void
 */
void task1() {
  // reset everything given a reset signal
  if (reset1) {
    LED_PORT &= ~BIT2; // turn off LED
    taskArr[currTask].time = 0; // reset time
    reset1 = 0; // reset reset signal
    return;
  }

  // flash led on pin 47 for FLASH_DURATION
  if (taskArr[currTask].time < (1 * FLASH_DURATION) + 1) {
    LED_PORT |= BIT2; // turn on LED
    sleep_474(250); // sleep for 250ms
    return;
  }

  if (taskArr[currTask].time < (2 * FLASH_DURATION) + 1) {
    LED_PORT &= ~BIT2; // turn off LED
    sleep_474(750); // sleep for 750ms
    return;
  }

  if (taskArr[currTask].time >= 1000) {
    taskArr[currTask].time = 0; // reset time
  }

  return;
}

/**
 * @brief Plays a melody on the speaker.
 * 
 * This function checks the current time and plays the corresponding note in the melody.
 * If the melody has finished playing, it resets the time.
 * 
 * @return void
 */
void task2() {
  if (reset2) {
    setOC4AFreq(0);
    taskArr[currTask].time = 0;
    reset2 = 0;
    return;
  }

  for (int i = 0; i < NMELODY; i++) {
    if ( taskArr[currTask].time >= ((unsigned long) i * PLAY_DURATION) &&
      taskArr[currTask].time < (((unsigned long) i + 1) * PLAY_DURATION) ) {
      setOC4AFreq(melody[i]);
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  if (taskArr[currTask].time < PICKUP_TIME) {
    setOC4AFreq(0);
    sleep_474(PAUSE_DURATION);
    return;
  }

  for (int i = 0; i < NMELODY; i++) {
    if (taskArr[currTask].time >= (PICKUP_TIME + ((unsigned long) i) * PLAY_DURATION) &&
        taskArr[currTask].time < (PICKUP_TIME + ((unsigned long) i + 1) * PLAY_DURATION)) {
      setOC4AFreq(melody[i]);
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION)) {
    taskArr[currTask].time = 0;
  }
}

/**
 * @brief Sets up the interrupt for the speaker.
 * 
 * This function sets up the timer to work in toggle mode and enables interrupts.
 * 
 * @return void
 */
void interruptSetup() {
  TCCR3A = 0; // clear timer control register A
  TCCR3B = 0; // clear timer control register B

  // set waveform generation mode
  TCCR3A |= (0 << WGM31) | (0 << WGM30);
  TCCR3B |= (0 << WGM33) | (1 << WGM32);

  // set timer to work in toggle mode
  TCCR3A |= (0 << COM3A1) | (1 << COM3A0);

  // enable output compare interrupt
  TIMSK3 |= (1 << OCIE3A);

  // set prescaler to 1 and timer counter to 0
  TCCR3B |= (0 << CS32) | (0 << CS31) | (1 << CS30);
 
  OCR3A = 32000; // set output compare register to 32000
  TCNT3H = 0; // clear timer counter high byte

  // enable interrupts
  SREG |= (1<<7);
}


/**
 * @brief Sets up the speaker.
 * 
 * This function sets up the timer to work in toggle mode and sets the prescaler and counter to 0.
 * 
 * @return void
 */
void speakerSetup() {
  TIMER_REG_A = 0; // clear timer register A
  TIMER_REG_B = 0; // clear timer register B

  // set waveform generation mode
  TIMER_REG_A |= (0 << WGM41) | (0 << WGM40);
  TIMER_REG_B |= (0 << WGM43) | (1 << WGM42);

  // set timer to work in toggle mode
  TIMER_REG_A |= (0 << COM4A1) | (1 << COM4A0);

  TIMSK4 = 0; // disable timer interrupts

  // set prescaler and timer counter to 0
  TIMER_REG_B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  SPEAKER_DDR |= BIT3; // set speaker pin as output
}


/**
 * @brief Initializes the LED pin as an output.
 * 
 * This function sets the LED pin as an output by setting the corresponding bit in the LED_DDR register.
 * 
 * @return void
 */
void ledSetup() {
  LED_DDR |= BIT2; // set LED pin as output
}


/**
 * @brief Initializes the task array and dead task array.
 * 
 * This function initializes the task array and dead task array by setting all the values to their default values.
 * 
 * @return void
 */
void DDSSetup() {
  for (int i = 0; i < NTASKS; i++) {
    taskArr[i].fn_ptr = NULL; // function pointer
    taskArr[i].id = 0; // task ID
    taskArr[i].nTimes = 0; // number of times task has run
    taskArr[i].timeSleep = 0; // time to sleep before running task
    taskArr[i].time = 0; // current time of task
    taskArr[i].state = DEAD; // state of task
    deadTasks[i].fn_ptr = NULL; // function pointer
    deadTasks[i].id = 0; // task ID
    deadTasks[i].nTimes = 0; // number of times task has run
    deadTasks[i].timeSleep = 0; // time to sleep before running task
    deadTasks[i].time = 0; // current time of task
    deadTasks[i].state = DEAD; // state of task
  }
}

