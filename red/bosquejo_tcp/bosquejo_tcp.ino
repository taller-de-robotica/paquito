#include "WiFiEsp.h"

//#define HOTSPOT
#ifdef HOTSPOT
char ssid[] = "paquito_zero"; 
#else
#include "Network.h"
// Contenido de Network.h
//char ssid[] = "NetName";         // your network SSID (name)
//char pass[] = "Password";        // your network password
#endif

#define DEBUG


//
// Configuración
//
const int LED_PIN = 13;
const unsigned int LOCAL_PORT = 80;    // local port to listen on

//
// Variables globales
//
int ledStatus = LOW;
int status = WL_IDLE_STATUS;
WiFiEspServer server(LOCAL_PORT);



void SETUP_WIFI() {
  Serial1.begin(115200);
  Serial1.write("AT+UART_DEF=9600,8,1,0,0\r\n");   //cambiar los baudios de 115200 a 9600 para que Serial lo pueda procesar. (https://naylampmechatronics.com/blog/21_tutorial-esp8266-parte-i.html)
  delay(200);
  Serial1.write("AT+RST\r\n");
  delay(200);
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1);    // initialize ESP module
  
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No se detectó el escudo WiFi");
    // aquí se queda
    while (true);
  }

  // Conexión a red WiFi
  while (status != WL_CONNECTED) {
#ifdef HOTSPOT
    Serial.print("Intentando inicializar AP ");
    Serial.println(ssid);
    //AP mode
    status = WiFi.beginAP(ssid, 10, "", 0);
#else
    Serial.print("Intentando conectarse a WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
#endif
  } 
  
  Serial.println("Estás conectado a la red");
  printWifiStatus();

  // Inicia el servidor de red en el puerto 80
  server.begin();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("Dirección IP: ");
  Serial.println(ip);
  Serial.print("Escuchando en el puerto ");
  Serial.println(LOCAL_PORT);
  Serial.println();
}

void verifyWiFiStatus() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    Serial.print("Estado de AP ");
    Serial.print(status);
    switch(status) {
      case WL_IDLE_STATUS:
        Serial.println(" WL_IDLE_STATUS");
        break;
      case WL_CONNECTED:
        Serial.println(" WL_CONNECTED");
        break;
    }
  }
}

void addressGet(WiFiEspClient client, String message) {
  if (message.startsWith("GET /H")) {
    Serial.println("Turn led ON");
    ledStatus = HIGH;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else if (message.startsWith("GET /L")) {
    Serial.println("Turn led OFF");
    ledStatus = LOW;
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }
  sendHttpResponse(client);
}

void addressPost(WiFiEspClient client, String message) {
  // Lee el cuerpo del mensaje
  int ini = message.lastIndexOf("\r\n\r\n");
  String command = message.substring(ini + 4);

  Serial.print("--> Ejecutando ");
  Serial.println(command);

  sendJSONResponse(client, command);
}

void serveEspClient(WiFiEspClient client) {
  Serial.println("Llegó un cliente nuevo"); // print a message out the serial port
  while (client.connected()) {              // loop while the client's connected
    delayMicroseconds(10);                  // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.

    int numBytes = client.available();
    if (numBytes) {                         // if there's bytes to read from the client,
      String message = client.readString();

#ifdef DEBUG      
      Serial.println("^_^");
      Serial.println(message);
      Serial.println("o_o");
      Serial.println();
#endif
      
      if (message.startsWith("GET")) {
        addressGet(client, message);
      } else if(message.startsWith("POST")) {
        addressPost(client, message);
      }
      break;
    }
  }
}


void sendHttpResponse(WiFiEspClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // the content of the HTTP response follows the header:
  client.print("The LED is ");
  client.print(ledStatus);
  client.println("<br>");
  client.println("<br>");
  
  client.println("Click <a href=\"/H\">here</a> turn the LED on<br>");
  client.println("Click <a href=\"/L\">here</a> turn the LED off<br>");
  
  // The HTTP response ends with another blank line:
  client.println();
}

void sendJSONResponse(WiFiEspClient client, String command) {
  Serial.println("Respondiento POST");
  String content = "{\"command\": \"" + command + "\", \"status\": \"ok\"}";
  int length = content.length();
  // headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.print("Content-Length:");
  client.println(length);
  client.println();

  // content
  client.print(content);

  // ending
  client.println();
}

// BOARD WORK

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  SETUP_WIFI();
}

void loop() {
  //verifyWiFiStatus();

  WiFiEspClient client = server.available();   // listen for incoming clients
  if (client) {                                // if you get a client
    Serial.println("Se recibió un cliente\n");
    serveEspClient(client);
    //serve(client);
    // cierra la conexión:
    client.stop();
    Serial.println("El cliente se desconectó\n");
  }
}
