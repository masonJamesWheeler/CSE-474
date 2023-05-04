#define SPEAKER_PIN 6

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  
  TCCR4A = (1 << COM4A1) | (1 << WGM41); // Clear OC4A on Compare Match when up-counting, set OC4A on Compare Match when down-counting, Fast PWM mode
  TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Fast PWM mode, prescaler 8
  ICR4 = 40000; // TOP value for Timer/Counter 4
}

void loop() {
  play_tone(400, 1000); // 400 Hz for 1 second
  play_tone(250, 1000); // 250 Hz for 1 second
  play_tone(800, 1000); // 800 Hz for 1 second
  silence(1000); // Silence for 1 second
}

void play_tone(uint16_t frequency, uint32_t duration) {
  ICR4 = F_CPU / (8 * frequency); // Calculate the TOP value based on the frequency
  OCR4A = ICR4 / 2; // Set the duty cycle to 50%
  delay(duration);
}

void silence(uint32_t duration) {
  TCCR4A &= ~(1 << COM4A1); // Disconnect OC4A from the Timer/Counter
  delay(duration);
  TCCR4A |= (1 << COM4A1); // Reconnect OC4A to the Timer/Counter
}

