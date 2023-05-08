#define LED_PIN_47_BIT 0
#define LED_PIN_48_BIT 1
#define LED_PIN_49_BIT 2

void setup() {
  // Set pins 47, 48, 49 as outputs
  DDRL |= (1 << LED_PIN_47_BIT) | (1 << LED_PIN_48_BIT) | (1 << LED_PIN_49_BIT);
}

void loop() {
  // Turn on LED at pin 47
  PORTL |= (1 << LED_PIN_47_BIT);
  delay(333);
  // Turn off LED at pin 47
  PORTL &= ~(1 << LED_PIN_47_BIT);

  // Turn on LED at pin 48
  PORTL |= (1 << LED_PIN_48_BIT);
  delay(333);
  // Turn off LED at pin 48
  PORTL &= ~(1 << LED_PIN_48_BIT);

  // Turn on LED at pin 49
  PORTL |= (1 << LED_PIN_49_BIT);
  delay(333);
  // Turn off LED at pin 49
  PORTL &= ~(1 << LED_PIN_49_BIT);
}