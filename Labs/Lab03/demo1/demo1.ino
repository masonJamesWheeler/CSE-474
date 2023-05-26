#include "RR.h"

// Global variables
int reset1 = 0;
int reset2 = 0;

// Function to set the frequency of the speaker using PWM
void setOC4AFreq(uint32_t freq) {
  PRESCALER = freq == 0 ? 0 : 16000000 / (2 * freq);
  TIMER_COUNTER = 0;
}

/**
 * Task 1: Flash an LED on and off at a fixed rate
 * 
 * This function flashes an LED on and off at a fixed rate. The LED is connected to pin 2 of the LED_PORT.
 * 
 * @param None
 * @return None
 */
void task1() {
  static unsigned long time = 0; // Static variable to keep track of elapsed time
  
  // If reset1 is set, turn on the LED and reset the time variable
  if (reset1) {
    LED_PORT |= BIT2; // Turn on the LED
    time = 0; // Reset the time variable
    reset1 = 0; // Reset the reset1 flag
  } else {
    // If the elapsed time is equal to the duration of the LED on or off state, toggle the LED
    if (time == (0 * FLASH_DURATION) + 1) {
      LED_PORT &= ~BIT2; // Turn off the LED
    } else if (time == (1 * FLASH_DURATION) + 1) {
      LED_PORT |= BIT2; // Turn on the LED
    }

    // If the elapsed time reaches 1000, reset the time variable
    if (time == 1000) {
      time = 0; // Reset the time variable
    }
  }
  
  // Increment the time variable
  time++;
};

/**
 * Task 2: Play a melody on the speaker using PWM
 * 
 * This function plays a melody on the speaker using PWM. The melody is stored in the `melody` array, and the duration of each note is defined by the `PLAY_DURATION` constant. The speaker is connected to pin 3 of the `SPEAKER_PORT`.
 * 
 * @param None
 * @return None
 */
void task2() {
  static unsigned long time = 0; // Static variable to keep track of elapsed time
  
  // If reset2 is set, turn off the speaker and reset the time variable
  if (reset2) {
    setOC4AFreq(0); // Turn off the speaker
    time = 0; // Reset the time variable
    reset2 = 0; // Reset the reset2 flag
  } else {
    // Iterate over the melody notes and set the PWM frequency to the corresponding note frequency at the appropriate time
    for (int i = 0; i < NMELODY; i++) {
      unsigned long timeKey = i * PLAY_DURATION; // Calculate the time key for the current note
      if (time == timeKey + 1 || time == PICKUP_TIME + timeKey + 1) { // If the elapsed time matches the time key or the pickup time plus the time key, set the PWM frequency to the corresponding note frequency
        setOC4AFreq(melody[i]);
      }
    }

    // Turn off the speaker when the melody is finished
    if (time == NMELODY * PLAY_DURATION + 1 || time == PICKUP_TIME + NMELODY * PLAY_DURATION + 1) { // If the elapsed time matches the end of the melody or the end of the melody plus the pickup time, turn off the speaker
      setOC4AFreq(0);
    }

    // If the elapsed time reaches the end of the melody, reset the time variable
    if (time == PICKUP_TIME + NMELODY * PLAY_DURATION + 1) {
      time = 0; // Reset the time variable
    }
  }
  
  // Increment the time variable
  time++;
}

// Function to initialize the hardware used by the sketch
void setup() {
  // Initialize the timer registers, and hardware pins
  TIMER_REG_A = (0 << WGM41) | (0 << WGM40) | (0 << COM4A1) | (1 << COM4A0);
  TIMER_REG_B = (0 << WGM43) | (1 << WGM42) | (0 << CS42) | (0 << CS41) | (1 << CS40);
  TIMER_MASK = 0;
  PRESCALER = 0;
  TIMER_COUNTER = 0;
  SPEAKER_DDR |= BIT3;
  LED_DDR |= BIT0 | BIT1 | BIT2;
}

// Main loop function
void loop() {
  // Call task1 and task2 repeatedly
  task1();
  task2();
  
  // Wait for 1 millisecond before repeating the loop
  delay(1);
}