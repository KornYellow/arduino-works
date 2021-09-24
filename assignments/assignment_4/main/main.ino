/**
 *  Assignment #4 "Pong on Arduino using LED Matrix"
 *  @author  (64010009) Korn Rojrattanapanya
 *  @version 1.0.0
**/

/*\
 * =============================================
 *   INCLUDE LIBRARIES
 * =============================================
\*/

#include <LedControl.h>

/*\
 * =============================================
 *   LED CONTROL
 * =============================================
\*/

const int PIN_CS  = 10;
const int PIN_CLK = 13;
const int PIN_DIN = 11;
const int MODULES =  4;

LedControl led_control = LedControl(PIN_DIN, PIN_CLK, PIN_CS, MODULES);

/**
 *  Setup LED Matrix 
**/
void ledMatrixSetup() {
    
    int device_count = led_control.getDeviceCount();
    for(int address = 0; address < device_count; address++) {
        led_control.shutdown(address, false);
        led_control.setIntensity(address, 1);
        led_control.clearDisplay(address);
    }
}

/*\
 * =============================================
 *   BUTTONS
 * =============================================
\*/

const int BUTTON_PLAYER_1_A = 5;
const int BUTTON_PLAYER_1_B = 4;
const int BUTTON_PLAYER_2_A = 3;
const int BUTTON_PLAYER_2_B = 2;

bool is_button_pressed[4] = {false, false, false, false};

/**
 *  Setup Buttons
**/
void buttonSetup() {

    pinMode(BUTTON_PLAYER_1_A, INPUT_PULLUP);
    pinMode(BUTTON_PLAYER_1_B, INPUT_PULLUP);
    pinMode(BUTTON_PLAYER_2_A, INPUT_PULLUP);
    pinMode(BUTTON_PLAYER_2_B, INPUT_PULLUP);
}

/**
 *  Get state of the button (true, false).
 *  @param pin Digital pin on Arduino board.
 *  @return Button state (true, false).
**/
bool isButtonPressed(int pin) {
    
    delay(10);
    if(digitalRead(pin) == LOW) return true;
    return false;
}

/*\
 * =============================================
 *   SPEAKER
 * =============================================
\*/

const int SPEAKER = 12;

/**
 *  Setup Speaker
**/
void speakerSetup() {

    pinMode(SPEAKER, OUTPUT);
}

/**
 *  Play tone with delay
 *  @param frequency Frequency to play.
 *  @param delay_amount Amount of delay.
**/
void playTone(int frequency, int delay_amount) {

    tone(SPEAKER, frequency);
    delay(delay_amount);
    noTone(SPEAKER);
}

/*\
 * =============================================
 *   OTHER FUNCTIONS
 * =============================================
\*/

/**
 *  Turn off all LED on the matrix.
**/
void resetDisplay() {

    for(int n = 0; n < 4; n++) {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                led_control.setLed(n, i, j, false);
            }
        }
    }
}

/**
 *  Turn on LED on (x, y) location.
 *  @param x x location on the matrix.
 *  @param y y location on the matrix.
**/ 
void drawPoint(int x, int y) {
    
    for(int n = 0; n < 4; n++) {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(i == x && j + (8 * n) == y) led_control.setLed((3 - n), 7 - i, j, true);
            }
        }
    }
}

/**
 *  Turn off LED on (x, y) location.
 *  @param x x location on the matrix.
 *  @param y y location on the matrix.
**/ 
void deletePoint(int x, int y) {
    
    for(int n = 0; n < 4; n++) {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(i == x && j + (8 * n) == y) led_control.setLed((3 - n), 7 - i, j, false);
            }
        }
    }
}

int NUMBER_STRUCT[16] = {1, 2, 8, 11, 16, 19, 24, 25, 26, 27, 32, 35, 40, 43, 49, 50};
int NUMBER_PATTERN[10][16] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
    {0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
};

/**
 *  Clear 7-segment number in LED Matrix.
 *  @param x X position in LED Matrix.
 *  @param y Y position in LED Matrix.
**/
void clearNumber(int x, int y) {
    for(int i = 0; i < 16; i++) {
        int offset_y = NUMBER_STRUCT[i] / 8;
        int offset_x = NUMBER_STRUCT[i] - (offset_y * 8);
        deletePoint(x + offset_x, y + offset_y);
    }
}

/**
 *  Show 7-segment number in LED Matrix.
 *  @param x X position in LED Matrix.
 *  @param y Y position in LED Matrix.
 *  @param number Number to show 0 - 9.
**/
void showNumber(int x, int y, int number) {
    for(int i = 0; i < 16; i++) {
        int offset_y = NUMBER_STRUCT[i] / 8;
        int offset_x = NUMBER_STRUCT[i] - (offset_y * 8);
        if(NUMBER_PATTERN[number][i]) {
            drawPoint(x + offset_x, y + offset_y); 
        } else deletePoint(x + offset_x, y + offset_y);
    }
}

/**
 *  Blink winner score.
 *  @param score_1 Player 1 score.
 *  @param score_2 Player 2 score.
 *  @param winner Player to blink (1, 2).
**/ 
void blinkScore(int score_1, int score_2, int winner) {

    int player_1_x = 2, player_1_y = 7;
    showNumber(player_1_x, player_1_y, score_1);

    int player_2_x = 2, player_2_y = 18;
    showNumber(player_2_x, player_2_y, score_2);

    delay(800);

    if(winner == 1) {
        score_1++;
        clearNumber(player_1_x, player_1_y);
        delay(400);
        showNumber(player_1_x, player_1_y, score_1);
        playTone(659, 100);
        playTone(784, 100);
        playTone(1319, 100);
        playTone(1047, 100);
        clearNumber(player_1_x, player_1_y);
        playTone(1175, 100);
        playTone(1568, 100);
        delay(200);
        showNumber(player_1_x, player_1_y, score_1);
        delay(800);
    }
    else {
        score_2++;
        clearNumber(player_2_x, player_2_y);
        delay(400);
        showNumber(player_2_x, player_2_y, score_2);
        playTone(659, 100);
        playTone(784, 100);
        playTone(1319, 100);
        playTone(1047, 100);
        clearNumber(player_2_x, player_2_y);
        playTone(1175, 100);
        playTone(1568, 100);
        delay(200);
        showNumber(player_2_x, player_2_y, score_2);
        delay(800);
    }   

    clearNumber(player_1_x, player_1_y);
    clearNumber(player_2_x, player_2_y);
    delay(600);
}

/*\
 * =============================================
 *   GAME CLASSES
 * =============================================
\*/

// Game Size
const int GAME_WIDTH = 8;
const int GAME_HEIGHT = 32;

class Paddle {
public: 
    int x;
    int y;
    int direction;
    int size;

    /**
     *  Constructor of the Paddle Object.
     *  @param x Initial X location on LED matrix.
     *  @param y Initial Y location on LED matrix.
    **/ 
    Paddle(int x, int y) {
        this->x = x;
        this->y = y;
        this->direction = 0;
        this->size = 4;
    }
    
    /**
     *  Set Paddle direction on the X axis.
     *  @param direction Paddle direction.
    **/ 
    void setDirection(int direction) {
        this->direction = direction;
    }

    /**
     *  Render Paddle.
    **/ 
    void draw() {
        if(this->direction != 0) {
            for(int i = 0; i < this->size; i++) {
                deletePoint(this->x + i, this->y);
            }
            this->x += this->direction;
            if(this->x > 8 - this->size) this->x = 8 - this->size;
            if(this->x < 0) this->x = 0;
        }

        for(int i = 0; i < this->size; i++) {
            drawPoint(this->x + i, this->y);
        }
    }
};

class Puck {
public: 
    float x;
    float y;
    float x_previous;
    float y_previous;
    float x_speed;
    float y_speed;

    /**
     *  Constructor of the Puck Object.
     *  @param x Initial X location on LED matrix.
     *  @param y Initial Y location on LED matrix.
    **/ 
    Puck(float x, float y) {
        this->x = x;
        this->y = y;
        this->x_previous = x;
        this->y_previous = y;
        this->x_speed = 1;
        this->y_speed = -1;
    }

    /**
     *  Update Puck.
    **/ 
    void update() {
        if(this->x + this->x_speed >= GAME_WIDTH || this->x + this->x_speed < 0) {
            this->x_speed *= -1;
            playTone(123, 15);
            playTone(62, 15);
        }
        
        this->x += this->x_speed;
        this->y += this->y_speed;
    }

    /**
     *  Check if Puck got out of Game.
     *  @return The Winner (0 : Game not end yet, 1 : Player 1 Wins, 2 : Player 2 Wins).
    **/ 
    int getWinner() {
        if(this->y + this->y_speed >= GAME_HEIGHT) return 1;
        if(this->y + this->y_speed < 0) return 2;
        return 0;
    }

    /**
     *  Check if Puck hits Paddle.
     *  @param paddle Paddle to check.
     *  @param direction Direction to check from.
     *  @return true or false
    **/ 
    bool collisionCheck(Paddle paddle, int direction) {
        if(direction < 0) {
            if(this->y < paddle.y + 2 && this->y > paddle.y && this->x >= paddle.x - 1 && this->x < paddle.x + paddle.size + 0.5) {
                this->y_speed *= -1;
                return true;
            }
        }
        else if(direction > 0) {
            if(this->y > paddle.y - 2 && this->y < paddle.y && this->x >= paddle.x - 1 && this->x < paddle.x + paddle.size + 0.5) {
                this->y_speed *= -1;
                return true;
            }
        }
        return false;
    }

    /**
     *  Blink Puck out of game.
    **/
    void blink() {
        drawPoint(this->x, this->y);
        playTone(196, 100);
        playTone(147, 100);
        deletePoint(this->x, this->y);
        playTone(98, 100);
        delay(100);
        drawPoint(this->x, this->y);
        delay(200);
        deletePoint(this->x, this->y);
        delay(200);
        drawPoint(this->x, this->y);
        delay(200);
        deletePoint(this->x, this->y);
        delay(800);
    }

    /**
     *  Reset Puck.
    **/
    void reset() {
        this->x = GAME_WIDTH/2;
        this->y = GAME_HEIGHT/2;
        draw();
        playTone(494, 100);
        playTone(659, 100);

        deletePoint(this->x, this->y);
        delay(200);
        drawPoint(this->x, this->y);
        delay(200);
        deletePoint(this->x, this->y);
        delay(200);
        drawPoint(this->x, this->y);
        delay(400);
    }

    /**
     *  Render Puck.
    **/ 
    void draw() {
        if(round(this->x) != round(this->x_previous)
        || round(this->y) != round(this->y_previous)) {
            deletePoint(this->x_previous, this->y_previous);
            this->x_previous = this->x;
            this->y_previous = this->y;
            drawPoint(this->x, this->y);
        }
    }
};

/*\
 * =============================================
 *   GAME FUNCTIONS
 * =============================================
\*/

// Game States
enum GameState {
    GAME_WAITING,
    GAME_RUNNING,
    GAME_ENDING,
};
int game_state = GAME_RUNNING;

// Initialize Player's Paddles
Paddle player_1(2, 2);
Paddle player_2(2, 29);
int player_1_score = 0;
int player_2_score = 0;

// Initialize Puck
Puck puck(GAME_WIDTH/2, GAME_HEIGHT/2);

void gameSetup() {
    
}
void gameWaiting() {
    
    if(game_state != GAME_WAITING) return;
}
void gameRunning() {
    
    if(game_state != GAME_RUNNING) return;

    // Get Winner
    if(puck.getWinner() == 1) { // Player 1 Wins
        puck.blink();
        blinkScore(player_1_score, player_2_score, 1);
        player_1_score++;
        puck.reset();
    }
    else if(puck.getWinner() == 2) { // Player 2 Wins
        puck.blink();
        blinkScore(player_1_score, player_2_score, 2);
        player_2_score++;
        puck.reset();
    }

    // Player 1 | Paddle Move
    player_1.setDirection(0);
    if(isButtonPressed(BUTTON_PLAYER_1_A)) {
        player_1.setDirection(-1);
    }
    if(isButtonPressed(BUTTON_PLAYER_1_B)) {
        player_1.setDirection(1);
    }

    // Player 2 | Paddle Move
    player_2.setDirection(0);
    if(isButtonPressed(BUTTON_PLAYER_2_A)) {
        player_2.setDirection(-1);
    }
    if(isButtonPressed(BUTTON_PLAYER_2_B)) {
        player_2.setDirection(1);
    }
    
    // Puck Update
    if(puck.collisionCheck(player_1, -1)
    || puck.collisionCheck(player_2, 1)) {
        playTone(1568, 20);
        playTone(784, 20);
    }
    puck.update();

    // Draw Player's Paddles
    player_1.draw();
    player_2.draw();

    // Draw Puck
    puck.draw();
}
void gameEnding() {
    
    if(game_state != GAME_ENDING) return;
}

/*\
 * =============================================
 *   MAIN FUNCTIONS
 * =============================================
\*/

void setup() {

    // Serial Monitor Setup
    Serial.begin(115200);
    while(!Serial);
    Serial.println();
    Serial.println("Serial is ready!");
    
    // Others Setup
    ledMatrixSetup();
    speakerSetup();
    buttonSetup();
    gameSetup();
}

void loop() {

    // Game Loop
    gameWaiting();
    gameRunning();
    gameEnding();
}
