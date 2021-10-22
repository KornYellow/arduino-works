const int LED_TOP_RED = 4;
const int LED_TOP_YELLOW = 5;
const int LED_TOP_GREEN = 6;
const int LED_RIGHT_RED = 8;
const int LED_RIGHT_YELLOW = 9;
const int LED_RIGHT_GREEN = 10;
const int LED_WALK_RED = 11;
const int LED_WALK_GREEN = 12;

const int BUTTON_TOP = 2;
const int BUTTON_RIGHT = 3;
const int BUTTON_WALK = 7;

int led_out[8] = {
    LED_TOP_RED,
    LED_TOP_YELLOW,
    LED_TOP_GREEN,
    LED_RIGHT_RED,
    LED_RIGHT_YELLOW,
    LED_RIGHT_GREEN,
    LED_WALK_RED,
    LED_WALK_GREEN,
};

//                  RYGRYGRG
#define TOP_GO     B00110010
#define TOP_WAIT   B01010010
#define RIGHT_GO   B10000110
#define RIGHT_WAIT B10001010
#define WALK_GO    B10010001
#define WALK_WAIT  B10010000

struct State {

    unsigned int output;
    unsigned int time;
    unsigned int next[8];
};

State finite_state[] = {
    // Input(WALK,RIGHT,TOP) 000 001 010 011 100 101 110 111
    {TOP_GO      , 3000  , {  0,  0,  1,  1,  2,  2,  2,  1}},  // State : 0  เปิดไฟเขียวให้รถทางบน
    {TOP_WAIT    , 500   , {  3,  3,  3,  3,  3,  3,  3,  3}},  // State : 1  เปลี่ยนไฟเหลือง รอไปเปิดไฟเขียวให้ทางขวา
    {TOP_WAIT    , 500   , {  6,  6,  6,  6,  6,  6,  6,  6}},  // State : 2  เปลี่ยนไฟเหลือง รอไปเปิดไฟเขียวให้คนเดิน

    {RIGHT_GO    , 3000  , {  3,  4,  3,  4,  5,  5,  5,  5}},  // State : 3  เปิดไฟเขียวให้รถทางขวา
    {RIGHT_WAIT  , 500   , {  0,  0,  0,  0,  0,  0,  0,  0}},  // State : 4  เปลี่ยนไฟเหลือง รอไปเปิดไฟเขียวให้ทางบน
    {RIGHT_WAIT  , 500   , {  6,  6,  6,  6,  6,  6,  6,  6}},  // State : 5  เปลี่ยนไฟเหลือง รอไปเปิดไฟเขียวให้คนเดิน

    {WALK_GO     , 3000  , {  6,  7, 14,  7,  6,  7, 14,  7}},  // State : 6  เปิดไฟเขียวให้คนเดิน

    {WALK_WAIT   , 500   , {  8,  8,  8,  8,  8,  8,  8,  8}},  // State : 7  กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_GO     , 500   , {  9,  9,  9,  9,  9,  9,  9,  9}},  // State : 8  กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_WAIT   , 500   , { 10, 10, 10, 10, 10, 10, 10, 10}},  // State : 9  กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_GO     , 500   , { 11, 11, 11, 11, 11, 11, 11, 11}},  // State : 10 กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_WAIT   , 500   , { 12, 12, 12, 12, 12, 12, 12, 12}},  // State : 11 กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_GO     , 500   , { 13, 13, 13, 13, 13, 13, 13, 13}},  // State : 12 กระพริบไฟ รอเปิดไฟเขียวให้ทางบน
    {WALK_WAIT   , 500   , {  0,  0,  0,  0,  0,  0,  0,  0}},  // State : 13 กระพริบไฟ รอเปิดไฟเขียวให้ทางบน

    {WALK_WAIT   , 500   , { 15, 15, 15, 15, 15, 15, 15, 15}},  // State : 14 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_GO     , 500   , { 16, 16, 16, 16, 16, 16, 16, 16}},  // State : 15 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_WAIT   , 500   , { 17, 17, 17, 17, 17, 17, 17, 17}},  // State : 16 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_GO     , 500   , { 18, 18, 18, 18, 18, 18, 18, 18}},  // State : 17 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_WAIT   , 500   , { 19, 19, 19, 19, 19, 19, 19, 19}},  // State : 18 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_GO     , 500   , { 20, 20, 20, 20, 20, 20, 20, 20}},  // State : 19 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
    {WALK_WAIT   , 500   , {  3,  3,  3,  3,  3,  3,  3,  3}},  // State : 20 กระพริบไฟ รอเปิดไฟเขียวให้ทางขวา
};

void output(int output) {

    for(int i = 0; i < 8; i++) {

        digitalWrite(led_out[i], bitRead(output, (7 - i)));
    }
}

void setup() {

    Serial.begin(115200);
    while(!Serial);
    Serial.println();
    Serial.println("Serial is ready!");
  
    pinMode(BUTTON_TOP, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(BUTTON_WALK, INPUT_PULLUP);

    pinMode(LED_TOP_RED, OUTPUT);
    pinMode(LED_TOP_YELLOW, OUTPUT);
    pinMode(LED_TOP_GREEN, OUTPUT);
    pinMode(LED_RIGHT_RED, OUTPUT);
    pinMode(LED_RIGHT_YELLOW, OUTPUT);
    pinMode(LED_RIGHT_GREEN, OUTPUT);
    pinMode(LED_WALK_RED, OUTPUT);
    pinMode(LED_WALK_GREEN, OUTPUT);
}

int state = 0;

void loop() {

    output(finite_state[state].output);
    delay(finite_state[state].time);

    int input_top   = digitalRead(BUTTON_TOP);
    int input_right = digitalRead(BUTTON_RIGHT);
    int input_walk  = digitalRead(BUTTON_WALK);

    Serial.println("TOP : " + String(input_top) + " | RIGHT : " + String(input_right) + " | WALK : " + String(input_walk));
    
    int input = input_walk * 4 + input_right * 2 + input_top;

    state = finite_state[state].next[input];
}