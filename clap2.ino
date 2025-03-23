int SoundSensor=7;   
int LED=13; 
int LEDStatus = 0;

void setup() {
 pinMode(SoundSensor,INPUT);
 pinMode(LED,OUTPUT); 
}

void loop() {
 int SensorData=digitalRead(SoundSensor); 
   if(SensorData==1){
        if()
    }