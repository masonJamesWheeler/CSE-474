// Include necessary libraries. The arduinoFFT library allows for Fast Fourier Transform computations,
// the Final library is user-defined containg definitions for our final project,
// the Arduino_FreeRTOS is a real-time operating system for Arduino, 
// the queue library allows for inter-task communication, 
// task.h is the API for controlling real-time tasks, 
// and Encoder.h is a library for reading rotary encoders.
#include <arduinoFFT.h>
#include "Final.h"
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <Encoder.h>

// The joystick variables store the current state of the joystick.
// 'joystickMoved' is a boolean indicating whether the joystick has been moved,
// 'joystickX' and 'joystickY' store the current X and Y positions of the joystick,
// and 'joystickPressed' is a boolean indicating whether the joystick button has been pressed.
bool joystickMoved = false;
int joystickX = 0;
int joystickY = 0;
bool joystickPressed = false;

// LCD related variables. 'charIndex' is an array storing the index of each character on the LCD,
// and 'alphabetIndex' stores the current index in the alphabet (for scrolling through letters).
int charIndex[LCD_COLS * LCD_ROWS];  
int alphabetIndex = 0;

// The 'message' variable is a string to store a formatted message that will be displayed on the LCD.
char message[40];  

// Score related variables. 'teamAScore' and 'teamBScore' store the current scores for each team,
// 'quarter' stores the current quarter of the game,
// 'teamAName' and 'teamBName' store the names of the teams.
int teamAScore = 0;
int teamBScore = 0;
int quarter = 1;
char teamAName[7] = "Team A";  
char teamBName[7] = "Team B";  

// Variables related to the selected option in a menu or interface.
// 'selectedOption' stores the currently selected option,
// 'selectedLetter' stores the index of the currently selected letter in the alphabet,
// 'optionStartIndexes' is an array of the starting indexes of each option in the 'message' string,
// 'optionLengths' is an array of the lengths of each option,
// and 'numOptions' calculates the number of options based on the length of the 'optionStartIndexes' array.
int selectedOption = -1;
int selectedLetter = 0;  
int optionStartIndexes[] = {0, 10, 17, 22, 29};  
int optionLengths[] = {6, 6, 2, 5, 2};  
int numOptions = sizeof(optionStartIndexes) / sizeof(int); 

// A counter variable that starts at 600.
static unsigned long count = 600;

// Variables related to the encoder position.
// 'encoderPos' stores the current position of the encoder (and is volatile because it may be changed in an interrupt routine),
// 'myEncoder' is an Encoder object,
// and 'oldPosition' stores the previous position of the encoder.
volatile long encoderPos = 0;
Encoder myEncoder(ROTARY_CLK, ROTARY_DT);
long oldPosition  = -999;

// Timer related variables. 'timerMinutes' and 'timerSeconds' store the current time on a game timer.
int timerMinutes = 12;
int timerSeconds = 0;

/**
 * @brief Initializes the board and creates tasks for joystick input, LCD screen update, countdown, buzzer and LED control, and rotary encoder input.
 * 
 * @return void
 */
void setup() {
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    // initialize sensor pins as inputs:
    pinMode(JOY_X, INPUT);
    pinMode(JOY_Y, INPUT);
    pinMode(JOY_SW, INPUT_PULLUP);  // Enable internal pull-up resistor
    pinMode(SENSOR2_PIN, INPUT);
    pinMode(SENSOR3_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(CLOCK_SWITCH, INPUT_PULLUP);
    pinMode(ROTARY_CLK, INPUT);
    pinMode(ROTARY_DT, INPUT);
    pinMode(ROTARY_SW, INPUT_PULLUP);  // Enable internal pull-up resistor
    pinMode(OFFBOARD_LED_PIN, OUTPUT); // initialize off-board LED pin as output:

    // initialize 7-segment display
    displaySetup();

    // initialize LCD
    lcdInit();
    lcdClear();
    lcdSetCursor(0, 0);

    // Format and print the Message on the LCD Screen
    sprintf(message, "%s    %s %02d   Qtr:%d  %02d  ", teamAName, teamBName, teamAScore, quarter, teamBScore);
    lcdPrint(message);

    // Create the Tasks
    xTaskCreate(TaskJoyStick, "JoyStick", 128, NULL, 1, NULL);
    xTaskCreate(TaskLCD, "LCD", 128, NULL, 1, NULL);
    xTaskCreate(TaskCountdown, "Countdown", 128, NULL, 1, NULL);
    xTaskCreate(TaskBuzzerAndLED, "BuzzerAndLED", 128, NULL, 1, NULL);
    xTaskCreate(TaskRotaryEncoder, "Encoder", 128, NULL, 1, NULL);
    xTaskCreate(TaskLEDFlash, "LEDFlash", 128, NULL, 1, NULL);  // Add the new task here


    // Start the scheduler
    vTaskStartScheduler();
}

    void loop()
    {
        // Empty. Things are done in Tasks.
    }

    /*--------------------------------------------------*/
    /*---------------------- Tasks ---------------------*/
    /*--------------------------------------------------*/

    /**
     * @brief Task function to read joystick input and update global variables accordingly.
     * 
     * @param pvParameters Pointer to task parameters (not used in this function).
     * @return void
     */
    void TaskJoyStick(void *pvParameters) {
        for (;;) {
            int x = analogRead(JOY_X);
            int y = analogRead(JOY_Y);
            int sw = digitalRead(JOY_SW);

            if (x != 512 || y != 512 || sw == LOW) {  // If joystick is not in the stationary position
                joystickMoved = true;
                joystickX = x;
                joystickY = y;
                joystickPressed = (sw == LOW);
            }
            vTaskDelay(pdMS_TO_TICKS(100));  // Short delay before next read
        }
    }
/**
 * @brief Task function to update the LCD screen.
 *      This task should be responsible for updating the LCD screen based on the joystick input.
 *    The LCD screen should display the following information:
 *    - Team A name
 *   - Team B name
 *  - Team A score
 * - Team B score
 * - Quarter
 * 
 * The LCD screen should also display a blinking cursor to indicate which option is selected.
 * 
 * The LCD screen should be updated every 100 ms.
 * 
 * @param pvParameters Pointer to task parameters (not used in this function).
 * 
 * @return void
 * 
 * 
*/
 void TaskLCD(void *pvParameters) {
    int selectedOption = -1;  // -1 means no option is selected
    unsigned long lastBlinkTime = 0;
    bool blinkState = false;

    for (;;) {
        // Move selected option or letter based on joystick position
        if (joystickMoved) {
            if (selectedOption == -1) {  // If no option is selected
                if (joystickX < JOY_THRESHOLD && selectedLetter > 0) {
                    selectedLetter--;
                } else if (joystickX > 1023 - JOY_THRESHOLD && selectedLetter < numOptions - 1) {
                    selectedLetter++;
                }
            } else {  // If an option is selected
                if (joystickX < JOY_THRESHOLD && selectedLetter > optionStartIndexes[selectedOption]) {
                    selectedLetter--;
                } else if (joystickX > 1023 - JOY_THRESHOLD && selectedLetter < optionStartIndexes[selectedOption] + optionLengths[selectedOption] - 1) {
                    selectedLetter++;
                }
            }
        }
        if (joystickMoved && selectedOption != -1) {  // If an option is selected
            if (selectedOption == 2 || selectedOption == 3 || selectedOption == 4) {  // If the selected option is a score or quarter
                if (joystickY < JOY_THRESHOLD) {
                    if (selectedOption == 2) {
                        teamAScore++;
                        if (teamAScore >= 99) teamAScore = 99;  // Don't allow negative scores
                    } else if (selectedOption == 3 && quarter < 4) {
                        quarter++;
                    } else if (selectedOption == 4) {
                        teamBScore++;
                        if (teamBScore >= 99) teamBScore = 99;  // Don't allow negative scores
                    }
                } else if (joystickY > 1023 - JOY_THRESHOLD) {
                    if (selectedOption == 2 && teamAScore > 0) {
                        teamAScore--;
                    } else if (selectedOption == 3 && quarter > 1) {
                        quarter--;
                    } else if (selectedOption == 4 && teamBScore > 0) {
                        teamBScore--;
                    }
                }
                // Update the message with the new scores and quarter
             sprintf(message, "%s    %s %02d   Qtr:%d  %02d  ", teamAName, teamBName, teamAScore, quarter, teamBScore);
            } else {
                for (int i = 0; i < strlen(ALPHABET); i++) {
                    if (selectedOption == 0 && ALPHABET[i] == teamAName[selectedLetter - optionStartIndexes[selectedOption]]) {
                        alphabetIndex = i;
                        break;
                    } else if (selectedOption == 1 && ALPHABET[i] == teamBName[selectedLetter - optionStartIndexes[selectedOption]]) {
                        alphabetIndex = i;
                        break;
                    }
                }
                if (joystickY < JOY_THRESHOLD) {
                    if (alphabetIndex == strlen(ALPHABET) - 1) {
                        alphabetIndex = 0;
                    } else {
                        alphabetIndex++;
                    }
                    if (selectedOption == 0) {
                        teamAName[selectedLetter - optionStartIndexes[selectedOption]] = ALPHABET[alphabetIndex];
                    } else if (selectedOption == 1) {
                        teamBName[selectedLetter - optionStartIndexes[selectedOption]] = ALPHABET[alphabetIndex];
                    }
                } else if (joystickY > 1023 - JOY_THRESHOLD) {
                    if (alphabetIndex == 0) {
                        alphabetIndex = strlen(ALPHABET) - 1;
                    } else {
                        alphabetIndex--;
                    }
                    if (selectedOption == 0) {
                        teamAName[selectedLetter - optionStartIndexes[selectedOption]] = ALPHABET[alphabetIndex];
                    } else if (selectedOption == 1) {
                        teamBName[selectedLetter - optionStartIndexes[selectedOption]] = ALPHABET[alphabetIndex];
                    }
                }
                // Update the message with the new team name
                sprintf(message, "%s    %s %02d   Qtr:%d  %02d  ", teamAName, teamBName, teamAScore, quarter, teamBScore);
            }
            joystickMoved = false;
        }

        // Select or deselect option based on joystick press
        if (joystickPressed) {
            if (selectedOption == -1) {
                selectedOption = selectedLetter;
                selectedLetter = optionStartIndexes[selectedOption];  // Start at the first character of the selected option
            } else {
                selectedOption = -1;
                selectedLetter = 0;  // Reset to the first option
            }
            // waste some time while the button is held down
            while (digitalRead(JOY_SW) == LOW) {
                vTaskDelay(pdMS_TO_TICKS(50));
            }
            joystickPressed = false;
        }

        for (int i = 0; i < LCD_COLS * LCD_ROWS; i++) {
            if ((selectedOption == -1 && i >= optionStartIndexes[selectedLetter] && i < optionStartIndexes[selectedLetter] + optionLengths[selectedLetter]) || 
                (selectedOption != -1 && selectedOption != 0 && selectedOption != 1 && i >= optionStartIndexes[selectedOption] && i < optionStartIndexes[selectedOption] + optionLengths[selectedOption]) || 
                (selectedOption == 0 && i == selectedLetter) || 
                (selectedOption == 1 && i == selectedLetter)) {
                if (millis() - lastBlinkTime >= BLINK_DELAY) {
                    blinkState = !blinkState;
                    lastBlinkTime = millis();
                }
                if (blinkState) {
                    lcdSetCursor(i % LCD_COLS, i / LCD_COLS);
                    lcdPrint(String(message[i]).c_str());
                } else {
                    lcdSetCursor(i % LCD_COLS, i / LCD_COLS);
                    lcdPrint(" ");
                }
            } else {
                lcdSetCursor(i % LCD_COLS, i / LCD_COLS);
                lcdPrint(String(message[i]).c_str());
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Short delay before next update
    }
}   

/**
 * @brief Task function to update and display a countdown timer on a 7-segment display.
 * 
 * This function updates and displays a countdown timer on a 7-segment display. The countdown timer
 * is initialized to 10 minutes (600 seconds) and decrements by tenths of a second. The timer is
 * only decremented if the CLOCK_SWITCH pin is HIGH. The function converts the countdown timer into
 * minutes, seconds, and tenths, and then converts each of these values into separate digits to be
 * displayed on the 7-segment display. The function uses a static array to store the display states
 * of each digit, and cycles through these states to display each digit on the 7-segment display.
 * 
 * @param pvParameters Pointer to task parameters (not used in this function).
 * @return void
 */
void TaskCountdown(void *pvParameters) {
    // Initialize the countdown timer to 10 minutes (600 seconds)
    count = 600; // Count in tenths of a second
    int digits[4]; // Array to store digits of count
    static int displayStates[4] = {S0, S1, S2, S3}; // Array to store display states

    for (;;) {
        // Start a timer to count to 100 milliseconds
        unsigned long start = millis();
        Serial.println(digitalRead(CLOCK_SWITCH));
        // Decrement the countdown timer only if CLOCK_SWITCH is HIGH
        if (digitalRead(CLOCK_SWITCH) == HIGH && count > 0) {
            count--;
        }

        // Convert the count into minutes, seconds, and tenths
        int minutes = count / 600;
        int seconds = (count % 600) / 10;
        int tenths = count % 10;

        // Convert the minutes, seconds, and tenths into separate digits
        convert(digits, minutes, seconds, tenths);

        while (millis() - start < 100) { 
            // Display the countdown timer on the 7-segment display
            for (int h = 0; h < 4; h++) {
                PORTB = displayStates[h]; // Select the digit
                byte *disp = seven_seg_digits[digits[h]]; // Get the 7-segment code for the digit
                send7(disp); // Send the 7-segment code to the display
                delayMicroseconds(2500); // Add a short delay after setting each digit
            }
        }
    }
}

/**
 * @brief Sends a command to the LCD display.
 * 
 * This function sends a command to the LCD display by setting the RS pin to LOW and sending the command
 * to the display through the D4, D5, D6, and D7 pins. It then pulses the EN pin to signal the
 * display to read the command.
 * 
 * @param cmd The command to be sent to the display.
 * @return void
 */
void lcdCommand(uint8_t cmd) {
    digitalWrite(RS, LOW);  // Set RS pin to LOW to send command
    digitalWrite(D4, (cmd >> 4) & 1);  // Send the first 4 bits of command through D4
    digitalWrite(D5, (cmd >> 5) & 1);  // Send the next 4 bits of command through D5
    digitalWrite(D6, (cmd >> 6) & 1);  // Send the next 4 bits of command through D6
    digitalWrite(D7, (cmd >> 7) & 1);  // Send the last 4 bits of command through D7
    digitalWrite(EN, HIGH);  // Pulse the EN pin to signal the display to read the command
    delayMicroseconds(1);
    digitalWrite(EN, LOW);
    digitalWrite(D4, cmd & 1);  // Send the first 4 bits of command through D4
    digitalWrite(D5, (cmd >> 1) & 1);  // Send the next 4 bits of command through D5
    digitalWrite(D6, (cmd >> 2) & 1);  // Send the next 4 bits of command through D6
    digitalWrite(D7, (cmd >> 3) & 1);  // Send the last 4 bits of command through D7
    digitalWrite(EN, HIGH);  // Pulse the EN pin to signal the display to read the command
    delayMicroseconds(1);
    digitalWrite(EN, LOW);
    delayMicroseconds(37);  // Wait for the command to be executed
}

/**
 * @brief Sends data to the LCD display.
 * 
 * This function sends data to the LCD display by setting the RS pin to HIGH and sending the data
 * to the display through the D4, D5, D6, and D7 pins. It then pulses the EN pin to signal the
 * display to read the data.
 * 
 * @param data The data to be sent to the display.
 * @return void
 */
void lcdData(uint8_t data) {
    digitalWrite(RS, HIGH);  // Set RS pin to HIGH to send data
    digitalWrite(D4, (data >> 4) & 1);  // Send the first 4 bits of data through D4
    digitalWrite(D5, (data >> 5) & 1);  // Send the next 4 bits of data through D5
    digitalWrite(D6, (data >> 6) & 1);  // Send the next 4 bits of data through D6
    digitalWrite(D7, (data >> 7) & 1);  // Send the last 4 bits of data through D7
    digitalWrite(EN, HIGH);  // Pulse the EN pin to signal the display to read the data
    delayMicroseconds(1);
    digitalWrite(EN, LOW);
    digitalWrite(D4, data & 1);  // Send the first 4 bits of data through D4
    digitalWrite(D5, (data >> 1) & 1);  // Send the next 4 bits of data through D5
    digitalWrite(D6, (data >> 2) & 1);  // Send the next 4 bits of data through D6
    digitalWrite(D7, (data >> 3) & 1);  // Send the last 4 bits of data through D7
    digitalWrite(EN, HIGH);  // Pulse the EN pin to signal the display to read the data
    delayMicroseconds(1);
    digitalWrite(EN, LOW);
    delayMicroseconds(37);  // Wait for the display to process the data
}

/**
 * @brief Initializes the LCD display.
 * 
 * This function initializes the LCD display by setting the pin modes for RS, EN, D4, D5, D6, and D7.
 * It then sends a series of commands to the display to set it up for use.
 * 
 * @param None.
 * @return void
 */
void lcdInit() {
    pinMode(RS, OUTPUT);   // Set RS pin as output
    pinMode(EN, OUTPUT);   // Set EN pin as output
    pinMode(D4, OUTPUT);   // Set D4 pin as output
    pinMode(D5, OUTPUT);   // Set D5 pin as output
    pinMode(D6, OUTPUT);   // Set D6 pin as output
    pinMode(D7, OUTPUT);   // Set D7 pin as output
    delay(50);             // Wait for 50ms
    lcdCommand(0x33);      // Send command to initialize the display
    lcdCommand(0x32);      // Send command to initialize the display
    lcdCommand(0x28);      // Send command to initialize the display
    lcdCommand(0x0C);      // Send command to initialize the display
    lcdCommand(0x06);      // Send command to initialize the display
    lcdCommand(0x01);      // Send command to initialize the display
    delay(2);              // Wait for 2ms
}

/**
 * @brief Clears the LCD display.
 * 
 * This function sends the command to clear the LCD display and adds a delay of 2 milliseconds to allow the display to clear.
 * 
 * @param None.
 * @return void
 */
void lcdClear() {
    lcdCommand(0x01);
    delay(2);
}

/**
 * @brief Sets the cursor position on the LCD display.
 * 
 * This function sets the cursor position on the LCD display based on the given column and row values.
 * If the given column value is greater than or equal to the number of columns on the display, it sets the column to the last column.
 * If the given row value is greater than or equal to the number of rows on the display, it sets the row to the last row.
 * 
 * @param col The column number to set the cursor to.
 * @param row The row number to set the cursor to.
 * @return void
 */
void lcdSetCursor(uint8_t col, uint8_t row) {
    if (col >= LCD_COLS) {
        col = LCD_COLS - 1;
    }
    if (row >= LCD_ROWS) {
        row = LCD_ROWS - 1;
    }
    uint8_t address = (row == 0) ? 0x00 + col : 0x40 + col;
    lcdCommand(0x80 | address);
}

    /**
     * @brief Prints a string to the LCD display.
     * 
     * This function prints a string to the LCD display by iterating through each character in the string and sending it to the lcdData function.
     * 
     * @param str Pointer to the string to be printed.
     * @return void
     */
void lcdPrint(const char* str) {
    while (*str) {
        lcdData(*str++);
    }    
}

/**
 * @brief Task function to control the buzzer and LED based on button press and timer countdown.
 * 
 * This function checks if the button is pressed or the timer has hit zero, and turns on the buzzer and LED accordingly.
 * If the button is not pressed and the timer has not hit zero, the buzzer and LED are turned off.
 * 
 * @param pvParameters Pointer to task parameters (not used in this function).
 * @return void
 */
void TaskBuzzerAndLED(void *pvParameters) {
    for (;;) {
        // Check if the button is pressed or the timer hit zero
        if (digitalRead(BUTTON_PIN) == HIGH || count == 0) {
            // Put a Tone on the Buzzer
            tone(BUZZER_PIN, 1000);

            digitalWrite(LED_PIN, HIGH);

            // Delay for a while
            vTaskDelay(pdMS_TO_TICKS(200));
        }
         else {
            // Turn off the Buzzer
            noTone(BUZZER_PIN);
            digitalWrite(LED_PIN, LOW);
    }

    // Delay for a while
    vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Task function to read the rotary encoder input and update the global count variable accordingly.
 * 
 * This function reads the rotary encoder input and updates the global count variable based on the direction of rotation.
 * If the rotary encoder button is pressed, the count variable is reset to 600 and the encoder's position is set to 0.
 * 
 * @param pvParameters Pointer to task parameters (not used in this function).
 * @return void
 */
void TaskRotaryEncoder(void *pvParameters) {
    for (;;) {
        long newPosition = myEncoder.read();
        if (newPosition != oldPosition) {
            if (newPosition > oldPosition) {
                count = count + 10;
            } else if (newPosition < oldPosition) {
                count = count - 10;
            }
            Serial.println(count);
            oldPosition = newPosition;
        }

        if (digitalRead(ROTARY_SW) == LOW) {
            count = 600;
            myEncoder.write(0); // We reset the encoder's position to 0
            Serial.println(count);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief Task function to flash an off-board LED on and off. The LED is ON for 100ms and OFF for 200ms.
 * 
 * @param pvParameters Pointer to task parameters (not used in this function).
 * @return void
 */
void TaskLEDFlash(void *pvParameters) {
    for (;;) {
        digitalWrite(OFFBOARD_LED_PIN, HIGH);  // Turn on the LED
        vTaskDelay(pdMS_TO_TICKS(100));       // Delay for 100ms
        digitalWrite(OFFBOARD_LED_PIN, LOW);   // Turn off the LED
        vTaskDelay(pdMS_TO_TICKS(200));       // Delay for 200ms
    }
}

/**
 * @brief Converts the input time values into an array of digits.
 * 
 * This function takes in the input time values (minutes, seconds, and tenths) and separates
 * their digits, storing them in an array. The array is passed in as a pointer to an integer.
 * 
 * @param digits Pointer to an integer array to store the separated digits.
 * @param minutes The number of minutes in the input time.
 * @param seconds The number of seconds in the input time.
 * @param tenths The number of tenths of a second in the input time.
 * 
 * @return void
 */
void convert(int * digits, int minutes, int seconds, int tenths) {
    // Separate the digits of the minutes, seconds, and tenths and store them in the array
    digits[0] = tenths;
    digits[1] = seconds % 10;
    digits[2] = (seconds/10) % 10;
    digits[3] = minutes;
}
    
/**
 * @brief Sends an array of 7 bytes to the 7-segment display.
 * 
 * @param arr An array of 7 bytes to send to the 7-segment display.
 * 
 * @return void
 */
void send7(byte arr[7]) {
  int data = 0; // initialize a variable to store the data to be sent to the 7-segment display
  for (int i = 0; i < 7; i++) { // loop through each byte in the array
    data |= (arr[i] << i); // set the corresponding bit in the data variable to the value of the byte
  }
  PORTA = data; // send the data to the 7-segment display
}


/**
 * @brief Sets up the pins for the 7-segment display and digit selectors.
 * 
 * @param None
 * 
 * @return void
 */
void displaySetup() {
  // set up digit selectors
  DDRB |= BIT7 | BIT6 | BIT5 | BIT4; // set pins 4-7 on PORTB as output pins

  // set up 7-seg outputs
  DDRA |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0; // set pins 0-7 on PORTA as output pins

  PORTB |= BIT7 | BIT6 | BIT5 | BIT4; // set pins 4-7 on PORTB to HIGH
}

