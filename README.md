# Data-Science-for-IoT-project

Het project voor het keuzevak Data Science for IoT.

Het project is een alarm systeem voor de voordeur. Het maakt gebruik van een magneetlezer en een IR sensor om bij te houden wanneer de deur open is en op welk tijdstip iemand naar binnen is gegaan. Er is ook een interface zodat de gebruiker kan aangeven niet thuis te zijn / gaat slapen waardoor er een alarm wordt aangezet. Wanneer het alarm afgaat, gaat er bij de pi een buzzer af en krijgt de gebruiker een email binnen dat iemand is binnengebroken.

De IoT-pipeline van het project ziet er zo uit:

<p align="center">
![bbbb](https://i.imgur.com/kC5iHOo.png)
  </p>

Het project bestaat uit twee sofware onderdelen:
- Pi C-code
- Node Red dashboard

Naast dit heeft het project ook een hardware onderdeel die uitgelicht wordt in het projectdocumentatie.

Voor beide codes is er setup nodig om het programma te runnen op een Raspberry Pi 4

## Pi c-code

Voordat de C-code wordt gebruikt moeten er twee libraries gecloned worden:
- [wiringPi](https://github.com/WiringPi/WiringPi)
- [paho MQTT for C](https://github.com/eclipse/paho.mqtt.c)

Installation guides zijn beschikbaar op de repositories.

Nadat alles is geinstalleerd kan de build.sh via bash gerunt worden. Er kan ook op de executable geklikt worden.

```bash
./build.sh
```
Daarna kan de main.o bestand gerunt en dan runt het systeem. In de CLI kan bijgehouden wat er gebeurt.

```bash
main.o
```

## Node Red

Om de flows te kunnen gebruiken moet er eerst [Node Red](https://nodered.org/docs/getting-started/raspberrypi) geinstalleerd worden op de pi. Naast dit moeten er twee Node Red palletes geinstalleerd worden:
- [node-red-dashboard](https://flows.nodered.org/node/node-red-dashboard)
- [node-red-node-email](https://flows.nodered.org/node/node-red-node-email)
- [@prescient-devices/node-red-contrib-downloadfile](https://flows.nodered.org/node/@prescient-devices/node-red-contrib-downloadfile?utm_source=YouTube&utm_medium=tutorial&utm_campaign=prescient-tutorials&utm_id=prescient)

Wanneer alles is geinstalleerd kan deze respository gecloned worden.

Alle JSON bestanden in deze repository kunnen gedownload worden en daarna geimporteerd worden naar Node Red.

**LET OP:**

De Log.txt is cruciaal voor de werking van de log. Zonder dit tekst bestand kan er niet in de log geschreven of gelezen worden. Dus dit bestand in deze folder zetten als het flows.json bestand.
