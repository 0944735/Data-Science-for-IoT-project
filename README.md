# Data-Science-for-IoT-project

Het project voor het keuzevak Data Science for IoT.

Het project is een alarm systeem voor de voordeur. Het maakt gebruik van een magneetlezer en een IR sensor om bij te houden wanneer de deur open is en hoeveel mensen er naar binnen gaan. Er is ook een interface zodat de gebruiker kan aangeven niet thuis te zijn / gaat slapen waardoor er een alarm wordt aangezet.

Het project bestaat uit twee onderdelen:
- Pi c-code
- Node Red dashboard

Voor beide codes is er setup nodig om het programma te runnen op een Raspberry Pi 4

## Pi c-code

Voordat de C-code wordt gebruikt moeten er twee libraries gecloned worden:
- [wiringPi](https://github.com/WiringPi/WiringPi)
- [paho MQTT for C](https://github.com/eclipse/paho.mqtt.c)
