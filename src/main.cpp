#include <Arduino.h>
#include <WiFi.h>


const char* ssid = "name of network";
const char* password = "password of the network";


WiFiServer server(80);

void connectToWiFi() {
  
  WiFi.begin(ssid, password);
  int attempt = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempt < 10) {
    delay(1000);
    Serial.print(".");
    attempt++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Connected to WiFi.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi. Please check your network credentials.");
  }
}



String header;

IPAddress staticIP(192, 168, 1, 128); 
IPAddress gateway(192, 168, 1, 1);   
IPAddress subnet(255, 255, 255, 0);  


String output45State = "off";


const int output45 = 45;


unsigned long currentTime = millis();

unsigned long previousTime = 0; 

const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  WiFi.config(staticIP, gateway, subnet);
  
  pinMode(output45, OUTPUT);
  
  digitalWrite(output45, LOW);

  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {

if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost. Reconnecting...");
    connectToWiFi(); // Attempt to reconnect to Wi-Fi
  }

  WiFiClient client = server.available();   

  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();            
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
      
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            
            if (header.indexOf("GET /45/openthedoor") >= 0) {
              Serial.println("kapi acildi");
              output45State = "on";
              digitalWrite(output45, HIGH);
              delay(3000);
              digitalWrite(output45, LOW);

            }
            
            // Display the HTML web page with a single button
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off button 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
           
            client.println("<body><h1>ESP32 Web Server</h1>");
            
           
            client.println("<p>click on the button to open the door </p>");

            client.println("<p><a href=\"/45/openthedoor\"><button class=\"button\">open the door</button></a></p>");
            client.println("</body></html>");
            
            
            client.println();
            
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


