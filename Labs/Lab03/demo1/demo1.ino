#include "RR.h"

int reset1 = 0;
int reset2 = 0;

// Set the OCR4A to match the input frequency
void setOC4AFreq(uint32_t freq) {
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  TIMER_COUNTER = 0;
}

void task1() {
  static unsigned long time = 0;
  
  if (reset1) {
    LED_PORT |= BIT2;
    time = 0;
    reset1 = 0;
  } else {
    // LED flashes on pin 47 for FLASH_DURATION
    if (time == (0 * FLASH_DURATION) + 1) {
      LED_PORT &= ~BIT2;
    } else if (time == (1 * FLASH_DURATION) + 1) {
      LED_PORT |= BIT2;
    }

    // Reset time after 1000 cycles
    if (time == 1000) {
      time = 0;
    }
  }
  
  time++;
};


void task2() {
  static unsigned long time = 0;
  
  if (reset2) {
    setOC4AFreq(0);
    time = 0;
    reset2 = 0;
  } else {
    for (int i = 0; i < NMELODY; i++) {
      unsigned long timeKey = i * PLAY_DURATION;
      if (time == timeKey + 1 || time == PICKUP_TIME + timeKey + 1) {
        setOC4AFreq(melody[i]);
      }
    }

    if (time == NMELODY * PLAY_DURATION + 1 || time == PICKUP_TIME + NMELODY * PLAY_DURATION + 1) {
      setOC4AFreq(0);
    }

    if (time == PICKUP_TIME + NMELODY * PLAY_DURATION + 1) {
      time = 0;
    }
  }
  
  time++;
}


// Initializes everything needed for the tasks
void setup() {
  Serial.begin(9600);
  // Clear timer settings and set timer to work in CTC and toggle mode
  TIMER_REG_A = (0 << WGM41) | (0 << WGM40) | (0 << COM4A1) | (1 << COM4A0);
  TIMER_REG_B = (0 << WGM43) | (1 << WGM42) | (0 << CS42) | (0 << CS41) | (1 << CS40);
  
  // Disable interrupts on the timer
  TIMER_MASK = 0;

  // Reset prescaler and timer counter
  PRESCALER = 0;
  TIMER_COUNTER = 0;

  // Set pin 6 and LEDs as output pins
  SPEAKER_DDR |= BIT3;
  LED_DDR |= BIT0 | BIT1 | BIT2;
}

// Scheduler loop
void loop() {
  task1();
  task2();
  delay(1);
}
