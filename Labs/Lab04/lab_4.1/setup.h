/**
 * @file setup.h
 * @brief Header file containing function prototypes and macros for Lab 4.1
 * @details This file contains the function prototypes and macros needed for Lab 4.1, including the melody notes, timer and port configurations, and task declarations.
 * @authors
 *  - Mason Wheeler
 * - Joey Pirich
 */

// Define notes for the melody
#define NOTE_C  262
#define NOTE_CS 277
#define NOTE_D  294
#define NOTE_DS 311
#define NOTE_E  330
#define NOTE_F  349
#define NOTE_FS 370
#define NOTE_G  392
#define NOTE_GS 415
#define NOTE_A  440
#define NOTE_AS 466
#define NOTE_B  494
#define NOTE_C5 523

// Define the number of notes in the melody and the melody itself
#define NMELODY 72
int melody[] = {
  NOTE_E, NOTE_E, 0, NOTE_E, 0, NOTE_C, NOTE_E, 0,
  NOTE_G, 0, 0,  0, NOTE_G, 0, 0, 0,
  NOTE_C5, 0, 0, NOTE_G, 0, 0, NOTE_E, 0,
  0, NOTE_A, 0, NOTE_B, 0, NOTE_AS, NOTE_A, 0,
  NOTE_G, NOTE_E, NOTE_G, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5,
  0, 0, NOTE_G, 0, 0, NOTE_E, 0, 0,
  NOTE_G, 0, 0, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5
};

#define TIMER_REG_A TCCR4A
#define TIMER_REG_B TCCR4B
#define TIMER_MASK TIMSK4
#define TIMER_COUNTER TCNT4H
#define PRESCALER OCR4A
#define LED_DDR DDRL
#define LED_PORT PORTL
#define LED_BIT 1 << 2
#define SPEAKER_DDR DDRH
#define SPEAKER_PORT PORTH
#define NSAMPLES 512

#define PLAY_DURATION 200
#define NPLAY 3

// define two tasks for Blink & AnalogRead
void TaskBlink(void * pvParameters);
void TaskTheme(void * pvParameters);
void Task34Starter(void * pvParameters);
void Task3(void * pvParameters);
void Task4(void * pvParameters);


void ledSetup();

void speakerSetup();

void setOC4AFreq(uint32_t freq);
