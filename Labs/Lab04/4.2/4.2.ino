#include <arduinoFFT.h>
#include "sensors.h"
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <Encoder.h>



// the setup function runs once when you press reset or power the board
bool joystickMoved = false;
int joystickX = 0;
int joystickY = 0;
bool joystickPressed = false;

int charIndex[LCD_COLS * LCD_ROWS];  // One for each character on the LCD
int alphabetIndex = 0;

char message[40];  // Make sure this is large enough to hold your formatted string
int teamAScore = 0;
int teamBScore = 0;
int quarter = 1;
char teamAName[7] = "Team A";  // Increase size by 1 to accommodate null terminator
char teamBName[7] = "Team B";  // Increase size by 1 to accommodate null terminator
// Array to store the selected option of teamAScore, teamBScore, quarter, teamAName, teamBName
// Add these variables
int selectedOption = -1;  // -1 means no option is selected
int selectedLetter = 0;  // Index of the selected letter in the alphabet
int optionStartIndexes[] = {0, 10, 17, 22, 29};  // Start indexes of each option in the message
int optionLengths[] = {6, 6, 2, 5, 2};  // Lengths of each option
int numOptions = sizeof(optionStartIndexes) / sizeof(int);  // Number of options
static unsigned long count = 600;
// Global variable to hold the encoder position
volatile long encoderPos = 0;
Encoder myEncoder(ROTARY_CLK, ROTARY_DT);
long oldPosition  = -999;


int timerMinutes = 12;
int timerSeconds = 0;

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


    // functions for manual LCD control
    void lcdCommand(uint8_t cmd) {
        digitalWrite(RS, LOW);
        digitalWrite(D4, (cmd >> 4) & 1);
        digitalWrite(D5, (cmd >> 5) & 1);
        digitalWrite(D6, (cmd >> 6) & 1);
        digitalWrite(D7, (cmd >> 7) & 1);
        digitalWrite(EN, HIGH);
        delayMicroseconds(1);
        digitalWrite(EN, LOW);
        digitalWrite(D4, cmd & 1);
        digitalWrite(D5, (cmd >> 1) & 1);
        digitalWrite(D6, (cmd >> 2) & 1);
        digitalWrite(D7, (cmd >> 3) & 1);
        digitalWrite(EN, HIGH);
        delayMicroseconds(1);
        digitalWrite(EN, LOW);
        delayMicroseconds(37);
    }

    void lcdData(uint8_t data) {
        digitalWrite(RS, HIGH);
        digitalWrite(D4, (data >> 4) & 1);
        digitalWrite(D5, (data>> 5) & 1);
        digitalWrite(D6, (data >> 6) & 1);
        digitalWrite(D7, (data >> 7) & 1);
        digitalWrite(EN, HIGH);
        delayMicroseconds(1);
        digitalWrite(EN, LOW);
        digitalWrite(D4, data & 1);
        digitalWrite(D5, (data >> 1) & 1);
        digitalWrite(D6, (data >> 2) & 1);
        digitalWrite(D7, (data >> 3) & 1);
        digitalWrite(EN, HIGH);
        delayMicroseconds(1);
        digitalWrite(EN, LOW);
        delayMicroseconds(37);
    }

    void lcdInit() {
        pinMode(RS, OUTPUT);
        pinMode(EN, OUTPUT);
        pinMode(D4, OUTPUT);
        pinMode(D5, OUTPUT);
        pinMode(D6, OUTPUT);
        pinMode(D7, OUTPUT);
        delay(50);
        lcdCommand(0x33);
        lcdCommand(0x32);
        lcdCommand(0x28);
        lcdCommand(0x0C);
        lcdCommand(0x06);
        lcdCommand(0x01);
        delay(2);
    }

    void lcdClear() {
        lcdCommand(0x01);
        delay(2);
    }

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

    void lcdPrint(const char* str) {
        while (*str) {
            lcdData(*str++);
        }    
    }

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
