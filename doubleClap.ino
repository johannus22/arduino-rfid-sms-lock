const int SoundSensor = 7;
const int LED = 13;
int LEDState = LOW;  
unsigned long lastClapTime = 0;
int clapCount = 0;

void setup() {
    pinMode(SoundSensor, INPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LEDState);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(SoundSensor) == HIGH) {
        if (millis() - lastClapTime > 50) { // Debounce to avoid false detection
            clapCount++;
        }
        lastClapTime = millis();
    }

    if (clapCount == 2) { // Detect double clap
        LEDState = !LEDState; // Toggle LED state
        digitalWrite(LED, LEDState);
        Serial.println("LED Toggled!");
        clapCount = 0; // Reset clap counter
        delay(500); // Small delay to avoid accidental triple detection
    }

    // Reset clap count if no second clap within 1 second
    if (millis() - lastClapTime > 1000) {
        clapCount = 0;
    }
}
