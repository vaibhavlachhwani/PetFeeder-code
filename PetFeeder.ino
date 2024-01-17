#import <Servo.h>
#import <ESP8266WiFi.h>

Servo servo;

int angle = 60;

int servoControl = D5;
int onBoardLed = D4;
int pirIn = D7;

int shortDelay = 1000;
int longDelay = 5000;

const char* ssid = "merl";
const char* passwd = "Voldemort";

int motionEnabled = 0;

String header;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  
  pinMode(pirIn, INPUT);
  pinMode(onBoardLed, OUTPUT);
  pinMode(servoControl, OUTPUT);

  servo.attach(servoControl);
  servo.write(0);

  Serial.println("Please wait, PIR Sensor is warming up.");
  delay(20000);

  WiFi.begin(ssid, passwd);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected -->");
  Serial.println("IP : ");
  Serial.println(WiFi.localIP());

  server.begin();  
}

void loop() {
  //iot
  WiFiClient client = server.available();
  
  if(client) {
    Serial.println("New client connected");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             
        Serial.write(c);                    
        header += c;

        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println("");
            
            if (header.indexOf("GET /feed") >= 0) {
              Serial.println("Feeding");
              openLid(longDelay);
            }

            if (header.indexOf("GET /motionEn") >= 0) {
              Serial.println("Motion Detection Enabled");
              motionEnabled = 1;
            }

            if (header.indexOf("GET /motionDe") >= 0) {
              Serial.println("Motion Detection Deactivated");
              motionEnabled = 0;
            }

            client.println("<!DOCTYPE html><html lang=\"en\"><head><title>Pet Feeder</title><meta charset=\"UTF-8\">"); 
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            client.println("<style>html {display: inline; margin: 0px auto; text-align: centre;background-color: rgb(230, 0, 35);}");
            client.println("h1,h2 {text-align: center;color: rgb(255, 255, 255);}");
            client.println("button {margin: 1em;padding: 20px 46px;font-size: 1em;cursor: pointer;border-radius: 50px;}</style>");

            client.println("<body><h1>Pet Feeder</h1><h2>Keep your cuties fed, hassle free!</h2>");
            client.println("<p><a href=\"/feed\"><button>Feed! &#9825;</button></a></p>");
            
            if(motionEnabled == 0) {
              client.println("<p><a href=\"/motionEn\"><button>Activate Motion Detection</button></a></p>");
            }
            
            if(motionEnabled == 1) {
              client.println("<p><a href=\"/motionDe\"><button>Deactivate Motion Detection</button></a></p>");
            }
    
            client.println("</body></html>");
            client.println("");
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
    Serial.println("Client disconnected");
    Serial.println("");
  }

  //motion control
  int pirState = digitalRead(pirIn);
  if(motionEnabled){
    if(pirState == HIGH) {
      digitalWrite(onBoardLed, HIGH);
      Serial.println("Motion Detected!");
      openLid(shortDelay);
      delay(1000);
    } else {
      digitalWrite(onBoardLed, LOW);
      Serial.println("No Motion Detected!");
    }
  }
}

void openLid(int del) {
  servo.write(angle);
  delay(del);
  servo.write(0);
}
