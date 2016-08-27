    /* DHTServer - ESP8266 Webserver with a DHT sensor as an input
     
    Based on ESP8266Webserver, DHTexample, and BlinkWithoutDelay (thank you)
     
    Version 1.0 5/3/2014 Version 1.0 Mike Barela for Adafruit Industries
    */
    
    #include <SPI.h>
    #include <Wire.h>
    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
    #include <ESP8266WebServer.h>
    
    #include <DHT.h>
    #include <Adafruit_BMP280.h>
    #include "RTClib.h"

    #define DHTTYPE DHT22
    #define DHTPIN 2
    
    
    
     
    const char* ssid = "AndroidAP";
    const char* password = "uicm4398";
     
    ESP8266WebServer server(80);
    Adafruit_BMP280 bmp; // I2C
    DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
    
    float HR,T1,T2,P,alt,PR,ST,IZ;
    
    String webString=""; // String to display
    
    
    // Generally, you should use "unsigned long" for variables that hold time
    unsigned long previousMillis = 0; // will store last temp was read
    const long interval = 2000; // interval at which to read sensor
    
    void handle_root() {
    server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity");
    delay(100);
    }
    
    void setup(void)
    {
    // You can open the Arduino IDE Serial Monitor window to see what the code is doing
    Serial.begin(115200); // Serial connection from ESP-01 via 3.3v console cable
    dht.begin(); // initialize temperature sensor
     
  /*  if (!bmp.begin()) {  
    Serial.println(F("Chequea el cableado del bmp280 dit 76H"));
    while (1);    
    } */
     
    // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.print("\n\r \n\rWorking to connect");
     
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("DHT Weather Reading Server");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.on("/", handle_root);
    
    server.on("/estacion", [](){ // if you add this subdirectory to your webserver call, you get text below :)
    gettemperature(); // read sensor
    bmp280(); // read sensor
    webString+="<br>Estacion Metereologica - Catriel RN by SPP";
    webString+="<br>";
    webString+="<br>SENSOR DHT22";
    webString+="<br>Humedad Relativa: "+String((int)HR)+" %";
    webString+="<br>Temperatura Ambiente: "+String((int)T1)+" C";
    webString+="<br>";
    webString+="<br>SENSOR BMP280";
    webString+="<br>Presion Atmosferica: "+String((int)P)+" mBAR";
    webString+="<br>Temperatura Ambiente: "+String((int)T2)+" C";
    webString+="<br>Sensacion termica: "+String((int)ST)+" C";
    webString+="<br>Punto o Temp. de Rocio a Presion Atm. : "+String((int)PR)+" C";
    webString+="<br>Indice Zambretti experimental.... : "+String((float)IZ)+" C";
    
    server.send(200, "text/html", webString); // send to someones browser when asked
     webString="";
    });
    
       
    server.begin();
    Serial.println("HTTP server started");
    }
    void loop(void)
    {
    server.handleClient();
    }
     
    void gettemperature() {
        unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    HR = dht.readHumidity(); // Read humidity (percent)
    T1 = dht.readTemperature(false); // Read temperature as Fahrenheit
    
    if (isnan(HR) || isnan(T1)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
    ST = dht.computeHeatIndex(T1, HR, false);
    PR =dht.DewPoint();
    }
    }

    void bmp280(){
	
	P =(bmp.readPressure()/100); //HPA o mBar
    T2 = bmp.readTemperature();
    alt = bmp.readAltitude(1013.25); // this should be adjusted to your local forcase
    IZ = pres_baj(1,1033,10,960,P);    
	// (pres_baj(1,1050,9,985,pres_atm)); 
    //(pres_baj(1,1030,13,947,pres_atm();
	}
	
	
	float pres_baj(float y1,float x1,float y2, float x2,float pres_atm){
	float  prom_met;
	
    float m_aux=pendiente(y1,x1,y2,x2);
    float k_aux =constante(m_aux,y1,x1);
    
    prom_met = (m_aux * pres_atm)+k_aux;
    return prom_met;
	
}



float pendiente(float y1,float x1,float y2, float x2){
float m=0;

 m=(y2-y1)/(x2-x1);
return m;
}

//la constante la obtengo despejando 
float constante(float m,float y1,float x1){
  float c=0;

  c=y1-(m*x1);
  return c;
}














