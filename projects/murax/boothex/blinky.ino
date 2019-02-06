#define LED (*(volatile uint32_t*)0xF0000004)
#define UART (*(volatile uint32_t*)0xF0010000)
#define PWM (*(volatile uint32_t*)0xF0030000)
#define PERIOD (*(volatile uint32_t*)0xF0040000)
#define DURATION (*(volatile uint32_t*)0xF0040004)

void analogWrite(int pin, int value) {
  PWM = value;
}

void tone(int pin, unsigned int frequency, unsigned long duration = 0xFFFFFFFF) {
  PERIOD = 1000000 / frequency;
  DURATION = duration;  
}

void noTone(int pin) {
  DURATION = 0;
}

void putch(char c) {
  UART = c;
}

void print(const char *p) {
  while (*p) putch(*(p++));
}

int ledState = 0;
long previousMillis = 0, currentMillis = 0;
const long interval = 100000;

void setup() { 
  LED = 0xFF;
  analogWrite(0, 127);
  tone(1, 262, 30000);
}

void loop()
{
  currentMillis++;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == 0) ledState = 0x0F;
    else ledState = ledState / 2;
    LED = ledState;
    print("\Blink\n");
  }
}
