const int LED_RED       = 2;
const int LED_YELLOW    = 3;
const int LED_GREEN     = 4;
const int BUTTON_LEFT   = 5;
const int BUTTON_CENTER = 6;
const int BUTTON_RIGHT  = 7;
#define PRESSED LOW

long time_now;

//LED STATE
bool is_red_active    = false;
bool is_yellow_active = false;
bool is_green_active  = false;

int red_timer    = 0;
int yellow_timer = 0;
int green_timer  = 0;

bool is_button_left_pushed   = false;
bool is_button_center_pushed = false;
bool is_button_right_pushed  = false;

long debounce_delay = 50;

void setup() {

    pinMode(LED_RED       , OUTPUT);
    pinMode(LED_YELLOW    , OUTPUT);
    pinMode(LED_GREEN     , OUTPUT);

    pinMode(BUTTON_RIGHT  , INPUT_PULLUP);
    pinMode(BUTTON_CENTER , INPUT_PULLUP);
    pinMode(BUTTON_LEFT   , INPUT_PULLUP);

    Serial.begin(115200);
    while(!Serial);
    Serial.println();
    Serial.println("Serial is ready!");
}

void loop() {

    /* ------------------- */
    /* ----- RED LED ----- */
    if(checkButton(BUTTON_LEFT)) {
            
        //Force turn off LED
        if(is_red_active && !is_button_left_pushed) {

            digitalWrite(LED_RED, LOW);

            is_red_active = false;
            red_timer = 0;

            Serial.println((String)millis() + " : [ ] [LED RED] has been forced to turn off.");
        }

        //Turn on LED
        else if(!is_red_active && red_timer == 0 && !is_button_left_pushed) {
            
            digitalWrite(LED_RED, HIGH);

            is_red_active = true;
            red_timer = millis();

            Serial.println((String)millis() + " : [X] [LED RED] has been turned on for 3 seconds.");
        }

        is_button_left_pushed = true;
    }
    else {

        is_button_left_pushed = false;
    }

    //Turn off LED
    if(isTimePassed(red_timer, 3000) && is_red_active) {
        
        digitalWrite(LED_RED, LOW);

        is_red_active = false;
        red_timer = 0;

        Serial.println((String)millis() + " : [ ] [LED RED] has been turned off.");
    }
    /* ----- RED LED ----- */
    /* ------------------- */

    /* ------------------- */
    /* ----- GREEN LED ----- */
    if(checkButton(BUTTON_RIGHT)) {
            
        //Force turn off LED
        if(is_green_active && !is_button_right_pushed) {

            digitalWrite(LED_GREEN, LOW);

            is_green_active = false;
            green_timer = 0;

            Serial.println((String)millis() + " : [ ] [LED GREEN] has been forced to turn off.");
        }

        //Turn on LED
        else if(!is_green_active && green_timer == 0 && !is_button_right_pushed && !is_red_active) {
            
            digitalWrite(LED_GREEN, HIGH);

            is_green_active = true;
            green_timer = millis();

            Serial.println((String)millis() + " : [X] [LED GREEN] has been turned on for 3 seconds.");
        }

        is_button_right_pushed = true;
    }
    else {

        is_button_right_pushed = false;
    }

    //Turn off LED
    if(isTimePassed(green_timer, 3000) && is_green_active) {
        
        digitalWrite(LED_GREEN, LOW);

        is_green_active = false;
        green_timer = 0;

        Serial.println((String)millis() + " : [ ] [LED GREEN] has been turned off.");
    }
    /* ----- GREEN LED ----- */
    /* ------------------- */

    /* ------------------- */
    /* ----- YELLOW LED ----- */
    if(checkButton(BUTTON_CENTER)) {

        //Change LED state
        if(!is_yellow_active && yellow_timer == 0 && !is_button_center_pushed && !is_red_active && !is_green_active) {

            digitalWrite(LED_YELLOW, HIGH);

            is_yellow_active = true;
            yellow_timer = millis();

            Serial.println((String)millis() + " : [X] [LED YELLOW] will be flashing for 2 seconds.");
        }

        is_button_center_pushed = true;
    }
    else {

        is_button_center_pushed = false;
    }
    
    //Flashing LED
    if(is_yellow_active) {
        if(isTimePassed(yellow_timer, 500)) digitalWrite(LED_YELLOW, LOW);
        if(isTimePassed(yellow_timer, 1000)) digitalWrite(LED_YELLOW, HIGH);
        if(isTimePassed(yellow_timer, 1500)) digitalWrite(LED_YELLOW, LOW);
    }

    //Stop flashing LED
    if(isTimePassed(yellow_timer, 2000) && is_yellow_active) {

        digitalWrite(LED_YELLOW, LOW);

        is_yellow_active = false;
        yellow_timer = 0;

        Serial.println((String)millis() + " : [ ] [LED YELLOW] has stopped flashing.");
    }
    /* ----- YELLOW LED ----- */
    /* ------------------- */
}

bool checkButton(int pin) {

    delay(10);
    if(digitalRead(pin) == LOW) return true;
    return false;
}

bool isTimePassed(long last_run, int delay){
    return (millis() >= last_run + delay);
}