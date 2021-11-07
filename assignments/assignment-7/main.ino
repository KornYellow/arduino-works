#include <SPI.h>
#include <Wire.h>
#include <TimerOne.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Config
#define IS_DEBUG_MODE true

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

// Clock Mode
#define MODE_NORMAL 0
#define MODE_SELECT_MODE 1
#define MODE_EDIT_NORMAL 2
#define MODE_EDIT_ALARM 3
#define MODE_STOP_WATCH 4
#define MODE_TIMER 5

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

// BUTTON
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_OK 5

// Clock Mode
void clockModeNormal();
void clockModeSelectMode();
void clockModeEditNormal();
void clockModeEditAlarm();
void clockModeStopWatch();
void clockModeTimer();

// TimerOne
void timerCount();

// For Drawing
void drawTextMiddle(int y, const char* text, int size, int color);
void drawText(int x, int y, const char* text, int size, int color);

// Others
int calculateTextWidth(int text_length, int text_size);
int calculateTextLength(const char* text);

// Declaration and Initialization
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int clock_mode = MODE_NORMAL;

bool is_first_interrupt = true;
int time_seconds = 0;
int time_minutes = 0;
int time_hours = 0;
int time_days = 0;
int time_months = 0;
int time_years = 0;
char time[9] = "00:00:00";

int mode_selection = 0;

void setup() {

    // Serial Initialize
    Serial.begin(9600);
    while(!Serial);
    if(IS_DEBUG_MODE) Serial.println("- Serial ready!");

    // Screen Initialize
    while(!screen.begin(SSD1306_SWITCHCAPVCC, 0x3D));
    if(IS_DEBUG_MODE) Serial.println("- OLED ready!");

    // TimerOne Initialize
    Timer1.initialize();
    Timer1.attachInterrupt(timerCount);
    if(IS_DEBUG_MODE) Serial.println("- TimerOne ready!");

    // Button Initialize
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(BUTTON_OK, INPUT_PULLUP);
}

void loop() {

    screen.clearDisplay();
    
    clockModeNormal();
    clockModeSelectMode();
    clockModeEditNormal();
    clockModeEditAlarm();
    clockModeStopWatch();
    clockModeTimer();
    
    screen.display();
}

void clockModeNormal() {

    if(clock_mode != MODE_NORMAL) return;

    // Change to Mode Select Mode
    if(isButtonPressed(BUTTON_OK)) {

        clock_mode = MODE_SELECT_MODE;
        return;
    }

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);
    drawTextMiddle(25, "07/12/2564", 1, SSD1306_WHITE);
    drawTextMiddle(9, time, 2, SSD1306_WHITE);
}
void clockModeSelectMode() {

    if(clock_mode != MODE_SELECT_MODE) return;

    drawTextMiddle(0, "-----Select Menu-----", 1, SSD1306_WHITE);
    drawText(6,  8, "Back", 1, SSD1306_WHITE);
    drawText(6, 16, "Edit Time", 1, SSD1306_WHITE);
    drawText(6, 24, "Set Alarm", 1, SSD1306_WHITE);

    drawText(72,  8, "Set Mode", 1, SSD1306_WHITE);
    drawText(72, 16, "Exit Save", 1, SSD1306_WHITE);
    drawText(72, 24, "Exit", 1, SSD1306_WHITE);
}
void clockModeEditNormal() {

    if(clock_mode != MODE_EDIT_NORMAL) return;
}
void clockModeEditAlarm() {

    if(clock_mode != MODE_EDIT_ALARM) return;
}
void clockModeStopWatch() {

    if(clock_mode != MODE_STOP_WATCH) return;
}
void clockModeTimer() {

    if(clock_mode != MODE_TIMER) return;
}

void timerCount() {

    // Prevent Initialization Interrupt
    if(is_first_interrupt) {

        is_first_interrupt = false;
        return;
    }

    // Calculate Time
    time_seconds++;
    if(time_seconds >= 60) time_minutes++;
    if(time_minutes >= 60) time_hours++;

    time_seconds %= 60;
    time_minutes %= 60;
    time_hours %= 24;

    // Set Seconds
    time[7] = (time_seconds % 10) + '0';
    time[6] = (time_seconds / 10) + '0';

    // Set Minutes
    time[4] = (time_minutes % 10) + '0';
    time[3] = (time_minutes / 10) + '0';

    // Set Hours
    time[1] = (time_hours % 10) + '0';
    time[0] = (time_hours / 10) + '0';

    if(IS_DEBUG_MODE) {
        
        String time_string = time;
        Serial.println("- Time Now : " + time_string);
    }
}

void drawTextMiddle(int y, const char* text, int size, int color) {

    screen.setTextSize(size);
    screen.setTextColor(color);
    screen.setCursor((SCREEN_WIDTH/2) - calculateTextWidth(calculateTextLength(text), size), y);
    screen.println(text);
}
void drawText(int x, int y, const char* text, int size, int color) {

    screen.setTextSize(size);
    screen.setTextColor(color);
    screen.setCursor(x, y);
    screen.println(text);
} 

int calculateTextWidth(int text_length, int text_size) {

    int font_size = text_size * 5;
    int result = ((font_size * text_length) + (text_length - 1)) / 2;
    if(result % 2 != 0) return result + text_size;
    return result;
}
int calculateTextLength(const char* text) {

    int size = 0;
    while (text[size] != '\0') size++;
    return size;
}

bool isButtonPressed(int pin) {
    
    delay(10);
    if(digitalRead(pin) == LOW) return true;
    return false;
}