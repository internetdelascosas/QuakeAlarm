// Internet de las cosas / www.internetdelascosas.cl
// Sketch que integra un sensor sismico QuakeAlarm con Arduino
// Version 0.1
// Autor: Jose Antonio Zorrilla @joniuz
// 2013-06-25 @joniuz
// 2013-06-25 @joniuz Corrijo conexion al QuakeAlarm
// 2015-02-04 @joniuz Modificacion para Ethernet Shield

#include <SPI.h>
#include <Ethernet.h>

// Configuracion del dispositivo - Deben ser modificadas para cada equipo

// ID unico para cada estacion de monitoreo, solicitarlo a contacto@iot.cl
char id[40] = "13b08178c3a37c08a8027b2cbbef80e5"; // id de pruebas retorna sensibilidad de 20 y 120000 milisegundos

// Direccion del servidor
char server[] = "dcs.iot.cl";

// Mac unica de cada EthernetShield, normalmente viene impresa en la tarjeta
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };

// Pin Arduino donde esta conectado el QuakeAlarm
#define QUAKEALARM A0

// Descomentar esta linea si se desea un IP fijo
//IPAddress ip(192,168,1,200);

// Descomentar esta linea para asignar un DNS fijo
//IPAddress myDns(172,17,17,1);


// Inicializa la instancia client
EthernetClient client;

// Variable que mantiene el valor anterior del QuakeAlarm
static int diferencia;
static long timer;

// Variables Configuracion QuakeAlarm
int qaValor = 0;
int qaValorPrevio = 0;
int sensibilidad = 10;
long ping = 10000; // Intervalo de tiempo inicial para pings
int intervaloTiempo = 10000; // Intervalo de tiempo entre dos llamadas al servicio web
long ultimaConexion = 0; // Tiempo en milisegundos desde la ultima conexion al servicio web
int ultimoEstado = 0; // Estado de la ultima conexion
long ultimoPing = 0; // Tiempo desde que se envio el ultimo ping al servidor

// Fumcion que realiza conexion http al servidor
void httpRequest(char accion[200]) {
  // Se conecta al servidor en el puerto 80 (web)
  Serial.println("Iniciando conexion...");
  if (client.connect(server, 80)) {
    // Cambia el tipo de mensaje segun la accion (ping|activacion)
    if (accion == "ping") {
      // Envia ping al servidor via GET
      client.print("GET /sensor.php?id=");
      client.print(id);
      client.print("&sensor=quakealarm&v=");
      client.print(qaValor);
      client.println("&valor=ping HTTP/1.1");
      client.print("Host: ");
      client.println(server);
      client.println("User-Agent: Arduino-Ethernet");
      client.println("Connection: close");
      client.println();
      // Actualiza el tiempo en milisegundos del ultimo ping
      ultimoPing = millis();
    }
    else {
       // Activacion del QuakeAlarm
       Serial.print("### Activacion QuakeAlarm : ");
       Serial.print(diferencia);
       Serial.println("##### Internetdelascosas.cl #####");
       Serial.println("## Enviando activacion al servidor#####");
       // Envia ping al servidor via GET
       client.print("GET /sensor.php?id=");
       client.print(id);
       client.print("&sensor=quakealarm&valor=");
       client.print(qaValor);
       client.print("&diferencia=");
       client.print(diferencia);
       client.println(" HTTP/1.1");
       client.print("Host: ");
       client.println(server);
       client.println("User-Agent: Arduino-Ethernet");
       client.println("Connection: close");
       client.println();
       // Actualiza el tiempo en milisegundos de la ultima conexion
       ultimaConexion = millis();
    }
  } 
  else {
    // Si la conexion fallo se desconecta
    Serial.println("Error al conectarse al servidor");
    Serial.println("Desconectando...");
    Serial.println();
    client.stop();
  }
}

// Setup de Arduino
void setup () {   
  // Inicializa puerto serial
  Serial.begin(9600);
  Serial.println("##### Internetdelascosas.cl #####");
  Serial.println("###   Arduino - QuakeAlarm    ###");
  // Espera 1 segundo para que se inicie la tarjeta Ethernet
  delay(1000);
  // Si deseas usar un ip fijo y un DNS fijo descomentar esta linea y comentar la linea 39
  // Ethernet.begin(mac, ip, myDns);
  // Inicializa la tarjeta ethernet mediante DHCP
  Ethernet.begin(mac);
  // Imprime la direccion IP de la tarjeta
  Serial.print("Direccion IP: ");
  Serial.println(Ethernet.localIP());

  // Configura PIN para el QuakeAlarm
  pinMode(QUAKEALARM, INPUT); 
} 

// Loop principal
void loop () {
  // Obtiene el valor del QuakeAlarm
  qaValor = analogRead(QUAKEALARM);   
  // Obtiene la diferencia con el valor anterior  
  diferencia = qaValorPrevio - qaValor;   
  
  // Si hay datos que llegan por la conexion los envia a la puerta serial
  if (client.available()) { 
    // Parsea datos recibidos desde elservidor (sensibilidad, ping)
    if(client.find("<sensibilidad>")) {
      Serial.println("Configuracion recibida:");
      sensibilidad = client.parseInt();
      Serial.print("-> Sensibilidad: ");
      Serial.println(sensibilidad);
    }
    if(client.find("<ping>")) {
      ping = client.parseInt();
      Serial.print("-> Ping: ");
      Serial.println(ping);
    }
    // Asigna valores por defecto en caso de no obtenerlos del servidor
    if (sensibilidad == 0)
      sensibilidad = 10;
    if (ping == 0)
      ping = 10000;
  }

  // Si no hay conexion de red y se conecto correctamente la ultima vez
  // detiene el cliente Ehternet
  if (!client.connected() && ultimoEstado) {
    Serial.println("Cerrando conexion...");
    Serial.println();
    client.stop();
  }
 
  // Si no esta conectado y han pasado X segundos (intervaloConexion) 
  // despues de la ultima conexion envia datos al servidor
  if(!client.connected() && ((diferencia > 0) and (diferencia >= sensibilidad) and ((millis() - ultimaConexion) > intervaloTiempo))) {
    httpRequest("activacion");
  }
  qaValorPrevio = qaValor;
  
  // Envia Ping al servidor - Estacion conectada
  if ((millis() - ultimoPing) > ping) {
    Ethernet.begin(mac);
    // Imprime la direccion IP de la tarjeta
    Serial.println("Ping - Estacion conectada");
    Serial.print("Direccion IP: ");
    Serial.println(Ethernet.localIP());
    httpRequest("ping"); 
  }
  // Actualiza la variable ultimoEstado 
  ultimoEstado = client.connected();
}
// Fin del loop principal 
