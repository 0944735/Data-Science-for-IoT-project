#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <wiringPi.h>
#include "MQTTClient.h"

#define MAGNET_SENSOR 25
#define DOOR_LED 24
#define BODY_CHECK 22
#define BUZZER 17
#define CLIENTID "ALARM SYSTEM OMEGA"
#define ADDRESS  "mqtt://broker.hivemq.com:1883"
#define ALARM_TOPIC "project/alarm"
#define DOOR_TOPIC "project/deur"
#define NOT_TOPIC "project/melding"
#define PERSON_TOPIC "project/persoon"
#define CHECK_TOPIC "project/check"
#define DOOR_OPEN "OPEN"
#define DOOR_CLOSED "CLOSED"
#define INTRUDER_ALERT "M"
#define PERSON_DETECTED "P"
#define QOS                 2
#define TIMEOUT             10000L

//todo: buzzer code -> waiting for logic converter to arrive

struct globalVar{
	bool doorOpen;
	MQTTClient_deliveryToken token;
	bool alarmSystem;
	bool intruderEntered;
	MQTTClient client;
	MQTTClient_message pubmsg;
	char topic[20];
};
typedef struct globalVar global_t; //global variable struct for easy access

global_t variables; 

extern void MQTTPublish(global_t input); //MQTT publish to a topic
extern global_t msgInit(global_t input); //initialization for MQTT message because of the usage of the global variable structure
extern void GPIO_setup(); //General gpio setup.
extern int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message); //Callback function for receiving payloads.

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    variables.token = dt;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR)){ //Message are sent depending on the interrupt edge
		variables.doorOpen = true;
		digitalWrite(DOOR_LED, 1);
		variables.pubmsg.payload = DOOR_OPEN;
		variables.pubmsg.payloadlen = strlen(DOOR_OPEN);
		variables.pubmsg.qos = QOS;
		variables.pubmsg.retained = 0;
		strcpy(variables.topic, DOOR_TOPIC);
		MQTTPublish(variables);
	}
	else if(!digitalRead(MAGNET_SENSOR)){
		variables.doorOpen = false;
		digitalWrite(DOOR_LED, 0);
		variables.pubmsg.payload = DOOR_CLOSED;
		variables.pubmsg.payloadlen = strlen(DOOR_CLOSED);
		variables.pubmsg.qos = QOS;
		variables.pubmsg.retained = 0;
		strcpy(variables.topic, DOOR_TOPIC);
		MQTTPublish(variables);
	}
}

void intruderCheck(){
	delay(2);
	if (variables.doorOpen == true){ //checks if door is open
		variables.pubmsg.payload = PERSON_DETECTED;
		variables.pubmsg.payloadlen = strlen(PERSON_DETECTED);
		variables.pubmsg.qos = QOS;
		variables.pubmsg.retained = 0;
		strcpy(variables.topic, PERSON_TOPIC);
		MQTTPublish(variables); //check if alarm is turned on
		if ((variables.alarmSystem == true) && (variables.intruderEntered == false)){ //Check if alarm system is turned on, intruderEntered is for email spam protection
			variables.intruderEntered = true;
			variables.pubmsg.payload = INTRUDER_ALERT;
			variables.pubmsg.payloadlen = strlen(INTRUDER_ALERT);
			variables.pubmsg.qos = QOS;
			variables.pubmsg.retained = 0;
			strcpy(variables.topic, NOT_TOPIC);
			MQTTPublish(variables);
		}
	}
	
}

int main(int argc, char* argv[]){
	wiringPiSetupGpio(); //<- needed for C gpio programming
	GPIO_setup(variables);
	int rc;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//	variables.pubmsg = MQTTClient_message_initializer;
	variables = msgInit(variables); // <- replacement for line above
	
	MQTTClient_create(&variables.client, ADDRESS, CLIENTID,
						MQTTCLIENT_PERSISTENCE_NONE, NULL); //create MQTT client
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    MQTTClient_setCallbacks(variables.client, NULL, connlost, msgarrvd, delivered);
    
    if ((rc = MQTTClient_connect(variables.client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    } //connect to the broker
    
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", variables.topic, CLIENTID, QOS);
    MQTTClient_subscribe(variables.client, ALARM_TOPIC, QOS);       
    magnetRead(); //initial read of the door sensors
    
    int ch;
    
    do{
		ch = getchar();
    }while(ch!='Q' && ch != 'q');
	
	printf("Shutting down....\n");
    MQTTClient_disconnect(variables.client, 10000); //MQTT disconnect when the user wants to
    MQTTClient_destroy(&variables.client);
	
	return 0;
}

void MQTTPublish(global_t input){ //publish function to the topic specified 
	int ret;
	MQTTClient_publishMessage(variables.client, variables.topic, &variables.pubmsg, &variables.token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), variables.pubmsg.payload, variables.topic, CLIENTID);
    ret = MQTTClient_waitForCompletion(variables.client, variables.token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", variables.token);
   
}

global_t msgInit(global_t input){ //these are the settings for the initializer
	strcpy(input.pubmsg.struct_id, "MQTM");
	input.pubmsg.struct_version = 1;
	input.pubmsg.payloadlen = 0;
	input.pubmsg.payload = NULL;
	input.pubmsg.qos = 0;
	input.pubmsg.retained = 0;
	input.pubmsg.dup = 0;
	input.pubmsg.msgid = 0;
	input.pubmsg.properties = (struct MQTTProperties) {0, 0, 0, NULL};
	return input;
}

void GPIO_setup(global_t input){
	pinMode(MAGNET_SENSOR, INPUT);
	digitalWrite(MAGNET_SENSOR, 1);
	pullUpDnControl(MAGNET_SENSOR, PUD_UP); //Pull-up resistors since the sensor is active low
	
	pinMode(BODY_CHECK, INPUT);
	digitalWrite(BODY_CHECK, 1);
	pullUpDnControl(BODY_CHECK, PUD_UP); //same as above
	
	wiringPiISR(MAGNET_SENSOR, INT_EDGE_BOTH, magnetRead);
	wiringPiISR(BODY_CHECK, INT_EDGE_FALLING, intruderCheck); 
	
	pinMode(DOOR_LED, OUTPUT); //LED to indicate if the door is open for testing
	digitalWrite(DOOR_LED, 0);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) // this code is based off the paho mqtt subscribe example: https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/subasync.html
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    if (payloadptr[0] == 'A'){
        variables.alarmSystem = true;
    }
    if (payloadptr[0] == 'S'){
        variables.alarmSystem = false;
        variables.intruderEntered = false; //essentially resetting functionality when the alarm is turned off
    }
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }

    putchar('\n');
    if (variables.alarmSystem == true){
        printf("ALARM ON\n");
    }
    else{
        printf("ALARM OFF\n");
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName); //clear memory

    return 1;
}
