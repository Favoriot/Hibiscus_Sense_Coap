#include <WiFi.h>
#include "coap_client.h"
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
coapClient coap;

const char* ssid     = "your ssid";
const char* password = "your password";

unsigned long start, finished, elapsed;

String apikey = "your accesstoken device";
char device_developer_id[] = "your device developer id";
char url[] = "coap.favoriot.com";
String meth = "POST";
IPAddress host(159,65,134,213);//DNS lookup coap.favoriot.com
int port = 5683;

void callback_response(coapPacket &packet, IPAddress ip, int port);

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port) {
    char p[packet.payloadlen + 1];
    memcpy(p, packet.payload, packet.payloadlen);
    p[packet.payloadlen] = NULL;

    //response from coap server
   if(packet.type==3 && packet.code==0){
      Serial.println("ping ok");
    }

    Serial.println("Stream Created");
}


void setup() {
  Serial.begin(115600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if(!bme.begin()){
     Serial.println("Failed to find BME280 chip"); 
   }
  Serial.println("Setup Response Callback");
  coap.response(callback_response);

  // start coap 
  coap.start();
}

String dataStreams()
{   
     // Data streams format
    String parameters = "{\"device_developer_id\":\"" + String(device_developer_id) + "\",\"data\":{";
           parameters += "\"temperature\":\"" + String(bme.readTemperature()) + "\"";
           parameters += "}}";
    
    String payload = "{\"method\":\"" + meth + "\",";
           payload += "\"apikey\":\"" + apikey + "\",";
           payload += "\"parameters\":"+parameters;
           payload += "}";

           
    return payload;
  
}

void sendStreams(String streams){
        char payload[1024];
        streams.toCharArray(payload,1024);
        Serial.println("Send Request");
        int msgid =coap.post(host,port,url,payload,strlen(payload));
 }
 
void loop() {
    bool state;
    start=millis();  //start counting time
    String json = dataStreams();
    Serial.println("Data Streams:"+json);
    sendStreams(json);
    finished=millis();  //stop counting time
    elapsed=(finished-start);  //calculate time taken to send data
    Serial.print("PROCESSING TIME : " + String(elapsed) + " ms\n");  //display time taken
    Serial.print("**************************************************\n");
    state = coap.loop();
    delay(5000);
  
}
