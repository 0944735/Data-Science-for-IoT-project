#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <softPwm.h>

#define MAGNET_SENSOR 17
#define DOOR_LED 24
#define BUZZER 22

volatile bool doorOpen;

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR)){
		printf("BZZR ON");
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
	
	wiringPiISR(17, INT_EDGE_FALLING, magnetRead);
	
	pinMode(DOOR_LED, OUTPUT);
	digitalWrite(DOOR_LED, 0);
	
	pwmSetClock(0.02);
	softPwmCreate(BUZZER, 0, 100);

	magnetRead();
	
	while(1){
		if (doorOpen == true){
			digitalWrite(DOOR_LED, 1);
			softPwmWrite(BUZZER, 50);
		}
		else if (doorOpen == false){
			digitalWrite(DOOR_LED, 0);
			softPwmWrite(BUZZER, 0);
		}
	}
	
	return 0;
}
