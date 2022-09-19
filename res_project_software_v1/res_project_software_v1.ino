#include "DHT.h"

// dht sensors
#define DHT_TYPE DHT11
#define DHT_SENSOR 13

// analog sensors
#define PHOTO_SENSOR A0
#define MOISTURE_SENSOR A1

// fan pins
#define FAN 12
#define FAN_BUTTON 9
#define FAN_POWER 2

// pump pins
#define PUMP 11
#define PUMP_BUTTON  10
#define PUMP_POWER 3

// led pins
#define GREEN_LED 8
#define YELLOW_LED 7
#define RED_LED 6


DHT dht(DHT_SENSOR, DHT_TYPE); 

String input = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(DHT_SENSOR, INPUT);
  
  pinMode(PUMP, OUTPUT);
  pinMode(PUMP_BUTTON, INPUT);
  pinMode(PUMP_POWER, OUTPUT);

  
  pinMode(FAN, OUTPUT);
  pinMode(FAN_BUTTON, INPUT);
  pinMode(FAN_POWER, OUTPUT);

  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  Serial.begin(9600);
  dht.begin();
  display_welcome_text();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() == 0) {
    // do stuff while no text is in serial
    int PUMP_BUTTON_value = digitalRead(PUMP_BUTTON);
    int fan_button_value = digitalRead(FAN_BUTTON);

    
    if(PUMP_BUTTON_value == 1 && fan_button_value == 1){
      Serial.println("Both buttons are pressed");
      fan_pump_activate();
    } else if(PUMP_BUTTON_value == 1){
      Serial.println("PUMP_BUTTON is pressed");
      pump_activate();
    } else if(fan_button_value == 1){
      Serial.println("FAN_BUTTON is pressed");
      fan_activate();
    } else {
      // digitalWrite(FAN, LOW);
      // digitalWrite(PUMP, LOW);
      fan_pump_off();
    }

    
    
    return;
  }
  byte b = Serial.read();

  if(b != '\n'){
    input += (char) b;
    return;
  }  
  Serial.println(input);

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
    Serial.println(F("4. ping <sensor/led/all> - Ping a sensor/LED; all sensors to know if it is functional."));
    Serial.println(F("5. toggle <led_color/pump/fan> <on/off> - Toggle an LED/fan/pump on or off. Overrides any current automatic processes. The LED/fan/pump will continue running until it is toggled off"));
    Serial.println(F("6. get <all/temperature/humidity/brightness/moisture> - Gets temperature/humidity/brightness/moisture/all current measurable sensor metrics"));
    Serial.println(F("7. status <all/warnings/recommendations/environment> - Displays the status (warnings, recommendations, environment) of the current setup."));
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
      if(!ping(PUMP)){
        return 5;
      }
      Serial.print(F("Activating PUMP for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      pump_activate();
      delay(duration);
      fan_pump_off();
      
      Serial.println(F("PUMP Activated Successfully"));
    } else if(module == "fan"){
      if(!ping(FAN)){
        return 5;
      }
      Serial.print(F("Activating FAN for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      fan_activate();
      delay(duration);
      fan_pump_off();
      
      Serial.println(F("FAN Activated Successfully"));
    } else if(module == "led_green"){
      if(!ping(GREEN_LED)){
        return 5;
      }
      Serial.print(F("Activating GREEN_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      led_activate(GREEN_LED);
      delay(duration);
      led_off(GREEN_LED);
      
      Serial.println(F("GREEN_LED Activated Successfully"));
    } else if(module == "led_yellow"){
      if(!ping(YELLOW_LED)){
        return 5;
      }
      Serial.print(F("Activating YELLOW_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      led_activate(YELLOW_LED);
      delay(duration);
      led_off(YELLOW_LED);
      
      Serial.println(F("YELLOW_LED Activated Successfully"));
    } else if(module == "led_red"){
      if(!ping(RED_LED)){
        return 5;
      }
      Serial.print(F("Activating RED_LED for "));
      Serial.print(duration/1000);
      Serial.print(F(" seconds...\n"));
      
      led_activate(RED_LED);
      delay(duration);
      led_off(RED_LED);
      
      Serial.println(F("RED_LED Activated Successfully"));
    } else {
      return 4;
    }
    return 0;
  } else if(command == "ping"){
    return 0;
  } else if(command == "toggle"){
    return 0;
  } else if(command == "get"){
    if (n != 1){
      return 2;
    }
    
    if(arguments == "moisture"){
      if(!ping(MOISTURE_SENSOR)){
        return 5;
      }
      int moisture = analogRead(MOISTURE_SENSOR);
      Serial.print(F("Current Moisture: "));
      Serial.print(moisture);
      Serial.print(F("\n"));

      if(moisture <= 495){
        Serial.print(F("The soil is extremely wet (!) "));
      } else if(moisture >= 496 && moisture <= 670) {
        Serial.print(F("The soil is wet (-) "));
      } else if(moisture >= 671 && moisture <= 845) {
        Serial.print(F("The soil is damp (+) "));
      } else if(moisture >= 846 && moisture <= 1020) {
        Serial.print(F("The soil is dry (-) "));
      } else {
        Serial.print(F("The soil is extremely dry (!) "));
      }
    } else if(arguments == "temperature"){
      if(!ping(DHT_SENSOR)){
        return 5;
      }
      int temperature_c = dht.readTemperature();
      int temperature_f = dht.readTemperature(true);
      
      Serial.print("Temperature: ");
      Serial.print(temperature_c);
      Serial.print(" C ");
      Serial.print(temperature_f);
      Serial.print(" F\n");
      
      if(temperature_c <= 25){
        Serial.print(F("The temperature is frigid (-)"));
      } else if(temperature_c >= 26 && temperature_c <= 30){
        Serial.print(F("The temperature is optimal (+)"));
      } else if(temperature_c >= 31){
        Serial.print(F("The temperature is hot (-)"));
      }
    } else if(arguments == "humidity"){
      if(!ping(DHT_SENSOR)){
        return 5;
      }
      int humidity = dht.readHumidity();
      
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print("%\n");
      
      if(humidity <= 40){
        Serial.print(F("The air is quite dry (-)"));
      } else if(humidity >= 41 && humidity <= 65){
        Serial.print(F("The humidity is optimal (+)"));
      } else if(humidity >= 66){
        Serial.print(F("The air is quite humid (-)"));
      }
      
    } else if(arguments == "brightness"){
      if(!ping(PHOTO_SENSOR)){
        return 5;
      }
      
      int brightness = analogRead(PHOTO_SENSOR);
      Serial.print(F("Current Brightness: "));
      Serial.print(brightness);
      Serial.print(F("\n"));
      
      if(brightness <= 30){
        Serial.print(F("The environment is quite dim (!)"));
      } else if(brightness >= 31 && brightness <= 50){
        Serial.print(F("The environment is lit (-)"));
      } else if(brightness >= 51 && brightness <= 100){
        Serial.print(F("The environment is well lit (+)"));
      } else if(brightness >= 101){
        Serial.print(F("The environment is very bright (+++)"));
      }

    } else {
      return 4;
    }
    return 0;
  } else if(command == "status"){
    return 0;
  }
  return 1;
}

void display_welcome_text(){
   Serial.println(F("\n"));       
   Serial.println(F("                             ____       _                                 "));                                
   Serial.println(F("                            / ___| __ _(_) __ _                           "));                           
   Serial.println(F("  _____ _____ _____ _____  | |  _ / _` | |/ _` |  _____ _____ _____ _____ ")); 
   Serial.println(F(" |_____|_____|_____|_____| | |_| | (_| | | (_| | |_____|_____|_____|_____|"));
   Serial.println(F("     _         _            \\____|\\__,_|_|\\__,_|        _   _             "));
   Serial.println(F("    / \\  _   _| |_ ___       (_)_ __ _ __(_) __ _  __ _| |_(_) ___  _ __  "));
   Serial.println(F("   / _ \\| | | | __/ _ \\ _____| | '__| '__| |/ _` |/ _` | __| |/ _ \| '_ \\ "));
   Serial.println(F("  / ___ \\ |_| | || (_) |_____| | |  | |  | | (_| | (_| | |_| | (_) | | | |"));
   Serial.println(F(" /_/__ \\_\\__,_|\\__\\___/      |_|_|  |_|  |_|\\__, |\\__,_|\\__|_|\\___/|_| |_|"));
   Serial.println(F(" / ___| _   _ ___| |_ ___ _ __ ___          |___/                         "));
   Serial.println(F(" \\___ \\| | | / __| __/ _ \\ '_ ` _ \\   _____ _____ _____ _____ _____ _____ "));
   Serial.println(F("  ___) | |_| \\__ \\ ||  __/ | | | | | |_____|_____|_____|_____|_____|_____|"));
   Serial.println(F(" |____/ \\__, |___/\\__\\___|_| |_| |_|                                      "));
   Serial.println(F("        |___/                          "));
   Serial.println(F(""));

   Serial.println(F("          #o#"));
   Serial.println(F("     ####o#"));
   Serial.println(F("    #o# \#|_#,#                         - Type 'help' to start."));
   Serial.println(F("   ###\ |/   #o#"));
   Serial.println(F("    # {}{      #"));
   Serial.println(F("       }{{"));
   Serial.println(F("      ,'  `"));
}

bool ping(int sensor){
  return true;
}

// functions for activating/turning on leds, motors, pumps
void led_activate(int led){
  digitalWrite(led, HIGH);
}

void led_off(int led){
  digitalWrite(led, LOW);
}

void fan_activate(){
  digitalWrite(FAN_POWER, HIGH);
  digitalWrite(PUMP_POWER, HIGH);
  
  digitalWrite(FAN, LOW);
  digitalWrite(PUMP, HIGH);
}

void pump_activate(){
  digitalWrite(FAN_POWER, HIGH);
  digitalWrite(PUMP_POWER, HIGH);
  
  digitalWrite(FAN, HIGH);
  digitalWrite(PUMP, LOW);
}

void fan_pump_activate(){
  digitalWrite(FAN_POWER, HIGH);
  digitalWrite(PUMP_POWER, HIGH);
  
  digitalWrite(FAN, HIGH);
  digitalWrite(PUMP, HIGH);
}

void fan_pump_off(){
  digitalWrite(FAN, LOW);
  digitalWrite(PUMP, LOW);
  
  digitalWrite(FAN_POWER, LOW);
  digitalWrite(PUMP_POWER, LOW);
}
