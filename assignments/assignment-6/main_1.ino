#include <Wire.h>

#define SLAVE_1 1
#define SLAVE_2 2

int progress = 1;
int next_progress = 1;
int board_to_send = 1;

char data_to_send[10][7] = {
    "HELLO0",
    "HELLO1",
    "HELLO2",
    "HELLO3",
    "HELLO4",
    "HELLO5",
    "HELLO6",
    "HELLO7",
    "HELLO8",
    "HELLO9"
};

void setup() {

    Wire.begin();

    Serial.begin(9600);
    while(!Serial);
    Serial.println();
    Serial.println("Master - Ready.");
}

void loop() {

    if(progress == next_progress) {

        board_to_send = !board_to_send;
        if(board_to_send == 0) Wire.beginTransmission(SLAVE_1);
        if(board_to_send == 1) Wire.beginTransmission(SLAVE_2);
        
        Wire.write(data_to_send[progress]);
        Wire.endTransmission();

        Serial.println(data_to_send[progress]);

        next_progress++;
        if(next_progress > 9) next_progress = 1;
    }
    else {

        if(board_to_send == 0) Wire.requestFrom(SLAVE_1, 6);
        if(board_to_send == 1) Wire.requestFrom(SLAVE_2, 6);

        while(Wire.available()) {

            String receivedData = "";
            for(int i = 0; i < 6; i++) {
                receivedData += (char)Wire.read();
            }
            progress = receivedData[5] - '0';
        }
    }

    delay(1000);
}