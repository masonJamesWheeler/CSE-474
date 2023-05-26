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

/// Timer 3 Interrupt, sets sFlag
ISR(TIMER3_COMPA_vect) {
  sFlag = DONE;
}

tcb * find_dead_task(const char * name) {
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (strcmp(name, deadTasks[i].name) == 0) break;
  }

  if (i == NTASKS) return NULL;

  return &(deadTasks[i]);
}

void copy_tcb(tcb * dst, tcb * src) {
  dst->fn_ptr = src->fn_ptr;
  strcpy(dst->name, src->name);
  dst->id = src->id;
  dst->nTimes = src->nTimes;
  dst->state = src->state;
  dst->time = src->time;
  dst->timeSleep = src->timeSleep;
}

void task_self_quit() {
  // find a spot in the dead array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (deadTasks[i].fn_ptr == NULL) break;
  }

  if (i == NTASKS) return;
  copy_tcb(&(deadTasks[i]), &(taskArr[currTask]));
  deadTasks[i].state = DEAD;
  taskArr[currTask].state = DEAD;
  taskArr[currTask].fn_ptr = NULL;
}

void task_start(tcb * task) {
  if (task == NULL || task->state != DEAD) return;
  // find a spot in the dead array
  int i;
  for (i = 0; i < NTASKS; i++) {
    if (taskArr[i].fn_ptr == NULL) break;
  }

  if (i == NTASKS) return;
  copy_tcb(&(taskArr[i]), task);
  taskArr[i].state = READY;
  task->state = DEAD;
  task->fn_ptr = NULL;
}

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

void sleep_474(long t) {
  // sleep array @ [function index], set value to t
  taskArr[currTask].timeSleep = t;
  // state array @ [function index], set state to SLEEPING
  taskArr[currTask].state = SLEEPING;
  return;
}

void schedule_sync() {
  while (sFlag == PENDING) {
    3 + 5;
  }
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

    taskArr[i].time += 2;
  }

  // reset sFlag
  sFlag = PENDING;
  return;
}

void task2() {
  if (reset2) {
    setOC4AFreq(0);
    taskArr[currTask].time = 0;
    reset2 = 0;
    return;
  }

  // play tone
  for (int i = 0; i < NFREQ; i++) {
    if ( taskArr[currTask].time >= ((unsigned long) i * PLAY_DURATION) &&
      taskArr[currTask].time < (((unsigned long) i + 1) * PLAY_DURATION) ) {
      setOC4AFreq(melody[i]);
      currFreq = melody[i];
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // stop playing for 4 seconds
  if (taskArr[currTask].time < PICKUP_TIME) {
    setOC4AFreq(0);
    currFreq = 0;

    sleep_474(PAUSE_DURATION);
    return;
  }

  // start playing after 4 seconds
  for (int i = 0; i < NFREQ; i++) {
    if (taskArr[currTask].time >= (PICKUP_TIME + ((unsigned long) i) * PLAY_DURATION) &&
        taskArr[currTask].time < (PICKUP_TIME + ((unsigned long) i + 1) * PLAY_DURATION)) {
      setOC4AFreq(melody[i]);
      currFreq = melody[i];
      sleep_474(PLAY_DURATION);
      return;
    }
  }

  // Play the countdown timer after the second melody
  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NFREQ * PLAY_DURATION) &&
      taskArr[currTask].time < (PICKUP_TIME + (unsigned long) NFREQ * PLAY_DURATION + PAUSE_DURATION)) {
    task_start(find_dead_task("countdown"));
    sleep_474(PAUSE_DURATION);
    return;
  }

  if (taskArr[currTask].time >= (PICKUP_TIME + (unsigned long) NFREQ * PLAY_DURATION + PAUSE_DURATION)) {
    // Set time back to zero and loop
    currFreq = 0;
    taskArr[currTask].time = 0;
    return;
  }
}


void task4() {
  // launch task 2 to run on the first call to task4
  if (taskArr[currTask].nTimes == 0) {
    task_load(task2, "task2");
    void task_start(int id) {
      taskArr[id].state = RUNNING;
      taskArr[id].fn_ptr();
    }

    int find_dead_task(char *name) {
      for (int i = 0; i < NTASKS; i++) {
        if (deadTasks[i].fn_ptr != NULL && strcmp(deadTasks[i].name, name) == 0) {
          int id = deadTasks[i].id;
          deadTasks[i].fn_ptr = NULL;
          deadTasks[i].id = 0;
          deadTasks[i].nTimes = 0;
          deadTasks[i].timeSleep = 0;
          deadTasks[i].time = 0;
          deadTasks[i].state = DEAD;
          return id;
        }
      }
      return -1;
    }

    void task_load(int id, char *name) {
      taskArr[id].fn_ptr = id == 0 ? task0 : id == 1 ? task1 : id == 2 ? task2 : id == 3 ? task3 : id == 4 ? task4_1 : task4_2;
      taskArr[id].id = id;
      taskArr[id].nTimes = 0;
      taskArr[id].timeSleep = 0;
      taskArr[id].time = 0;
      taskArr[id].state = READY;
      strcpy(taskArr[id].name, name);
    }

    void task_self_quit() {
      taskArr[currTask].state = DEAD;
      deadTasks[deadTaskIndex].fn_ptr = taskArr[currTask].fn_ptr;
      deadTasks[deadTaskIndex].id = taskArr[currTask].id;
      deadTasks[deadTaskIndex].nTimes = taskArr[currTask].nTimes;
      deadTasks[deadTaskIndex].timeSleep = taskArr[currTask].timeSleep;
      deadTasks[deadTaskIndex].time = taskArr[currTask].time;
      deadTasks[deadTaskIndex].state = DEAD;
      strcpy(deadTasks[deadTaskIndex].name, taskArr[currTask].name);
      deadTaskIndex = (deadTaskIndex + 1) % NTASKS;
      currTask = -1;
    }

    void task0() {
      static int time;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of currFreq
      convert(digits, currFreq);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
      }
      if (time >= NFREQ * PLAY_DURATION) {
        time = 0;
        PORTB = 0xFF;
        task_self_quit();
      }
    }

    void task1() {
      static int time;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of currFreq
      convert(digits, currFreq);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
      }
      if (time >= NFREQ * PLAY_DURATION) {
        time = 0;
        PORTB = 0xFF;
        task_self_quit();
      }
    }

    void task2() {
      static int time;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of currFreq
      convert(digits, currFreq);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
      }
      if (time >= NFREQ * PLAY_DURATION) {
        time = 0;
        PORTB = 0xFF;
        task_self_quit();
      }
    }

    void task3() {
      static int time;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of currFreq
      convert(digits, currFreq);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
      }
      if (time >= NFREQ * PLAY_DURATION) {
        time = 0;
        PORTB = 0xFF;
        task_self_quit();
      }
    }

    void task4_1() {
      static int time;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of currFreq
      convert(digits, currFreq);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
      }
      if (time >= NFREQ * PLAY_DURATION) {
        time = 0;
        PORTB = 0xFF;
        task_self_quit();
      }
    }

    void task4_2() {
      static long time;
      static int count;
      static int digits[4];
      static int displayStates[4] = {S0, S1, S2, S3};

      // take digits out of count
      convert(digits, (PAUSE_DURATION / 100) - count);

      // display count on the 7seg display
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 4; i++) {
          int pin = 10 + i;
          if ((taskArr[currTask].time / 5) >= (4 * h) + i && (taskArr[currTask].time / 5) < (4 * h) + (i + 1)) {
            // turn 7seg & specified digit on
            PORTB = displayStates[i];
            byte *disp = seven_seg_digits[digits[i]];
            send7(disp);
            sleep_474(2);
            return;
          }
        }
      }

      // reset
      if (taskArr[currTask].time >= 100) {
        time += taskArr[currTask].time;
        taskArr[currTask].time = 0;
        count = count == 9999 ? 0 : count + 1;
      }

      if (time >= PAUSE_DURATION) {
        PORTB = 0xFF;
        time = 0;
        taskArr[currTask].time = 0;
        count = 0;
        task_self_quit();
      }
    }

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

    // sets the OCR4A to make the clock cycle frequency
    // the same as the input freq
    void setOC4AFreq(uint32_t freq) {
      PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
      TIMER_COUNTER = 0;
    }

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

    void displaySetup() {
      // set up digit selectors
      DDRB |= BIT7 | BIT6 | BIT5 | BIT4;

      // set up 7-seg outputs
      DDRA |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;

      PORTB |= BIT7 | BIT6 | BIT5 | BIT4;
    }

    void send7(byte arr[7]) {
      int data = 0;
      for (int i = 0; i < 7; i++) {
        data |= (arr[i] << i);
      }
      PORTA = data;
    }

    void convert(int * digits, int val) {
      digits[0] = val % 10;
      digits[1] = (val/10) % 10;
      digits[2] = (val/100) % 10;
      digits[3] = (val/1000) % 10;
    }

    void DDSSetup() {
      // populate task array
      for (int i = 0; i < NTASKS; i++) {
        taskArr[i].fn_ptr = NULL;
        taskArr[i].id = 0;
        taskArr[i].nTimes = 0;
        taskArr[i].timeSleep = 0;
        taskArr[i].time = 0;
        taskArr[i].state = DEAD;
        deadTasks[i].fn_ptr = NULL;
        deadTasks[i].id = 0;
        deadTasks[i].nTimes = 0;
        deadTasks[i].timeSleep = 0;
        deadTasks[i].time = 0;
        deadTasks[i].state = DEAD;
      }   
  }