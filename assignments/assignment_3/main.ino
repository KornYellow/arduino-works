/*---- PIN VARIABLES
------------------------------------*/
const int SEGMENT_PIN_COUNT = 8;
const int SEGMENT_PINS[SEGMENT_PIN_COUNT] = {2, 3, 4, 5, 6, 7, 8, 11};

const int BUTTON_A = 10;
const int BUTTON_B = 9;

const byte BIT_PATTERN_NUMBER[10] = {
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110  // 9
};

const byte BIT_PATTERN_CLEAR = 0b00000000;

/*---- GAME VARIABLES
------------------------------------*/
const int GAME_WAITING = 0;
const int GAME_PLAYING = 1;
const int GAME_ENDING  = 2;

int game_state = GAME_WAITING;

int selected_number = 1;
int guess_number;

unsigned long timer_waiting = 0;

bool is_button_a_pressed = false;
bool is_button_b_pressed = false;

/*---- OTHER FUNCTIONS
------------------------------------*/
void sevenSegmentPinSetup() {

    for(int i = 0; i < SEGMENT_PIN_COUNT; i++) pinMode(SEGMENT_PINS[i], OUTPUT);
}
bool isButtonPressed(int pin) {
    
    delay(10);
    return digitalRead(pin) == LOW;
}
void setSegmentPattern(byte bit_pattern) {

    int is_bit_on;
    for(int i = 0; i < SEGMENT_PIN_COUNT; i++) {
        
        is_bit_on = bitRead(bit_pattern, i);
        digitalWrite(SEGMENT_PINS[7 - i], !is_bit_on);
    }
}
bool isTimePassed(unsigned long time_init, int delay) {

    return (millis() >= time_init + delay);
}

/*---- GAME FUNCTIONS
------------------------------------*/
void gameSetup() {

    //Clear
    setSegmentPattern(BIT_PATTERN_CLEAR);
    
    //Set Random Number
    randomSeed(analogRead(A0));
    guess_number = random(1, 9);
}
void gameWaiting() {

    if(game_state != GAME_WAITING) return; 

    //Waiting Light Show
         if(isTimePassed(timer_waiting, 1600)) timer_waiting = millis();
    else if(isTimePassed(timer_waiting, 1400)) setSegmentPattern(0b11000100);
    else if(isTimePassed(timer_waiting, 1200)) setSegmentPattern(0b10000110);
    else if(isTimePassed(timer_waiting, 1000)) setSegmentPattern(0b00100110); 
    else if(isTimePassed(timer_waiting, 800 )) setSegmentPattern(0b00110010); 
    else if(isTimePassed(timer_waiting, 600 )) setSegmentPattern(0b00111000);
    else if(isTimePassed(timer_waiting, 400 )) setSegmentPattern(0b00011010);
    else if(isTimePassed(timer_waiting, 200 )) setSegmentPattern(0b01001010);
    else if(isTimePassed(timer_waiting, 0   )) setSegmentPattern(0b11000010);

    //Button for starting game
    if(isButtonPressed(BUTTON_A)) {

        //Display Transition light show
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(100);
        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(100);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(100);
        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(100);

        //Set state to selecting numbers
        game_state = GAME_PLAYING;
    }
}
void gamePlaying() {

    if(game_state != GAME_PLAYING) return;

    //Select Number
    if(isButtonPressed(BUTTON_A)) {

        if(!is_button_a_pressed) {
            
            is_button_a_pressed = true;

            selected_number ++;
            if(selected_number > 9) selected_number = 1;
        }
    }
    else {

        is_button_a_pressed = false;
        
        delay(10);
    }

    //Show Number
    setSegmentPattern(BIT_PATTERN_NUMBER[selected_number]);

    //Confirm Number
    if(isButtonPressed(BUTTON_B)) {
        
        //Display Transition light show
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[selected_number]);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[selected_number]);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[selected_number]);
        delay(1000);

        //Set state to game ending
        game_state = GAME_ENDING;
    }
}
void gameEnding() {

    if(game_state != GAME_ENDING) return;

    //Select Number = Guess
    if(selected_number == guess_number) {

        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(BIT_PATTERN_NUMBER[0]);
        delay(1000);
    }
    
    //Select Number > Guess
    else if(selected_number > guess_number) {
        
        setSegmentPattern(0b10111100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b10111100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b10111100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b10111100);
        delay(1000);
    }
    
    //Select Number < Guess
    else if(selected_number < guess_number) {

        setSegmentPattern(0b00011100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b00011100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b00011100);
        delay(500);
        setSegmentPattern(BIT_PATTERN_CLEAR);
        delay(200);
        setSegmentPattern(0b00011100);
        delay(1000);
    }

    //Return to Waiting state
    game_state = GAME_WAITING;

    //Reset guess and select number
    guess_number = random(1, 9);
    selected_number = 1;
}

/*---- GAME SETUP
------------------------------------*/
void setup() {

    //Setup pinMode
    sevenSegmentPinSetup();
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);

    //Setup Serial Monitor
    Serial.begin(115200);
    while(!Serial);
    Serial.println();
    Serial.println("Serial Ready!");

    //Setup Game
    gameSetup();
}

/*---- GAME LOOP
------------------------------------*/
void loop() {

    gameWaiting();
    gamePlaying();
    gameEnding();
}