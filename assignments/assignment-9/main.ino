#define LED_RED 8
#define LED_BLUE 9
#define LED_YELLOW 10
#define BUTTON_RED 3
#define BUTTON_BLUE 4
#define BUTTON_YELLOW 5

#define Sleep(x) vTaskDelay(pdMS_TO_TICKS(x))

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t blueAction;
SemaphoreHandle_t yellowAction;

void setup() {

    blueAction = xSemaphoreCreateMutex();
    yellowAction = xSemaphoreCreateMutex();
    xSemaphoreGive(blueAction);
    xSemaphoreGive(yellowAction);

    xTaskCreate(ledRedAction, "RED Task", 100, NULL, 1, NULL);
    xTaskCreate(ledBlueAction, "BLUE Task", 100, NULL, 1, NULL);
    xTaskCreate(ledYellowAction, "YELLOW Task", 100, NULL, 1, NULL);
}

void loop() {

}

void ledRedAction(void* param) {

    bool is_button_pressed = false;
    short timer = 0;

    pinMode(BUTTON_RED, INPUT_PULLUP);
    pinMode(LED_RED, OUTPUT);

    while(true) {
        
        if(isButtonPressed(BUTTON_RED) && !is_button_pressed){
            
            is_button_pressed = true;
            
            if(timer <= 0) {
                
                timer = 30;
                xSemaphoreTake(blueAction, 0);
                xSemaphoreTake(yellowAction, 0);
            }
            else if(timer > 0) {

                timer = 0;
                xSemaphoreGive(blueAction);
                xSemaphoreGive(yellowAction);
            }
        }
        if(!isButtonPressed(BUTTON_RED)) is_button_pressed = false;

        if(timer > 0) {

            timer--;
            xSemaphoreTake(blueAction, 0);
            xSemaphoreTake(yellowAction, 0);
            digitalWrite(LED_RED, HIGH);
        }
        else if(timer == 0) {

            timer--;
            digitalWrite(LED_RED, LOW);
            xSemaphoreGive(blueAction);
            xSemaphoreGive(yellowAction);
        }

        Sleep(100);
    }
}
void ledBlueAction(void* param) {

    bool is_button_pressed = false;
    short timer = 0;

    pinMode(BUTTON_BLUE, INPUT_PULLUP);
    pinMode(LED_BLUE, OUTPUT);

    while(true) {
        
        if(isButtonPressed(BUTTON_BLUE) && !is_button_pressed){
            
            is_button_pressed = true;
            if(timer <= 0 && xSemaphoreTake(blueAction, 0)) {

                timer = 30;
                xSemaphoreGive(blueAction);
                xSemaphoreTake(yellowAction, 0);
            }
            else if(timer > 0 && xSemaphoreTake(blueAction, 0)) {

                timer = 0;
                xSemaphoreGive(blueAction);
                xSemaphoreGive(yellowAction);
            }
        }
        if(!isButtonPressed(BUTTON_BLUE)) is_button_pressed = false;

        if(timer > 0) {

            timer--;
            xSemaphoreTake(yellowAction, 0);
            digitalWrite(LED_BLUE, HIGH);
        }
        else if(timer == 0) {

            timer--;
            digitalWrite(LED_BLUE, LOW);
            xSemaphoreGive(yellowAction);
        }

        Sleep(100);
    }
}
void ledYellowAction(void* param) {

    bool is_button_pressed = false;
    short timer = 0;

    pinMode(BUTTON_YELLOW, INPUT_PULLUP);
    pinMode(LED_YELLOW, OUTPUT);

    bool is_blink = true;

    while(true) {
        
        if(isButtonPressed(BUTTON_YELLOW) && !is_button_pressed){
            
            is_button_pressed = true;
            if(timer <= 0 && xSemaphoreTake(yellowAction, 0)) {

                timer = 20;
            }
            else if(timer > 0 && xSemaphoreTake(yellowAction, 0)) {

                timer = 0;
                xSemaphoreGive(yellowAction);
            }
        }
        if(!isButtonPressed(BUTTON_YELLOW)) is_button_pressed = false;

        if(timer > 0) {

            if(timer % 5 == 0) is_blink = !is_blink;    
            timer--;
            
            if(is_blink) digitalWrite(LED_YELLOW, HIGH);
            else digitalWrite(LED_YELLOW, LOW);
        }
        else if(timer == 0) {

            timer--;
            xSemaphoreGive(yellowAction);
            digitalWrite(LED_YELLOW, LOW);
        }

        Sleep(100);
    }
}
bool isButtonPressed(int pin) {
    
    Sleep(10);
    if(digitalRead(pin) == LOW) return true;
    return false;
}