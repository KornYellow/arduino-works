void setMode(int mode_1, int mode_2, int mode_3) {
    
    if(mode_1) pinMode(2, INPUT); else pinMode(2, OUTPUT);
    if(mode_2) pinMode(3, INPUT); else pinMode(3, OUTPUT);
    if(mode_3) pinMode(4, INPUT); else pinMode(4, OUTPUT);
}
void write(int status_1, int status_2, int status_3) {
    
    if(status_1 != -1) digitalWrite(2, status_1);
    if(status_2 != -1) digitalWrite(3, status_2);
    if(status_3 != -1) digitalWrite(4, status_3);
    delay(100);
}

void setup() {
  
}

void loop() {
   
    setMode(0, 0, 1);
    write(HIGH, LOW, -1);
    write(LOW, HIGH, -1);

    setMode(1, 0, 0);
    write(-1, HIGH, LOW);
    write(-1, LOW, HIGH);

    setMode(0, 1, 0);
    write(HIGH, -1, LOW);
    write(LOW, -1, HIGH);
    write(HIGH, -1, LOW);

    setMode(1, 0, 0);
    write(-1, LOW, HIGH);
    write(-1, HIGH, LOW);

    setMode(0, 0, 1);
    write(LOW, HIGH, -1);
}