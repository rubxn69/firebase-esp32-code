#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>
// Firebase credentials
#define WIFI_SSID "your wifi ssid"
#define WIFI_PASSWORD "password"

//update it from your firebase project
#define API_KEY "AIzaSyAFTdCqJtc8AmO9e8_aIFAv5FN8f0RShH8"
#define DATABASE_URL "https://useless-dustbin-default-rtdb.asia-southeast1.firebasedatabase.app/"

// create a user in authentication
#define USER_EMAIL "uselessdustbin1122@gmail.com"
#define USER_PASSWORD "12345678"

#define LED_PIN 2






#define TRIG_PIN 23  // Trig pin connected to GPIO 23
#define ECHO_PIN 22  // Echo pin connected to GPIO 22
#define BUZZER_PIN 15    // Buzzer pin
#define SERVO_PIN 14     // Servo pin

// Create servo object
Servo myServo;

HTTPClient http;



FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Set initial servo angle
int servoAngle = 170;  // Starting angle of the servo motor
int ledState;
long duration;
int distance;
int flag = 0;
// Define distance threshold (in cm)
#define DIST_THRESHOLD 20
#define BUZZ_DURATION 3000  // Buzzer will beep for 3 seconds

void servoclose() {
}

void setup() {
  pinMode(LED_PIN, OUTPUT);

   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
// firebase code
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
  // Start serial monitor
  Serial.begin(9600);

  // Set up the pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  // Attach the servo to the designated pin
  myServo.attach(SERVO_PIN);

  // Initialize the servo at 0 degrees
  myServo.write(servoAngle);
}

void loop() {
  int ledState;
 
 // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration of the pulse from Echo pin
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance (in cm)
  distance = duration * 0.0343 / 2;

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // firebase get code
  if (Firebase.ready()) {

    if(Firebase.RTDB.getInt(&fbdo, "/ledState")){
      ledState = fbdo.intData();
      Serial.print(ledState);
    };

  
  
  };

 
  // Check if the object is within the threshold distance (20 cm)
  if (distance <= DIST_THRESHOLD && flag == 0 && ledState == 0) {
    // Rotate servo to 90 degrees (or any other angle)
    myServo.write(170);
    flag = 1;
  
    // Start buzzing for 3 seconds (fast beeps)
    unsigned long startTime = millis();
    while (millis() - startTime < BUZZ_DURATION) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);  // Short beep duration (100ms)
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);  // Pause between beeps (100ms)
    }

Serial.println("lid close");
  }
 
 else if (ledState == 1 && flag == 1){
    myServo.write(0);
    flag = 0;
    Serial.println("servo reset");
    delay(10000);
  }

  else if (distance > DIST_THRESHOLD ) {
    // Stop the buzzer and servo if object is not within range
    digitalWrite(BUZZER_PIN, LOW);
    myServo.write(0);
    flag = 0;
    Serial.println("lid open");  // Return servo to 0 degrees
  }

  

 
  delay(100);
}
