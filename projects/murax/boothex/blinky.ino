#define LED (*(volatile uint32_t*)0xF0000004)
#define UART (*(volatile uint32_t*)0xF0010000)
#define PWM (*(volatile uint32_t*)0xF0030000)

void putch(char c) {
  UART = c;
}

void print(const char *p) {
  while (*p) putch(*(p++));
}

int ledState = 0;
int previousMillis = 0;
const long interval = 100000;

void setup() { 
  LED = 0xFF;
  PWM = 191;
}

void loop()
{
static int currentMillis = 0;
currentMillis++;
if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (ledState == 0) ledState = 0x0F;
    else ledState = ledState / 2;
    LED = ledState;
    print("\Blink\n");
  }
}
