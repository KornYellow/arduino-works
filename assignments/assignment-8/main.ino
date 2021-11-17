#define LED_RED 8
#define LED_BLUE 9
#define LED_YELLOW 10
#define BUTTON_RED 3
#define BUTTON_BLUE 4
#define BUTTON_YELLOW 5

#define Sleep(x) vTaskDelay(pdMS_TO_TICKS(x))

#include <Arduino_FreeRTOS.h>
#include <queue.h>

QueueHandle_t red_queue;
QueueHandle_t blue_queue;
QueueHandle_t yellow_queue;

void setup() {

    Serial.begin(9600);

    red_queue = xQueueCreate(10, sizeof(bool));
    blue_queue = xQueueCreate(10, sizeof(bool));
    yellow_queue = xQueueCreate(10, sizeof(bool));
 
    xTaskCreate(taskSender, "Sender RED Task", 100, (void*)BUTTON_RED, 1, NULL);
    xTaskCreate(taskSender, "Sender BLUE Task", 100, (void*)BUTTON_BLUE, 1, NULL);
    xTaskCreate(taskSender, "Sender YELLOW Task", 100, (void*)BUTTON_YELLOW, 1, NULL);

    xTaskCreate(taskReceiver, "Receiver RED Task", 100, (void*)LED_RED, 1, NULL);
    xTaskCreate(taskReceiver, "Receiver BLUE Task", 100, (void*)LED_BLUE, 1, NULL);
    xTaskCreate(taskReceiver, "Receiver YELLOW Task", 100, (void*)LED_YELLOW, 1, NULL);
}

void loop() {

}

void taskReceiver(void* param) {

    unsigned short led = (unsigned short)param;
    
    pinMode(led, OUTPUT);

    unsigned short red_counter = 0;
    
    bool is_blue_blink = false;
    bool blue_blink = true;

    unsigned short yellow_counter = 0;
    bool yellow_blink = true;
    bool yellow_pressed = false;
    
    while(true) {

        // Subtracting Value
        if(red_counter > 0 && led == LED_RED) red_counter --;

        // Receive Value from Queue
        bool is_received = false;
        if(led == LED_RED) is_received = xQueueReceive(red_queue, NULL, 0);
        else if(led == LED_BLUE) is_received = xQueueReceive(blue_queue, NULL, 0);
        else if(led == LED_YELLOW) is_received = xQueueReceive(yellow_queue, NULL, 0);
        
        // If received Queue
        if(is_received && led == LED_RED) red_counter += 6;
        if(is_received && led == LED_BLUE) is_blue_blink = !is_blue_blink;
        if(is_received && led == LED_YELLOW) {
            if(!yellow_pressed) yellow_counter = 6;
            yellow_pressed = true;
        }

        // Red Action
        if(red_counter > 0 && led == LED_RED) digitalWrite(LED_RED, HIGH);
        else if(!red_counter && led == LED_RED) digitalWrite(LED_RED, LOW); 

        // Blue Action
        if(is_blue_blink && led == LED_BLUE) {

            digitalWrite(LED_BLUE, blue_blink);
            blue_blink = !blue_blink;
        }
        else if(!is_blue_blink && led == LED_BLUE) digitalWrite(LED_BLUE, LOW);

        // Yellow Action
        if(yellow_counter > 0 && led == LED_YELLOW) {

            yellow_counter --;
            digitalWrite(LED_YELLOW, yellow_blink);
            yellow_blink = !yellow_blink;
        }
        else if(!yellow_counter && led == LED_YELLOW) {
        
            yellow_pressed = false;
            digitalWrite(LED_YELLOW, LOW);
        }

        Sleep(500);
    }
}

void taskSender(void* param) {

    unsigned short button = (unsigned short)param;
    int is_button_pressed = false;

    pinMode(button, INPUT_PULLUP);

    while(true) {
        
        // Get Button Input
        if(isButtonPressed(button) && !is_button_pressed){
            
            Serial.println("Button Pressed " + String(button));
            is_button_pressed = true;

            // Add to Queue
            if(button == BUTTON_RED) xQueueSend(red_queue, &button, 0);
            else if(button == BUTTON_BLUE) xQueueSend(blue_queue, &button, 0);
            else if(button == BUTTON_YELLOW) xQueueSend(yellow_queue, &button, 0);
        }
        if(!isButtonPressed(button)) is_button_pressed = false;

        Sleep(50);
    }
}

bool isButtonPressed(int pin) {
    
    Sleep(10);
    if(digitalRead(pin) == LOW) return true;
    return false;
}