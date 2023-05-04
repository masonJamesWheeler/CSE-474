# LED Matrix and Tone Generator Lab

## Overview
In this lab, we successfully drove an 8x8 matrix of LEDs and generated tones on a speaker using an Arduino Mega microcontroller board. We focused on the correct timing of multiple tasks by verifying light motion and audio tone. We also verified the timing and frequency output using an oscilloscope.

## Learning Objectives
Throughout this lab, we achieved the following objectives:
- Manipulated hardware registers/bits, without the use of existing libraries, to perform low-level hardware functions.
- Coordinated multiple concurrent tasks with round-robin scheduling.

## Equipment
1. Arduino Mega Microcontroller board, breadboards, wires
2. External Arduino power supply (recommended)
3. 3 LEDs and 250-500 Ohm resistors
4. Small 8-Ohm Speaker
5. 8x8 LED Matrix
6. 2-way thumbstick control

## Implementation
We completed the lab in a team of two, collaborating on both the hardware setup and code development. We ensured that both team members understood all the wiring and code used in our solution. We submitted our work, including both team members' names and student numbers, as per the requirements.

## Completed Tasks
We successfully performed all demonstrations and completed the following tasks:
1. Flashed LEDs in a sequential pattern using direct hardware manipulation.
2. Generated tones using a 16-bit Timer/Counter.
3. Coordinated multiple tasks concurrently with round-robin scheduling.
4. Controlled an 8x8 LED matrix with a thumbstick.

## Files in this Repository
1. `LED_sequence.ino`: Arduino code to flash LEDs in a sequential pattern.
2. `timer_tone_output.ino`: Arduino code to generate tones using Timer/Counter.
3. `concurrent_tasks.ino`: Arduino code to coordinate multiple tasks with round-robin scheduling.
4. `interactive_display.ino`: Arduino code to control an 8x8 LED matrix with a thumbstick.

## Usage
1. Upload the corresponding Arduino code to your Arduino Mega board.
2. Set up the hardware components as described in the lab spec.
3. Observe and demonstrate the completed tasks and functionalities.

## Contributions
Both team members contributed equally to the completion of the lab, including hardware setup, code development, and documentation.

## License
[MIT](https://opensource.org/licenses/MIT)
