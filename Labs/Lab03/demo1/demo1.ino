
#include "RR.h"

int reset1 = 0;
int reset2 = 0;

// sets the OCR4A to make the clock cycle frequency
// the same as the input freq
void setOC4AFreq(uint32_t freq) {
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  TIMER_COUNTER = 0;
}

void task1() {
  static unsigned long time;
  time++;

  // reset everything given a reset signal
  if (reset1) {
    LED_PORT |= BIT2;
    time = 0;
    reset1 = 0;
    return;
  }

  // flash led on pin 47 for FLASH_DURATION
  if (time == (0 * FLASH_DURATION) + 1) {
    LED_PORT &= ~BIT2;
  }

  if (time == (1 * FLASH_DURATION) + 1) {
    LED_PORT |= BIT2;
  }

  if (time == 1000) {
    time = 0;
  }

  return;
}

void task2() {
  static unsigned long time;
  time++;

  if (reset2) {
    setOC4AFreq(0);
    time = 0;
    reset2 = 0;
    return;
  }

  // play tone
  for (int i = 0; i < NMELODY; i++) {
    if (time == ((unsigned long) i * PLAY_DURATION) + 1) {
      setOC4AFreq(melody[i]);
    }
  }

  // stop playing for 4 seconds
  if (time == ((unsigned long) NMELODY * PLAY_DURATION) + 1) {
    setOC4AFreq(0);
  }

  // start playing after 4 seconds
  for (int i = 0; i < NMELODY; i++) {
    if (time == (PICKUP_TIME + (unsigned long) i * PLAY_DURATION) + 1) {
      setOC4AFreq(melody[i]);
    }
  }

  // reset
  if (time == (PICKUP_TIME + (unsigned long) NMELODY * PLAY_DURATION) + 1) {
    time = 0;
  }
}

/// Initializes everything needed for the tasks
void setup() {
  // clear timer settings
  TIMER_REG_A = 0;
  TIMER_REG_B = 0;
  // set our timer to work in CTC mode
  TIMER_REG_A |= (0 << WGM41) | (0 << WGM40);
  TIMER_REG_B |= (0 << WGM43) | (1 << WGM42);

  // set timer to work in toggle mode
  TIMER_REG_A |= (0 << COM4A1) | (1 << COM4A0);

  // disable interrupts on the timer
  TIMER_MASK = 0;

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

  // set output pins for task1
  LED_DDR |= BIT0;
  LED_DDR |= BIT1;
  LED_DDR |= BIT2;
}

/// Scheduler loop
void loop() {
  while(1) {
  task1();
  task2();
  delayMicroseconds(700);
}
}
