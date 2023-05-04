/*

Place the LEDs and resistors on the breadboard:
Insert the anode (longer leg) of the first LED into a row on the breadboard, and connect its cathode (shorter leg) to another row.
Connect a 500-250 Ohm resistor to the cathode row of the LED and connect the other end of the resistor to the ground rail of the breadboard.
Repeat these steps for the second and third LEDs, leaving some space between them for easy identification and cable management.

*/

const int ledPin1 = 47;
const int ledPin2 = 48;
const int ledPin3 = 49;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
}

void loop() {
  digitalWrite(ledPin1, HIGH);
  delay(333);
  digitalWrite(ledPin1, LOW);

  digitalWrite(ledPin2, HIGH);
  delay(333);
  digitalWrite(ledPin2, LOW);

  digitalWrite(ledPin3, HIGH);
  delay(333);
  digitalWrite(ledPin3, LOW);
}