// This #include statement was automatically added by the Spark IDE.
#include "LedControl-MAX7219-MAX7221/LedControl-MAX7219-MAX7221.h"

LedControl *led;
int phase = 0;
char message[64];
int messageLength = 0;
int myUptime = 0;

uint8_t data = A2;
uint8_t load = A1;
uint8_t myclock = A0;
uint8_t potPin = A3;
uint8_t dataPin = D0;
uint8_t latchPin = D1;
uint8_t clockPin = D2;
uint8_t buttonPin = D3;
int val = 0;
char buf[12];
int thisThing = 1;
int currentShift = 0;
int changeShift;
int buttonState = 0;

String arrDays[] = {"Before School","At School","After School","Dinner Time","Bath Time"};
int arrShift[] = {128,64,32,16,8};

void setup() {
    Spark.publish("notifyr/announce", "Craig");
    led = new LedControl(data,myclock,load,1); //DIN,CLK,CS,HowManyDisplays
    led-> shutdown(0,false); //Turn it on
    led-> setIntensity(0,1);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(buttonPin, INPUT);
}

void loop() {
    buttonState = digitalRead(buttonPin);
    if(buttonState==HIGH){
        changeShift = currentShift ^ arrShift[thisThing];
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, changeShift);
        Spark.publish("stickerchart", itoa(changeShift,buf,10));
        digitalWrite(latchPin, HIGH);
        currentShift = changeShift;
    }

    if(phase==0){ //Message-loop starts
        val = analogRead(potPin);
        thisThing = val/819;
        //sprintf(message,itoa(val, buf, 10)); //update message
        sprintf(message,arrDays[thisThing]); //update message
        messageLength = strlen(message); // ...and length
        led->tweenLetters(0,' ',message[phase]); //scroll from empty to 1 letter
    }

    if(phase<messageLength-1){ //next characters except last one
        led->tweenLetters(0,message[phase],message[phase+1]);
        phase++;
    }else if(phase==messageLength-1){//last character scrolls to empty
        led->tweenLetters(0,message[phase],' ');
        phase = 0; //restart message-loop
    }
}
