#include <Wire.h> 
#include <VL53L0X.h>
#include <GyverEncoder.h>

// encoder pins
#define CLK 2
#define DT 3
#define SW 4

// library initialization
Encoder enc1(CLK, DT, SW);
VL53L0X sensor;

// for connection
const int ESPEED_PIN = 1;       // motor step length
const int DIR_PIN = 6;           // motor direction
const int ENABLE_PIN = 7;        // off/on driver
const int LIMIT_SWITCH = 10;      

const int REQUIRED_DISTANCE = 103;
float tolerance = 2;

// distance scan
int scanDistance() {
    
    int sum = 0;
    for(int count = 0; count < 4; count++) {
        if (sensor.timeoutOccurred()) {
            Serial.println("Sensor timeout!");
            continue;
        }
        int distance = sensor.readRangeSingleMillimeters();       
        sum += distance;
    }

    int currentDistance = sum / 4;
    return currentDistance;
}

void isr() {
    enc1.tick();
}

void setup() {
    Serial.begin(9600);

    //init of encoder
    enc1.setType(TYPE2);
    attachInterrupt(digitalPinToInterrupt(CLK), isr, CHANGE);

    //init of distance sensor
    Wire.begin();
    sensor.setTimeout(100);
    if (!sensor.init()) {
        Serial.println("Failed to initialize VL53L0X!");
        while (1);
    }
    sensor.startContinuous();

    //pins init
    pinMode(DIR_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(LIMIT_SWITCH, INPUT);
    pinMode(ESPEED_PIN, OUTPUT);

    //driver off at the start
    digitalWrite(ENABLE_PIN, HIGH);
}

void loop(){

    enc1.tick();
    int currentDistance = scanDistance();

    if (abs(currentDistance - REQUIRED_DISTANCE) <= tolerance) {
        digitalWrite(ENABLE_PIN, LOW);
    } else {
        digitalWrite(ENABLE_PIN, HIGH);

        if (currentDistance > REQUIRED_DISTANCE) {
            digitalWrite(DIR_PIN, HIGH);
        } else {
            digitalWrite(DIR_PIN, LOW);
        }
    }
}
