#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <millisDelay.h>

const int PIN_ENA = 27;
const int PIN_N1 = 14;
const int PIN_N2 = 12;

//ESP32 LED INTEGRADO
const int PINLED = 2;

const char* ssid = "LLAMAS";
const char* password = "-polonio210alfileres-";

WiFiUDP udp;

Servo servo;

IPAddress ip(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

WebServer server(80);

// Declaración de variables para el parpadeo
bool ledState = LOW;
bool ledBlinking = false;
unsigned long previousMillis = 0;
const long interval = 500;
millisDelay blinkDelay;

void handleConnectionRoot() {
    server.send(200, "text/plain", "Hello from ESP32!");
}

void setup() {

    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(ip, gateway, subnet);

    server.on("/", handleConnectionRoot);

    server.begin();

    Serial.println("WiFi AP Mode activated");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    udp.begin(1234);
    Serial.println("UDP server started");
    servo.attach(32);

    pinMode(PIN_ENA, OUTPUT);
    pinMode(PIN_N1, OUTPUT);
    pinMode(PIN_N2, OUTPUT);
    pinMode(PINLED, OUTPUT);

}

void loop() {
    server.handleClient();

    int packetSize = udp.parsePacket();
    if (packetSize) {
        char buffer[255];
        udp.read(buffer, 255);
        char* type = strtok(buffer, ":");
        char* code = strtok(NULL, ":");
        char* state = strtok(NULL, ":");

        if (strcmp(code, "ABS_RX") == 0) {
            int value = atoi(state);
            int servoValue;
            if (value==0){ servoValue=90;}
            else{
               servoValue = map(value, -32768, 327670, 180 , 0);
            }
            servo.write(servoValue);
            Serial.print("valor  del servo = "); Serial.println(servoValue) ;
           Serial.print("valor  del joystick = "); Serial.println(value) ;
        }

        if (strcmp(code, "ABS_Y") == 0) {
            int value = atoi(state);
            if (value < 0) {
                int motorValueatras = map(abs(value), 0,-327688, 255, 0);
                Serial.print("valor  del motor = "); Serial.println(motorValueatras) ;
                analogWrite(PIN_ENA, motorValueatras);
                digitalWrite(PIN_N2, HIGH);
                digitalWrite(PIN_N1, LOW);


            } else if (value > 0) {
                int motorValueadelante = map(abs(value), 0,3276708 , 0, 255);
                Serial.print("valor  del motor = "); Serial.println(motorValueadelante) ;
                analogWrite(PIN_ENA, motorValueadelante);
                digitalWrite(PIN_N2, LOW);
                digitalWrite(PIN_N1, HIGH);

                
            } else {
                Serial.println("valor  del motor = 0"); 
                analogWrite(PIN_ENA, 0);
                digitalWrite(PIN_N2, LOW);
                digitalWrite(PIN_N1, LOW);

                
            }
             
           Serial.print("valor  del joystickmotor = "); Serial.println(value) ;

        }

        //INTERMITENTES 
        if (strcmp(code, "BTN_TR") == 0) {
            ledBlinking = !ledBlinking; // Cambiar estado del parpadeo
            if (ledBlinking) {
                blinkDelay.start(interval); // Iniciar el parpadeo del LED
            }
        }

        if (ledBlinking) {
            blinkLed();
        } else {
            digitalWrite(PINLED, LOW); // Apagar el LED si no está parpadeando
        }
    }
}

void blinkLed() {
    if (blinkDelay.justFinished()) {
        ledState = !ledState; // Cambiar estado del LED
        digitalWrite(PINLED, ledState);
        blinkDelay.start(interval); // Reiniciar el contador para el siguiente intervalo
    }
}