#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wiringPi.h>

#define MAGNET_SENSOR 23
#define DOOR_LED 24

volatile bool doorOpen;

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR)){
		doorOpen = true;
	}
	else if(!digitalRead(MAGNET_SENSOR)){
		doorOpen = false;
	}
}

int main(int argc, char* argv[]){
	wiringPiSetupGpio();
	pinMode(MAGNET_SENSOR, INPUT);
	digitalWrite(MAGNET_SENSOR, 1);
	pullUpDnControl(MAGNET_SENSOR, PUD_UP);
	
	wiringPiISR(MAGNET_SENSOR, INT_EDGE_BOTH, magnetRead);
	
	pinMode(DOOR_LED, OUTPUT);
	digitalWrite(DOOR_LED, 0);
	
	magnetRead();
	
	while(1){
		if (doorOpen == true){
			digitalWrite(DOOR_LED, 1);
		}
		else if (doorOpen == false){
			digitalWrite(DOOR_LED, 0);
		}
	}
	
	return 0;
}
