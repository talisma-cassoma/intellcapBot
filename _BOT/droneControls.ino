//ESP8266

#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <DHT.h>   

#define ssid "******************"     
#define password "*********************" 
#define FIREBASE_HOST "****************" //Firebase Project URL Remove "https:" , "\" and "/"
#define FIREBASE_AUTH "****************"      



const int motor1 = 14; //D5
const int motor2 = 12; //D6
const int motor3 = 13; //D7

#define DHTPIN 2    // Connect Data pin of DHT to D2

#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);

Servo servo;
int angleDegree = 0; 

FirebaseData Humidity_Temp_sensor;
FirebaseData Drone_moves;
FirebaseData Dive;

void setup() {
  
  Serial.begin(115200);

   pinMode(motor1, OUTPUT);
   pinMode(motor2, OUTPUT);
   pinMode(motor3, OUTPUT);
   
   digitalWrite(motor1, LOW);
   digitalWrite(motor2, LOW);
   digitalWrite(motor3, LOW);
  
  dht.begin();
   
   WiFi.begin (ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println ("");
  Serial.println ("WiFi Connected!");
   //servo 
    servo.attach(5); //D1
    servo.write(0);  //set servo to 0°
  
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);  
  //Firebase.reconnectWiFi(true);
}

void DHT11sensorData(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("C  ,"));
  Serial.print(f);
  Serial.println(F("F  "));

  if (Firebase.setFloat(Humidity_Temp_sensor, "/sensors/temperature", t))
  {
    Serial.println("PATH: " + Humidity_Temp_sensor.dataPath());
    Serial.println("TYPE: " + Humidity_Temp_sensor.dataType());
    //Serial.println("ETag: " + Humidity_Temp_sensor.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + Humidity_Temp_sensor.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}
void BotControls(){
    
       //moves forward
       delay(1500);
       digitalWrite(motor1, HIGH);
       digitalWrite(motor2, HIGH);
    
    if (Firebase.getString(Drone_moves, "/controls/turn")) {
      if (Drone_moves.dataTypeEnum() == fb_esp_rtdb_data_type_string) { 
        String turn = Drone_moves.to<const char *>();
        if( turn == "on"){

            if (Firebase.getInt(Drone_moves, "/controls/angle")) {

               if (Drone_moves.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
                  angleDegree = Drone_moves.to<int>();
                  servo.write(angleDegree);
                  Serial.print("angulo: ");
                  Serial.println(angleDegree);

                    //turn to RIGHT
                    if((angleDegree >= 0 && angleDegree < 90 )|| (angleDegree > 270  && angleDegree <= 360)){

                       digitalWrite(motor1, HIGH);
                       digitalWrite(motor2, LOW);
                       //analogWrite(motor1, 244);
                       //analogWrite(motor2, angleDegree);
            }
                    //turn to LEFT
                    if((angleDegree >= 90 && angleDegree <= 270) ){
             
                      //analogWrite(motor1, 200);
                      //analogWrite(motor2, 244);
                      digitalWrite(motor1, LOW);
                      digitalWrite(motor2, HIGH);
                     }
                   }
                } else {Serial.println(Drone_moves.errorReason());}
              }

           } else {Serial.println(Drone_moves.errorReason().c_str());
           
            //moves forward
            digitalWrite(motor1, HIGH);
            digitalWrite(motor2, HIGH);
           }
  //make drone dives     
     if (Firebase.getString(Drone_moves, "/controls/dive")) {
      if (Drone_moves.dataTypeEnum() == fb_esp_rtdb_data_type_string) { 
        String dive = Drone_moves.to<const char *>();
        if( dive == "on"){
            digitalWrite(motor3, HIGH);
            Serial.print("dive: ");
            Serial.println(dive);
          }else{
            digitalWrite(motor3, LOW);
            Serial.print("dive: ");
            Serial.println(dive);
          }
        }

    } else {Serial.println(Drone_moves.errorReason().c_str()); }
        }
    }
void loop() {
  
  DHT11sensorData();
  //if connected do:
  if (Firebase.getString(Drone_moves, "/controls/connection")) {
      if (Drone_moves.dataTypeEnum() == fb_esp_rtdb_data_type_string) { 
        String connection = Drone_moves.to<const char *>();
        if( connection == "on"){
          Serial.print("connection: ");
          Serial.println(connection);
          BotControls();
          }
        if( connection == "off"){
        delay(1500);
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, LOW);
        digitalWrite(motor3, LOW);
        }
       }
   }
  }
