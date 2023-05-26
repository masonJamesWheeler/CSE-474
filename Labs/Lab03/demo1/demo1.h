#ifndef DEMO1_H
#define DEMO1_H

/**
 * @file demo1.h
 * @brief This file contains the code for a demo program that flashes an LED and plays a melody.
 * 
 * The program uses a timer to control the timing of the LED flashing and melody playing.
 * The LED is connected to pin 47 and the melody is played using the timer's output compare
 * register 4A. The program contains two tasks: task1() and task2(). task1() flashes the LED
 * at a fixed rate and task2() plays a melody at a fixed rate. The program also contains a
 * setup() function that initializes the timer and sets the prescaler.
 * 
 * This file was created by Mason Wheeler and Joey Pirich.
 */

#include <stdint.h>

#define LED_PORT PORTL
#define BIT2 0x04

#define TIMER_REG_A OCR4A
#define TIMER_REG_B TCCR4B
#define TIMER_MASK TIMSK4
#define PRESCALER OCR4C
#define TIMER_COUNTER TCNT4

#define WGM40 0
#define WGM41 1
#define WGM42 3
#define WGM43 4

#define COM4A0 6
#define COM4A1 7

#define CS40 0
#define CS41 1
#define CS42 2

#define FLASH_DURATION 100
#define PLAY_DURATION 100
#define PICKUP_TIME 4000
#define NMELODY 16

extern int reset1;
extern int reset2;
extern uint16_t melody[];

void setOC4AFreq(uint32_t freq);
void task1();
void task2();
void setup();

#endif // DEMO1_H