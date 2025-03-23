int SoundSensor=7;
int LED=13;
bool LEDState=false;
int lastClapTime = 0;

void setup() {
 pinMode(SoundSensor,INPUT);
 pinMode(LED,OUTPUT); 
}

void loop() {
    if (digitalRead(SoundSensor)==HIGH) {
        if (millis() - lastClapTime > 300) {
            LEDState = !LEDState;
            digitalWrite(LED, LEDState);
        }
        lastClapTime = millis();
    }
}
