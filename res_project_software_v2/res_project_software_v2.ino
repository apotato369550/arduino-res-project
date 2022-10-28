#include "DHT.h"
#include <Servo.h>

Servo servo;

int servo_value;
String input = "";
bool fan_toggle = false;
bool pump_toggle = false;

// dht sensors
#define DHT_TYPE DHT11
#define DHT_SENSOR 13

DHT dht(DHT_SENSOR, DHT_TYPE); 

// analog sensors
#define PHOTO_SENSOR A0
#define MOISTURE_SENSOR A1
#define POTENTIOMETER A2

// servo motor
#define SERVO 7

// fan pins
#define FAN_BUTTON 6
#define FAN 9

// pump pins
#define PUMP_BUTTON  5
#define PUMP 8

// led pins
#define GREEN_LED 4
#define YELLOW_LED 3
#define RED_LED 2

void setup() {
  pinMode(DHT_SENSOR, INPUT);
  
  pinMode(FAN_BUTTON, INPUT);
  pinMode(PUMP_BUTTON, INPUT);

  pinMode(FAN, OUTPUT);
  pinMode(PUMP, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  servo.attach(SERVO);
  servo.write(180);

  Serial.begin(9600);
  dht.begin();

  
  display_welcome_text();
}

void loop() {
  
  while (Serial.available() == 0) {
    // do stuff while no text is in serial
    int pump_button_value = digitalRead(PUMP_BUTTON);
    int fan_button_value = digitalRead(FAN_BUTTON);

    
    if(pump_button_value == 1 && fan_button_value == 1){
      Serial.println("Both buttons are pressed");
      servo.detach();
      digitalWrite(FAN, LOW);
      digitalWrite(PUMP, HIGH);
    } else if(pump_button_value == 1){
      Serial.println("PUMP_BUTTON is pressed");
      servo.detach();
      digitalWrite(PUMP, HIGH);
    } else if(fan_button_value == 1){
      servo.detach();
      Serial.println("FAN_BUTTON is pressed");
      digitalWrite(FAN, LOW);
    } else {
      if(!fan_toggle){
        digitalWrite(FAN, HIGH);
      }

      if(!pump_toggle){
        digitalWrite(PUMP, LOW);
      }
      if(!servo.attached()){
        servo.attach(SERVO);
      }
    }
    
    servo_value = analogRead(POTENTIOMETER);
    servo_value = map(servo_value, 0, 1023, 0, 180);
    servo.write(servo_value);
    delay(10);

    return;
  }

  byte b = Serial.read();

  if(b != '\n'){
    input += (char) b;
    return;
  }  
  Serial.println(input + "\n");
  input.toLowerCase();

  // get the main command
  String command = "";
  String arguments = "";
  // variable n = number of arguments
  int n = 0;
  int first_command_index = input.indexOf(" ", 0);

  if(first_command_index < 0){
    command = input;
  } else {
    command = input.substring(0, first_command_index);
    arguments = input.substring(first_command_index + 1);
    
    n = 1;
    for(int i = 0; i < arguments.length(); i++){
       if(isSpace(arguments.charAt(i))){
          n++;
       }
    }
  }

  command.toLowerCase();
  arguments.toLowerCase();

  int exit_code = command_handler(command, arguments, n);

  if(exit_code == 0){
    Serial.println("\nSuccess: Command executed.");
  } else {
    Serial.print("\nError with exit code = ");
    Serial.print(exit_code);
    Serial.println(": ");
  }

  input = "";
}

int command_handler(String command, String arguments, int n){
  if (command == "welcome"){
    if(n > 0){
      return 2;
    }
    display_welcome_text();
    return 0;
  } else if(command == "help"){
    if(n > 0){
      return 2;
    }
    Serial.println(F("Hi! Let's get you started with some basic commands: "));
    Serial.println(F("1. welcome - display the welcome message again:>"));
    Serial.println(F("2. help - Self explanatory. Use this when you need help with basic commands:)"));
    Serial.println(F("3. activate <pump/fan/led_color> <duration_in_seconds> - Activate pump/fan/LED for a set amount of time in seconds. Overrides any current automatic processes"));
    Serial.println(F("4. toggle <led_color/pump/fan> <on/off> - Toggle an LED/fan/pump on or off. Overrides any current automatic processes. The LED/fan/pump will continue running until it is toggled off"));
    Serial.println(F("5. get <all/temperature/humidity/brightness/moisture> - Gets temperature/humidity/brightness/moisture/all current measurable sensor metrics"));
    Serial.println(F("6. status <all/warnings/recommendations/environment> - Displays the status (warnings, recommendations, environment) of the current setup."));
    Serial.println(F("That is all :>"));
    return 0;
  } else if(command == "activate"){
    if(n != 2){
      Serial.println("Arguments: ");
      Serial.println(n);
      return 2;
    }
    int module_index = arguments.indexOf(" ", 0);
    String module = arguments.substring(0, module_index);
    int duration = arguments.substring(module_index).toInt();

    if(duration == 0){
       return 3;
    }

    duration *= 1000;
    
    if(module == "pump"){
      // recreate all the functions for this
      Serial.print(F("Activating PUMP for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
   
      activate(PUMP, duration);
      
      Serial.println(F("PUMP Activated Successfully"));
    } else if(module == "fan"){
      Serial.print(F("Activating FAN for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      activate(FAN, duration);
      
      Serial.println(F("FAN Activated Successfully"));
    } else if(module == "led_green"){
      Serial.print(F("Activating GREEN_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      activate(GREEN_LED, duration);
      
      Serial.println(F("GREEN_LED Activated Successfully"));
    } else if(module == "led_yellow"){
      Serial.print(F("Activating YELLOW_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      activate(YELLOW_LED, duration);
      
      Serial.println(F("YELLOW_LED Activated Successfully"));
    } else if(module == "led_red"){
      Serial.print(F("Activating RED_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      activate(RED_LED, duration);
      
      Serial.println(F("RED_LED Activated Successfully"));
    } else {
      return 4;
    }
    return 0;
  } else if(command == "toggle"){
    if(n != 1){
      Serial.println("Arguments: ");
      Serial.println(n);
      return 2;
    }
    int module_index = arguments.indexOf(" ", 0);
    String module = arguments.substring(0, module_index);
    
    if(module == "pump"){
      // recreate all the functions for this
      Serial.print(F("Toggling PUMP \n"));

      toggle(PUMP);
      
      Serial.println(F("PUMP Toggled Successfully"));
    } else if(module == "fan"){
      Serial.print(F("Toggling FAN \n"));
      
      toggle(FAN);
      
      Serial.println(F("FAN Toggled Successfully"));
    } else if(module == "led_green"){
      Serial.print(F("Toggling GREEN_LED \n"));
      
      toggle(GREEN_LED);
      
      Serial.println(F("GREEN_LED Toggled Successfully"));
    } else if(module == "led_yellow"){
      Serial.print(F("Toggling YELLOW_LED \n"));
      
      toggle(YELLOW_LED);
      
      Serial.println(F("YELLOW_LED Toggled Successfully"));
    } else if(module == "led_red"){
      Serial.print(F("Toggling RED_LED \n"));
      
      toggle(RED_LED);
      
      Serial.println(F("RED_LED Toggled Successfully"));
    } else {
      return 4;
    }
    return 0;
  } else if(command == "get"){

    if(arguments != "moisture" && arguments != "temperature" && arguments != "humidity" && arguments != "brightness" && arguments != "all"){
      return 4;
    }
    
    int moisture = analogRead(MOISTURE_SENSOR);
    int temperature_c = dht.readTemperature();
    int temperature_f = dht.readTemperature(true);
    int humidity = dht.readHumidity();
    int brightness = analogRead(PHOTO_SENSOR);
    
    if(arguments == "moisture" || arguments == "all"){
      Serial.print(F("Current Moisture: "));
      Serial.print(moisture);
      Serial.print(F("\n"));
      if(arguments == "moisture") return 0;
    }
    if(arguments == "temperature" || arguments == "all"){
      Serial.print("Temperature: ");
      Serial.print(temperature_c);
      Serial.print(" C ");
      Serial.print(temperature_f);
      Serial.print(" F\n");
      if(arguments == "temperature") return 0;
    } 
    if(arguments == "humidity" || arguments == "all"){
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print("%\n");
      if(arguments == "humidity") return 0;
    } 
    
    if(arguments == "brightness" || arguments == "all"){
      Serial.print(F("Current Brightness: "));
      Serial.print(brightness);
      Serial.print(F("\n"));
      if(arguments == "brightness") return 0;
    }
    
    return 0;
  } else if(command == "status"){
    return 0;
  }
  return 1;
}

void activate(int pin, int duration){
  if(pin == FAN){
    digitalWrite(pin, LOW);  
  } else {
    digitalWrite(pin, HIGH);
  }
  
  delay(duration);
  
  if(pin == FAN){
    digitalWrite(pin, HIGH);  
  } else {
    digitalWrite(pin, LOW);
  }
}

void toggle(int pin){
  if(pin == FAN){
    if(digitalRead(FAN) == LOW){
      digitalWrite(FAN, HIGH);
      fan_toggle = false;
    } else {
      digitalWrite(FAN, LOW);
      fan_toggle = true;
    }
  } else if(pin == PUMP){
    if(digitalRead(PUMP) == HIGH){
      digitalWrite(PUMP, LOW);
      pump_toggle = false;
    } else {
      digitalWrite(PUMP, HIGH);
      pump_toggle = true;
    }
  } else {
    if(digitalRead(pin) == HIGH){
      digitalWrite(pin, LOW);
    } else {
      digitalWrite(pin, HIGH);
    }
  }
}

void display_welcome_text(){
  Serial.println(F("\n"));
  Serial.println(F("   _____       _         "));                                                           
  Serial.println(F("  / ____|     (_)                                                                "));   
  Serial.println(F(" | |  __  __ _ _  __ _   ______ ______ ______ ______ ______ ______ ______           "));
  Serial.println(F(" | | |_ |/ _` | |/ _` | |______|______|______|______|______|______|______|          "));
  Serial.println(F(" | |__| | (_| | | (_| |                                                             "));
  Serial.println(F("  \\_____|\\__,_|_|\\__,_|       _      _____               _            _             "));
  Serial.println(F("  / ____|                    | |    / ____|             | |          (_)            "));
  Serial.println(F("  | (___  _ __ ___   __ _ _ __| |_  | |  __  __ _ _ __ __| | ___ _ __  _ _ __   __ _ "));
  Serial.println(F("  \\___ \\| '_ ` _ \\ / _` | '__| __| | | |_ |/ _` | '__/ _` |/ _ \\ '_ \\| | '_ \\ / _` |"));
  Serial.println(F("  ____) | | | | | | (_| | |  | |_  | |__| | (_| | | | (_| |  __/ | | | | | | | (_| |"));
  Serial.println(F(" |_____/|_| |_| |_|\\__,_|_|   \\__|  \\_____|\\__,_|_|  \\__,_|\\___|_| |_|_|_| |_|\\__, |"));
  Serial.println(F("  / ____|         | |                                                          __/ |"));
  Serial.println(F(" | (___  _   _ ___| |_ ___ _ __ ___    ______ ______ ______ ______ ______     |___/ "));
  Serial.println(F("  \\___ \\| | | / __| __/ _ \\ '_ ` _ \\  |______|______|______|______|______|          "));
  Serial.println(F("  ____) | |_| \\__ \\ ||  __/ | | | | |                                               "));
  Serial.println(F(" |_____/ \\__, |___/\\__\\___|_| |_| |_|                                               "));
  Serial.println(F("          __/ |                                                                     "));
  Serial.println(F("         |___/                                                                      "));


   Serial.println(F("\n\n          #o#"));
   Serial.println(F("     ####o#"));
   Serial.println(F("    #o# \\#|_#,#                         Developed By: Research Group 1: "));
   Serial.println(F("   ###\\ |/   #o#                        Yap, Villariza, Luistro, "));
   Serial.println(F("    # {}{      #                         Nacua, Cordero, Abella"));
   Serial.println(F("       }{{                                - Type 'help' to start."));
   Serial.println(F("      ,'  `"));
}
