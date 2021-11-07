#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Config
#define IS_DEBUG_MODE true

// Clock Mode
#define MODE_NORMAL 0
#define MODE_SELECT_MENU 1
#define MODE_SELECT_MODE 2
#define MODE_EDIT_NORMAL 3
#define MODE_EDIT_ALARM 4
#define MODE_STOP_WATCH 5
#define MODE_TIMER 6

#define EDIT_HOURS 0
#define EDIT_MINUTES 1
#define EDIT_SECONDS 2
#define TIMER_RUN 4

#define STOPWATCH_WAIT 0
#define STOPWATCH_START 1
#define STOPWATCH_END 2

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

// BUTTON
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_OK 5

// BUZZER
#define BUZZER 6

// Declaration and Initialization
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int clock_mode = MODE_NORMAL;

bool is_blink = false;
bool is_first_interrupt = true;
bool blink_enable = false;
int time_seconds = 0;
int time_minutes = 0;
int time_hours = 12;
char time[9] = "00:00:00";
char time_edit[9] = "00:00:00";

int edit_time_seconds = 0;
int edit_time_minutes = 0;
int edit_time_hours = 0;
int edit_normal_mode = EDIT_HOURS;

bool button_pressed_left = false;
bool button_pressed_right = false;
bool button_pressed_ok = false;

int stopwatch_mode = STOPWATCH_WAIT;
int stopwatch_seconds = 0;
int stopwatch_minutes = 0;
int stopwatch_hours = 0;
bool is_stopwatch_running = false;

bool is_timer_running = false;
int timer_seconds = 0;
int timer_minutes = 0;
int timer_hours = 0;

bool is_alarm = 0;
int alarm_seconds = -1;
int alarm_minutes = -1;
int alarm_hours = -1;

int menu_selection = 0;
int mode_selection = 0;

int save_text_timer = 0;

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

    // Buzzer Initialize
    pinMode(BUZZER, OUTPUT);

    // Load
    if(EEPROM.read(0) == 1) {

        // Load Time
        EEPROM.get(1, time_seconds);
        EEPROM.get(2, time_minutes);
        EEPROM.get(3, time_hours);

        // Load Alarm
        EEPROM.get(4, alarm_seconds);
        EEPROM.get(5, alarm_minutes);
        EEPROM.get(6, alarm_hours);

        if(IS_DEBUG_MODE) Serial.println("- Data Loaded successfully!"); 
    }
    else if(IS_DEBUG_MODE) Serial.println("- Data Load Failed : Not Found Data"); 
}
void loop() {

    screen.clearDisplay();
    
    clockModeNormal();
    clockModeSelectMenu();
    clockModeSelectMode();
    clockModeEditNormal();
    clockModeEditAlarm();
    clockModeStopwatch();
    clockModeTimer();
    
    screen.display();
}

void clockModeNormal() {

    if(clock_mode != MODE_NORMAL) return;

    // Change to Mode Select Mode
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok && !is_alarm) {

        playTone(500, 30);

        button_pressed_ok = true;
        clock_mode = MODE_SELECT_MENU;
        return;
    }
    else if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        is_alarm = false;
        button_pressed_ok = true;
    }
    if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false; 

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);
    drawTextMiddle(24, "( Normal )", 1, SSD1306_WHITE);
    drawTextMiddle(9, time, 2, SSD1306_WHITE);

    // Alarm Check
    if(time_seconds == alarm_seconds && time_minutes == alarm_minutes && time_hours == alarm_hours) is_alarm = true;
    if(is_alarm) {

        playTone(1000, 200);
        playTone(800, 200);
        delay(100);
    }
}
void clockModeSelectMenu() {

    if(clock_mode != MODE_SELECT_MENU) return;

    // Change to Mode Select Mode
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        switch(menu_selection) {
            
            case 0 : // Back
                playTone(500, 30);

                clock_mode = MODE_NORMAL;
                setTimerString(time_seconds, time_minutes, time_hours);
                break;
            
            case 1 : // Set Mode
                playTone(500, 30);

                clock_mode = MODE_SELECT_MODE;
                break;

            case 2 : // Edit Time
                playTone(500, 30);

                clock_mode = MODE_EDIT_NORMAL;
                edit_time_seconds = time_seconds;
                edit_time_minutes = time_minutes;
                edit_time_hours = time_hours;
                break;
            
            case 3 : // Set Alarm
                playTone(500, 30);

                clock_mode = MODE_EDIT_ALARM;
                if(alarm_seconds == -1) {
                    
                    edit_time_seconds = time_seconds;
                    edit_time_minutes = time_minutes;
                    edit_time_hours = time_hours;
                }
                else {

                    edit_time_seconds = alarm_seconds;
                    edit_time_minutes = alarm_minutes;
                    edit_time_hours = alarm_hours;
                }
                break;

            case 4 : // Save
                playTone(700, 50);
                playTone(1000, 30);

                EEPROM.update(0, 1);

                // Save Time
                EEPROM.update(1, time_seconds);
                EEPROM.update(2, time_minutes);
                EEPROM.update(3, time_hours);

                // Save Alarm
                EEPROM.update(4, alarm_seconds);
                EEPROM.update(5, alarm_minutes);
                EEPROM.update(6, alarm_hours);

                if(IS_DEBUG_MODE) Serial.println("- Data Saved"); 

                save_text_timer = 3;
                break;

            default :
                return;
        }

        button_pressed_ok = true;
        menu_selection = 0;
        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;

    // Draw Selectable Mode
    drawTextMiddle(0, "-----Select Menu-----", 1, SSD1306_WHITE);
    drawText(6,  8, "Back", 1, SSD1306_WHITE);
    drawText(6, 16, "Edit Time", 1, SSD1306_WHITE);
    drawText(6, 24, "Save", 1, SSD1306_WHITE);

    drawText(72,  8, "Set Mode", 1, SSD1306_WHITE);
    drawText(72, 16, "Set Alarm", 1, SSD1306_WHITE);

    // Saved Text
    if(save_text_timer) {

        drawTextMiddle(24, "-Saved-", 1, SSD1306_WHITE);
    }

    // Move Cursor
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        menu_selection--;
        save_text_timer = 0;
        button_pressed_left = true;

        if(menu_selection < 0) menu_selection = 4;
        if(IS_DEBUG_MODE) Serial.println("- Menu : " + String(menu_selection));
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {
        
        playTone(500, 30);

        menu_selection++;
        button_pressed_right = true;

        if(menu_selection > 4) menu_selection = 0;
        if(IS_DEBUG_MODE) Serial.println("- Menu : " + String(menu_selection));
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Draw Cursor
    int cursor_x = 0;
    int cursor_y = 8;
    drawText(cursor_x + ((menu_selection % 2) * 66), cursor_y + ((menu_selection / 2) * 8), ">", 1, SSD1306_WHITE);
}
void clockModeSelectMode() {

    if(clock_mode != MODE_SELECT_MODE) return;

    // Change to Mode Select Mode
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        switch(mode_selection) {
            
            case 0 : // Back
                playTone(500, 30);

                clock_mode = MODE_SELECT_MENU;
                break;

            case 1 : // Normal Mode
                playTone(500, 30);

                clock_mode = MODE_NORMAL;
                setTimerString(time_seconds, time_minutes, time_hours);
                break;
            
            case 2 : // Stopwatch Mode
                playTone(500, 30);

                clock_mode = MODE_STOP_WATCH;
                setTimerString(stopwatch_seconds, stopwatch_minutes, stopwatch_hours);
                break;
            
            case 3 : // Timer Mode
                playTone(500, 30);

                clock_mode = MODE_TIMER;
                edit_time_seconds = 0;
                edit_time_minutes = 5;
                edit_time_hours = 0;
                break;
            
            default :
                return;
        }

        button_pressed_ok = true;
        mode_selection = 0;
        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;

    // Draw Selectable Mode
    drawTextMiddle(0, "-----Select Mode-----", 1, SSD1306_WHITE);
    drawText(6,  8, "Back", 1, SSD1306_WHITE);
    drawText(6, 16, "Stopwatch", 1, SSD1306_WHITE);

    drawText(72,  8, "Normal", 1, SSD1306_WHITE);
    drawText(72, 16, "Timer", 1, SSD1306_WHITE);

    // Move Cursor
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {
        
        playTone(500, 30);

        mode_selection--;
        button_pressed_left = true;

        if(mode_selection < 0) mode_selection = 3;
        if(IS_DEBUG_MODE) Serial.println("- Mode : " + String(mode_selection));
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        mode_selection++;
        button_pressed_right = true;

        if(mode_selection > 3) mode_selection = 0;
        if(IS_DEBUG_MODE) Serial.println("- Menu : " + String(mode_selection));
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Draw Cursor
    int cursor_x = 0;
    int cursor_y = 8;
    drawText(cursor_x + ((mode_selection % 2) * 66), cursor_y + ((mode_selection / 2) * 8), ">", 1, SSD1306_WHITE);
}
void clockModeEditNormal() {

    if(clock_mode != MODE_EDIT_NORMAL) return;

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);

    // Edit Time
    editNormalHours();
    editNormalMinutes();
    editNormalSeconds();

    // Set Seconds
    time_edit[7] = (edit_time_seconds % 10) + '0';
    time_edit[6] = (edit_time_seconds / 10) + '0';

    // Set Minutes
    time_edit[4] = (edit_time_minutes % 10) + '0';
    time_edit[3] = (edit_time_minutes / 10) + '0';

    // Set Hours
    time_edit[1] = (edit_time_hours % 10) + '0';
    time_edit[0] = (edit_time_hours / 10) + '0';

    drawTextMiddle(9, time_edit, 2, SSD1306_WHITE);
}
void clockModeEditAlarm() {

    if(clock_mode != MODE_EDIT_ALARM) return;

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);

    // Edit Time
    editNormalHours();
    editNormalMinutes();
    editAlarmSeconds();

    // Set Seconds
    time_edit[7] = (edit_time_seconds % 10) + '0';
    time_edit[6] = (edit_time_seconds / 10) + '0';

    // Set Minutes
    time_edit[4] = (edit_time_minutes % 10) + '0';
    time_edit[3] = (edit_time_minutes / 10) + '0';

    // Set Hours
    time_edit[1] = (edit_time_hours % 10) + '0';
    time_edit[0] = (edit_time_hours / 10) + '0';

    drawTextMiddle(9, time_edit, 2, SSD1306_WHITE);
}
void clockModeStopwatch() {

    if(clock_mode != MODE_STOP_WATCH) return;

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);
    
    stopwatchWait();
    stopwatchStart();
    stopwatchEnd();

    drawTextMiddle(9, time, 2, SSD1306_WHITE);
}
void clockModeTimer() {

    if(clock_mode != MODE_TIMER) return; 

    drawTextMiddle(0, "-----Smart Clock-----", 1, SSD1306_WHITE);
    
    // Edit Time
    editNormalHours();
    editNormalMinutes();
    editTimerSeconds();

    if(edit_normal_mode != TIMER_RUN) {

        // Set Seconds
        time_edit[7] = (edit_time_seconds % 10) + '0';
        time_edit[6] = (edit_time_seconds / 10) + '0';

        // Set Minutes
        time_edit[4] = (edit_time_minutes % 10) + '0';
        time_edit[3] = (edit_time_minutes / 10) + '0';

        // Set Hours
        time_edit[1] = (edit_time_hours % 10) + '0';
        time_edit[0] = (edit_time_hours / 10) + '0';

        drawTextMiddle(9, time_edit, 2, SSD1306_WHITE);
    }
    else {
        
        // Beep
        if(timer_seconds == 0 && timer_minutes == 0 && timer_hours == 0) {
            playTone(1000, 200);
            playTone(800, 200);
            delay(100);
        }

        // End Mode
        if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

            playTone(500, 30);

            button_pressed_ok = true;
            is_timer_running = false;
            blink_enable = false;
            clock_mode = MODE_SELECT_MENU;
            edit_normal_mode = EDIT_HOURS;

            timer_seconds = 0;
            timer_minutes = 0;
            timer_hours = 0;
            return;
        }
        if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false; 

        drawTextMiddle(9, time, 2, SSD1306_WHITE);
        drawTextMiddle(24, "( Timer )", 1, SSD1306_WHITE);
    }
}

void editNormalHours() {

    if(edit_normal_mode != EDIT_HOURS) return;

    drawText(19, 12, "__", 2, SSD1306_WHITE);

    // Decrease
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        edit_time_hours--;
        if(edit_time_hours < 0) edit_time_hours = 23;
        button_pressed_left = true;
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    // Increase
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        edit_time_hours++;
        if(edit_time_hours > 23) edit_time_hours = 0;
        button_pressed_right = true;
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Done
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        playTone(500, 30);

        button_pressed_ok = true;
        edit_normal_mode = EDIT_MINUTES;

        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
}
void editNormalMinutes() {

    if(edit_normal_mode != EDIT_MINUTES) return;

    drawText(55, 12, "__", 2, SSD1306_WHITE);

    // Decrease
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        edit_time_minutes--;
        if(edit_time_minutes < 0) edit_time_minutes = 59;
        button_pressed_left = true;
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    // Increase
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        edit_time_minutes++;
        if(edit_time_minutes > 59) edit_time_minutes = 0;
        button_pressed_right = true;
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Done
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        playTone(500, 30);

        button_pressed_ok = true;
        edit_normal_mode = EDIT_SECONDS;

        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
}
void editNormalSeconds() {

    if(edit_normal_mode != EDIT_SECONDS) return;

    drawText(91, 12, "__", 2, SSD1306_WHITE);

    // Decrease
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        edit_time_seconds--;
        if(edit_time_seconds < 0) edit_time_seconds = 59;
        button_pressed_left = true;
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    // Increase
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        edit_time_seconds++;
        if(edit_time_seconds > 59) edit_time_seconds = 0;
        button_pressed_right = true;
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Done
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        playTone(500, 30);

        button_pressed_ok = true;
        edit_normal_mode = EDIT_HOURS;

        time_seconds = edit_time_seconds;
        time_minutes = edit_time_minutes;
        time_hours = edit_time_hours;

        clock_mode = MODE_NORMAL;
        setTimerString(time_seconds, time_minutes, time_hours);

        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
}
void editTimerSeconds() {

    if(edit_normal_mode != EDIT_SECONDS) return;

    drawText(91, 12, "__", 2, SSD1306_WHITE);

    // Decrease
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        edit_time_seconds--;
        if(edit_time_seconds < 0) edit_time_seconds = 59;
        button_pressed_left = true;
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    // Increase
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        edit_time_seconds++;
        if(edit_time_seconds > 59) edit_time_seconds = 0;
        button_pressed_right = true;
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Done
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        playTone(500, 30);

        button_pressed_ok = true;
        edit_normal_mode = TIMER_RUN;

        timer_seconds = edit_time_seconds;
        timer_minutes = edit_time_minutes;
        timer_hours = edit_time_hours;

        is_timer_running = true;
        blink_enable = true;

        setTimerString(timer_seconds, timer_minutes, timer_hours);

        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
}
void editAlarmSeconds() {
    
    if(edit_normal_mode != EDIT_SECONDS) return;

    drawText(91, 12, "__", 2, SSD1306_WHITE);

    // Decrease
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        playTone(500, 30);

        edit_time_seconds--;
        if(edit_time_seconds < 0) edit_time_seconds = 59;
        button_pressed_left = true;
    }
    else if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;

    // Increase
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        playTone(500, 30);

        edit_time_seconds++;
        if(edit_time_seconds > 59) edit_time_seconds = 0;
        button_pressed_right = true;
    }
    else if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Done
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        playTone(500, 30);

        button_pressed_ok = true;
        edit_normal_mode = EDIT_HOURS;

        alarm_seconds = edit_time_seconds;
        alarm_minutes = edit_time_minutes;
        alarm_hours = edit_time_hours;

        clock_mode = MODE_NORMAL;
        setTimerString(time_seconds, time_minutes, time_hours);

        return;
    }
    else if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
}

void stopwatchWait() {

    if(stopwatch_mode != STOPWATCH_WAIT) return;
    
    if(!is_blink) drawTextMiddle(24, "( Stopwatch )", 1, SSD1306_WHITE);
    else drawTextMiddle(24, "Press any button!", 1, SSD1306_WHITE);

    // Check for Pressing any button
    bool is_pressed_any = false;
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        is_pressed_any = true;
        button_pressed_ok = true;
    }
    if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        is_pressed_any = true;
        button_pressed_left = true;
    }
    if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        is_pressed_any = true;
        button_pressed_right = true;
    }
    if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Switch Mode if pressed any button
    if(is_pressed_any) {

        playTone(500, 30);

        is_stopwatch_running = true;
        stopwatch_mode = STOPWATCH_START;
        blink_enable = true;
        return;
    }
}
void stopwatchStart() {

    if(stopwatch_mode != STOPWATCH_START) return;

    drawTextMiddle(24, "( Stopwatch )", 1, SSD1306_WHITE);

    // Check for Pressing any button
    bool is_pressed_any = false;
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        is_pressed_any = true;
        button_pressed_ok = true;
    }
    if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        is_pressed_any = true;
        button_pressed_left = true;
    }
    if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        is_pressed_any = true;
        button_pressed_right = true;
    }
    if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Switch Mode if pressed any button
    if(is_pressed_any) {

        playTone(500, 30);

        is_stopwatch_running = false;
        stopwatch_mode = STOPWATCH_END;
        blink_enable = false;
        return;
    }
}
void stopwatchEnd() {

    if(stopwatch_mode != STOPWATCH_END) return;

    if(!is_blink) drawTextMiddle(24, "( Stopwatch )", 1, SSD1306_WHITE);
    else drawTextMiddle(24, "Press any button!", 1, SSD1306_WHITE);

    // Check for Pressing any button
    bool is_pressed_any = false;
    if(isButtonPressed(BUTTON_OK) && !button_pressed_ok) {

        is_pressed_any = true;
        button_pressed_ok = true;
    }
    if(!isButtonPressed(BUTTON_OK)) button_pressed_ok = false;
    if(isButtonPressed(BUTTON_LEFT) && !button_pressed_left) {

        is_pressed_any = true;
        button_pressed_left = true;
    }
    if(!isButtonPressed(BUTTON_LEFT)) button_pressed_left = false;
    if(isButtonPressed(BUTTON_RIGHT) && !button_pressed_right) {

        is_pressed_any = true;
        button_pressed_right = true;
    }
    if(!isButtonPressed(BUTTON_RIGHT)) button_pressed_right = false;

    // Press OK to EXIT
    if(is_pressed_any) {

        playTone(500, 30);

        stopwatch_seconds = 0;
        stopwatch_minutes = 0;
        stopwatch_hours = 0;
        stopwatch_mode = STOPWATCH_START;

        clock_mode = MODE_SELECT_MENU;
        return;
    }
}

void timerCount() {

    // Prevent Initialization Interrupt
    if(is_first_interrupt) {

        is_first_interrupt = false;
        return;
    }

    // Save Text
    if(save_text_timer > 0) save_text_timer--;

    // Blinking
    is_blink = !is_blink;

    // Set Colon
    time[2] = ':';  
    time[5] = ':';

    // Calculate Time
    increaseSecondNormal();
    if(is_stopwatch_running) increaseSecondStopwatch();
    if(is_timer_running) decreaseSecondTimer();
}
void increaseSecondNormal() {

    time_seconds++;
    if(time_seconds >= 60) time_minutes++;
    if(time_minutes >= 60) time_hours++;

    time_seconds %= 60;
    time_minutes %= 60;
    time_hours %= 24;

    if(clock_mode == MODE_NORMAL) setTimerString(time_seconds, time_minutes, time_hours);

    if(IS_DEBUG_MODE && clock_mode == MODE_NORMAL) {
        
        String time_string = time;
        Serial.println("- Time Now : " + time_string);
    }
}
void increaseSecondStopwatch() {

    stopwatch_seconds++;
    if(stopwatch_seconds >= 60) stopwatch_minutes++;
    if(stopwatch_minutes >= 60) stopwatch_hours++;

    stopwatch_seconds %= 60;
    stopwatch_minutes %= 60;
    stopwatch_hours %= 24;

    if(clock_mode == MODE_STOP_WATCH) setTimerString(stopwatch_seconds, stopwatch_minutes, stopwatch_hours);

    if(IS_DEBUG_MODE && clock_mode == MODE_STOP_WATCH) {
        
        String time_string = time;
        Serial.println("- Stopwatch Time Now : " + time_string);
    }
}
void decreaseSecondTimer() {
        
    if(timer_hours == 0 && timer_minutes == 0 && timer_seconds == 0) return;

    timer_seconds--;
    if(timer_seconds < 0) {
        timer_minutes--;
        timer_seconds = 59;
    }
    if(timer_minutes < 0) {
        timer_hours--;
        timer_minutes = 59;
    }

    timer_seconds %= 60;
    timer_minutes %= 60;
    timer_hours %= 24;

    if(clock_mode == MODE_TIMER) setTimerString(timer_seconds, timer_minutes, timer_hours);

    if(IS_DEBUG_MODE && clock_mode == MODE_TIMER) {
        
        String time_string = time;
        Serial.println("- Timer Time Now : " + time_string);
    }
}
void setTimerString(int seconds, int minutes, int hours) {
    
    // Set Seconds
    time[7] = (seconds % 10) + '0';
    time[6] = (seconds / 10) + '0';

    // Set Minutes
    time[4] = (minutes % 10) + '0';
    time[3] = (minutes / 10) + '0';

    // Set Hours
    time[1] = (hours % 10) + '0';
    time[0] = (hours / 10) + '0';

    // Colin Blink
    if(blink_enable) {
        
        if(is_blink) {

            time[2] = ' ';
            time[5] = ' ';
        }
        else {

            time[2] = ':';
            time[5] = ':';
        }
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
void playTone(int frequency, int delay_amount) {

    tone(BUZZER, frequency);
    delay(delay_amount);
    noTone(BUZZER);
}