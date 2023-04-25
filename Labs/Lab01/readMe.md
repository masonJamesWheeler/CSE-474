# Lab 1: Getting Started with the Arduino Mega

## Overview

This lab will guide you through the process of setting up your Arduino Mega microcontroller board, installing the required software (Arduino IDE), and running a basic sketch (program) to demonstrate blinking an LED and producing a tone through a small speaker. By the end of this lab, you will be familiar with the basic operations of the Arduino Mega and ready to explore more advanced projects.

## Learning Objectives

With successful completion of this lab, the student will be able to:

1. Install and set up Arduino IDE
2. Build and run a basic sketch (program) using the Arduino Libraries
3. Modify and demonstrate blinking light code and speaker output tone

## Equipment

(See Equipment List with sources)

* Arduino Mega Microcontroller board
* External Arduino power supply
* LEDs and 250 Ohm resistors
* Small 8-Ohm Speaker
* USB- Type-B cable (with USB-A or USB-C for computer end) (you probably got one with Arduino) (image: globetek.com)
* Solderless Breadboard
* Wires (either with pins (see the kits), or cut and strip your own 22AWG solid wire)

## Getting Started

1. Download and install the latest version of the Arduino IDE from the [Arduino website](https://www.arduino.cc/en/software).

2. Connect the Arduino Mega to your computer using the USB Type-B cable. The computer should recognize the board and install the necessary drivers.

3. Open the Arduino IDE, and navigate to `Tools > Board > Arduino AVR Boards > Arduino Mega or Mega 2560` to select the correct board.

4. Go to `Tools > Port` and select the appropriate port for your Arduino Mega board.

## Blinking LED

1. Assemble the circuit on your breadboard according to the provided schematic, connecting an LED and a 250 Ohm resistor in series with one of the digital pins on the Arduino Mega.

2. Open the Arduino IDE and create a new sketch.

3. Write the code to configure the digital pin as an output and control the LED blinking.

4. Upload the sketch to your Arduino Mega board and observe the LED blinking.

5. Modify the code to change the blinking rate or pattern.

## Speaker Output Tone

1. Assemble the circuit on your breadboard according to the provided schematic, connecting a small 8-Ohm speaker to one of the digital pins on the Arduino Mega.

2. Open the Arduino IDE and create a new sketch.

3. Write the code to configure the digital pin as an output and control the speaker output tone.

4. Upload the sketch to your Arduino Mega board and listen to the tone produced by the speaker.

5. Modify the code to change the frequency or duration of the tone.

## Conclusion

In this lab, you have successfully installed and set up the Arduino IDE, built and run a basic sketch using the Arduino Libraries, and demonstrated blinking light code and speaker output tone. You are now ready to explore more complex projects using the Arduino Mega platform.
