#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wiringPi.h>

#define MAGNET_SENSOR 14;

volatile bool doorOpen;

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR){
		doorOpen = true;
	}
	else if(!digitalRead(MAGNET_SENSOR){
		doorOpen = false;
	}
}

int main(int argc, char* argv[]){
	wiringPiSetup();
	pinMode(MAGNET_SENSOR, INPUT);
	pullUpDnControl(MANGET_CONTROL, PUD_UP);
	
	wiringPiISR(MAGNET_SENSOR, INT_EDGE_BOTH, magnetRead);
	
	return 0;
}
