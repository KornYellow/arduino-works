#include <Wire.h>
 
#define SLAVE 2
 
int progress = 1;
char receivedData[7];

void receiveEvent(int n_bytes) {

    for(int i = 0; i < n_bytes; i++) {
        receivedData[i]= (char)Wire.read();
    }
    String text = receivedData;
    Serial.println("Received " + text + " from Master");
    progress = receivedData[5] - '0';

    progress ++;
    if(progress > 9) progress = 1;
}
 
void requestEvent() {
  
    receivedData[5] = progress + '0';
    Wire.write(receivedData);
    String text = receivedData;
    Serial.println("Send " + text + " to Master");
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