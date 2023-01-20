#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "FirebaseESP8266.h"


#define FIREBASE_HOST "healcare-15ffb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "wwbSgauAcffIDddIyn86eyL7jRYhUNCRLH0Ad5xu"

/* Set these to your desired credentials. */
const char *ssid = "Error 404";  //ENTER YOUR WIFI SETTINGS
const char *password = "GoodDoctor";

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;

//Web/Server address to read/write from 
const char *host = "192.168.43.128";   //https://circuits4you.com website or IP address of server

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //  Firebase
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if(!Firebase.beginStream(firebaseData,path))
  {
    Serial.println("REASON: "+firebaseData.errorReason());
  }
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  HTTPClient http;    //Declare object of class HTTPClient

  String ADCData, machine_code, postData, url;
  int adcvalue=analogRead(A0);  //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion
  machine_code = "INS001";
  
  if(Serial.available() > 0){
    String mystring = Serial.readStringUntil('\n');
    Serial.println(mystring);
    int idx1 = mystring.indexOf(":");
    float temperature = mystring.substring(0,idx1).toFloat();

    Firebase.setFloat(firebaseData,path + "/temp",temperature); //    To Firebase
    
    String temp = String(temperature);
    
    //Post Data
    postData = "temperature=" + temp + "&machine_code=" + machine_code;
    Serial.println(postData);
   
//    url = "http://192.168.1.37:80/api/postdemo.php";
    url = "http://14.176.231.115:8080/api/instrumentation?instrumentationCd="+machine_code+"&temperature="+temp;
    
    http.begin(url);                                                        //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload
  
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    Serial.println(url);
    http.end();  //Close connection
    delay(5000);  //Post Data at every 5 seconds
  }
}
//=======================================================================
