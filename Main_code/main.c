#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
#define CHECK_TOPIC "project/check"
#define DOOR_OPEN "OPEN"
#define DOOR_CLOSED "CLOSED"
#define QOS                 2
#define TIMEOUT             10000L

struct globalVar{
	bool doorOpen;
	MQTTClient_deliveryToken token;
	bool alarmSystem;
	MQTTClient client;
	MQTTClient_message pubmsg;
	char topic[20];
};
typedef struct globalVar global_t;

global_t variables; 

extern void MQTTPublish(global_t input);
extern global_t msgInit(global_t input);
extern void GPIO_setup();

void magnetRead(){
	if(digitalRead(MAGNET_SENSOR)){
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

int main(int argc, char* argv[]){
	wiringPiSetupGpio();
	GPIO_setup(variables);
	int rc;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//	variables.pubmsg = MQTTClient_message_initializer;
	variables = msgInit(variables); // <- replacement for line above
	
	MQTTClient_create(&variables.client, ADDRESS, CLIENTID,
						MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    if ((rc = MQTTClient_connect(variables.client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", variables.topic, CLIENTID, QOS);
           
    magnetRead();
    
    int ch;
    
    do{
		ch = getchar();
    }while(ch!='Q' && ch != 'q');
	
	printf("Shutting down....\n");
    MQTTClient_disconnect(variables.client, 10000);
    MQTTClient_destroy(&variables.client);
	
	return 0;
}

void MQTTPublish(global_t input){
	int ret;
	MQTTClient_publishMessage(variables.client, variables.topic, &variables.pubmsg, &variables.token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), variables.pubmsg.payload, variables.topic, CLIENTID);
    ret = MQTTClient_waitForCompletion(variables.client, variables.token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", variables.token);
   
}

global_t msgInit(global_t input){
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
	pullUpDnControl(MAGNET_SENSOR, PUD_UP);
	
	pinMode(BODY_CHECK, INPUT);
	digitalWrite(BODY_CHECK, 1);
	pullUpDnControl(BODY_CHECK, PUD_UP);
	
	wiringPiISR(MAGNET_SENSOR, INT_EDGE_BOTH, magnetRead);
	
	pinMode(DOOR_LED, OUTPUT);
	digitalWrite(DOOR_LED, 0);
}
