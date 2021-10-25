#include <Wire.h>

#define SLAVE 2

int progress = 1;

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

void receiveEvent(int n_bytes) {

    String receivedData = "";
    for(int i = 0; i < n_bytes; i++) {
        receivedData += (char)Wire.read();
    }
    progress = receivedData[5] - '0';

    progress ++;
    if(progress > 9) progress = 1;
}

void requestEvent() {

    Wire.write(data_to_send[progress]);
    Serial.println(data_to_send[progress]);
}

void setup() {

    Wire.begin(SLAVE);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    Serial.begin(9600);
    while(!Serial);
    Serial.println();
    Serial.println("Slave 2 - Ready.");
}

void loop() {

}