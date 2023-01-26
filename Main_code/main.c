#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wiringPi.h>
#include "MQTTClient.h"

#define MAGNET_SENSOR 23
#define DOOR_LED 24
#define BODY_CHECK 22

struct globalVar{
	bool doorOpen;
	MQTTClient_deliveryToken token;
	bool alarmSystem;
};
typedef struct globalVar global_t;

volatile global_t variables; 

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR)){
		variables.doorOpen = true;
	}
	else if(!digitalRead(MAGNET_SENSOR)){
		variables.doorOpen = false;
	}
}

void GPIO_setup(global_t input){
	pinMode(MAGNET_SENSOR, INPUT);
	digitalWrite(MAGNET_SENSOR, 1);
	pullUpDnControl(MAGNET_SENSOR, PUD_UP);
	
	pinMode(BODY_CHECK, INPUT);
	digitalWrite(BODY_CHECK, 1);
	pullUpDnControl(BODY_CHECK, PUD_UP);
	
	wiringPiISR(MAGNET_SENSOR, INT_EDGE_BOTH, magnetRead);
	
	pinMode(DOOR_LED, OUTPUT);
	digitalWrite(DOOR_LED, 0);
}

int main(int argc, char* argv[]){
	wiringPiSetupGpio();
	GPIO_setup(variables);
	return 0;
}
