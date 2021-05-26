// send temprature 
// push notification when motion detected // and light is off
// rgb wave and breathing effects
// turn off light when no motion detected for 5 min

// #define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

unsigned long previousMillis = 0;       
const long interval = 1000;     //fade rate
const float vref = 3.3;
const float resolution = vref / 1023.0;
float temperature;
char auth[] = "YourAuthToken";
SimpleTimer timer;


// This function tells Arduino what to do if there is a Widget
// which is requesting data for Virtual Pin (5)
BLYNK_READ(V3)
{
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
  Blynk.virtualWrite(V3, val);
}
// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  // process received value
}
BLYNK_WRITE(V1) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Item 1 selected");
      break;
    case 2: // Item 2
      Serial.println("Item 2 selected");
      break;
    case 3: // Item 3
      Serial.println("Item 3 selected");
      break;
    default:
      Serial.println("Unknown item selected");
  }
}
void sendTemp(){
  temperature = analogRead(A0); //lm35 is connected to pin A0 on NodeMcu
  temperature = (temperature * resolution);
  temperature = temperature * 100.0;
  Blynk.virtualWrite(V0, temperature); //send the value to blynk application
}
    
void setup{
  WiFi.mode(WIFI_STA);
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if (cnt++ >= 10) {
        WiFi.beginSmartConfig();
        while (1) {
          delay(1000);
          if (WiFi.smartConfigDone()) {
            break;
          }
        }
      }
    }
  WiFi.printDiag(Serial);
  Blynk.config(auth);
  // Setup a function to be called every half second
  timer.setInterval(500, sendTemp);
}

void loop()
{
  Blynk.run();
  timer.run();
}
