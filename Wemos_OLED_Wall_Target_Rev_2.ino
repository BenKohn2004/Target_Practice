#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <FastLED.h>
#include <ezButton.h>
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool multi_targets = 0;

bool debug = 1;

// 0 mode is light turns on when target is hit
// 1 mode is a light turns on to signal to hit, gives average of three reaction times
bool mode_state = 0;

///////////////////////
///// Mode Button /////
///////////////////////

// Setup for Single Long Click
const int mode_button_pin = 16;

ezButton mode_button(mode_button_pin);       // D0 Button Input to Change of Mode
const int mode_button_debounce_delay = 500;  // 500 millisecond debouce

// Setup for Double Click
ezButton foil_epee_saber_button(mode_button_pin);      // D0 Button Input to Change Weapon Mode
const int foil_epee_saber_button_debounce_delay = 10;  // 10 millisecond debouce

//////////////////////
/// Pin Assignment ///
//////////////////////

// Setup for A/B/C Lines on the Weapon
const int A_Line = 14;  //D5
const int B_Line = 0;   //D3
const int C_Line = 12;  //D6

const int led_lights = 2;  // D7 Output for Lights Relay

// Setup for Weapon Hitting
const int target = 13;           // D6
ezButton target_button(B_Line);  // D6 Detects when the weapon hits the target

///////////////////////
///    Modes and    ///
/// Debounce Timing ///
///////////////////////

const int target_debounce_foil = 16;         // 16 ms for foil
const int target_debounce_epee = 2;          // Epee Debounce time
const int target_debounce_saber = 2;         // Epee Debounce time
const int target_debounce_foil_classic = 2;  // Epee Debounce time

bool lights_on = 0;
unsigned long start_loop = millis();            // Loop for Reaction Time
unsigned long randomtime = random(3000, 5000);  // Random amount of Time Added to Reaction Time Loop

// mode constants
const uint8_t FOIL_MODE = 0;
const uint8_t EPEE_MODE = 1;
const uint8_t SABER_MODE = 2;
const uint8_t FOIL_CLASSIC_MODE = 3;

// Sets Initial Mode to Foil Classic so that the Setup iterates to Foil
uint8_t currentMode = FOIL_CLASSIC_MODE;

// Varialbes for timing loops
long int start_LED_loop = 0;
long int LED_loop_delay = 1000;

long int start_wdt_loop = 0;
long int wdt_loop_delay = 250;

long int update_timer = 0;
long int udpate_timer_delay = 5000;

void setup() {
  // ESP.wdtDisable();   // Disable the WDT
  // ESP.wdtEnable(WDTO_10S);   // Enable the WDT with 10 second timeout period
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_OFF);  // Turns off the Wifi

  pinMode(led_lights, OUTPUT);  // Sets the led_lights pin to output
  pinMode(mode_button_pin, INPUT_PULLUP);

  // Sets the Mode for the Buttons
  mode_button.setDebounceTime(mode_button_debounce_delay);                        // set debounce time to 50 milliseconds
  foil_epee_saber_button.setDebounceTime(foil_epee_saber_button_debounce_delay);  // set debounce time to 10 milliseconds

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display();

  digitalWrite(led_lights, LOW);  // turn the LED off (LOW is the voltage level)

  // Calls Iterate Mode to set parameters for foil
  iterate_mode();

  Serial.println("Setup Complete.");
}

void loop() {
  // put your main code here, to run repeatedly:
  mode_button.loop();             // MUST call the loop() function first
  target_button.loop();           // MUST call the loop() function first
  foil_epee_saber_button.loop();  // MUST call the loop() function first

  //

  if ((millis() > (update_timer + udpate_timer_delay)) && (debug == 1)) {
    Serial.println("In Main Loop");
    update_timer = millis();
    Serial.print("The Mode Button is: ");
    if (digitalRead(16) == 0) {
      Serial.println("Pressed");
    } else {
      Serial.println("Not Pressed");
    }
    Serial.print("The current mode is: ");
    if (currentMode == 0) {
      Serial.println("Foil");
    } else if (currentMode == 1) {
      Serial.println("Epee");
    } else if (currentMode == 2) {
      Serial.println("Saber");
    } else if (currentMode == 3) {
      Serial.println("Foil Classic");
    }
  }


  /////////////////////
  //// Mode Change ////
  /////////////////////
  if (mode_button.isReleased()) {
    // Inverts the Mode_State to Reaction time or Targets
    if ((debug) && (!mode_state)) {
      Serial.println("Switching to Reaction Mode.");
    } else if ((debug) && (mode_state)) {
      Serial.println("Switching to Target Mode.");
    }
    mode_state = !mode_state;
  }

  ///////////////////////////////////
  //// Foil-Epee-Saber Iteration ////
  ///////////////////////////////////

  if ((foil_epee_saber_button.getCount() == 2)) {
    iterate_mode();
  }

  ///////////////////
  /// Target Mode ///
  ///////////////////

  if (mode_state == 0) {
    // Goes to Mode for Each Weapon
    if ((currentMode == FOIL_MODE) && (lights_on == 0)) {
      if (target_button.isReleased()) {
        digitalWrite(led_lights, HIGH);
        start_LED_loop = millis();
        if (debug) { Serial.println("The Foil has Hit."); }
      }
    } else if ((currentMode == EPEE_MODE) && (lights_on == 0)) {
      if (target_button.isPressed()) {
        digitalWrite(led_lights, HIGH);
        start_LED_loop = millis();
        if (debug) { Serial.println("The Epee has Hit."); }
      }
    } else if ((currentMode == SABER_MODE) && (lights_on == 0)) {
      if (target_button.isPressed()) {
        digitalWrite(led_lights, HIGH);
        start_LED_loop = millis();
        if (debug) { Serial.println("The Saber has Hit."); }
      }
    } else if ((currentMode == FOIL_CLASSIC_MODE) && (lights_on == 0)) {
      if (target_button.isReleased()) {
        digitalWrite(led_lights, HIGH);
        start_LED_loop = millis();
        if (debug) { Serial.println("The Foil Classic has Hit."); }
      }
    }
  }

  //////////////////////////
  /// Reaction Time Mode ///
  //////////////////////////

  if (mode_state == 1) {
    Serial.print("Running Reaction Time Mode.");
    int times[3] = { 2000, 2000, 2000 };

    display.setTextSize(4);

    for (int i = 3; i > 0; i--) {
      display.clearDisplay();
      display.setCursor(48, 20);
      display.println(i);
      display.display();
      delay(1000);
    }

    for (int i = 0; i < 3; i++) {
      // target_button.loop();           // MUST call the loop() function first
      // digitalWrite(led_lights, LOW);  // Turns OFF LED Lights
      lights_on = 0;
      digitalWrite(led_lights, LOW);  // Turns off LED Lights
      display.setTextSize(3);
      display.clearDisplay();
      display.setCursor(20, 20);
      display.println("Ready");
      display.display();

      start_loop = millis();

      Serial.print("The ");
      Serial.print(i);
      Serial.print(" reaction timer is: ");
      Serial.print(randomtime);
      Serial.println(" milliseconds.");

      // Starts timer to hit target

      display.setTextSize(4);

      long int wdt_reset_timer = 0;

      while ((millis() < start_loop + randomtime + 2000) && times[i] == 2000) {  // The Light only stays on for 2 seconds

        // yield();

        if (millis() > wdt_reset_timer + 100) {
          wdt_reset();
          wdt_reset_timer = millis();
        }

        if ((millis() > start_loop + randomtime) && (lights_on == 0)) {  //Turns on the Light at a Random Interval
          if (debug) { Serial.println("Turning on the LED Light for Reaction Time."); }
          digitalWrite(led_lights, HIGH);  // Turns on LED Lights
          lights_on = 1;
          display.clearDisplay();
          display.setCursor(48, 20);
          display.println("GO");
          display.display();
        }

        target_button.loop();  // MUST call the loop() function first

        if (lights_on == 1) {
          if (currentMode == FOIL_MODE) {
            if (target_button.isReleased()) {
              digitalWrite(led_lights, LOW);
              lights_on = 0;  // Turns the Lights Off
              times[i] = millis() - (start_loop + randomtime);
              if (debug) { Serial.println("The Foil has hit."); }
            }
          } else if (currentMode == EPEE_MODE) {
            if (target_button.isPressed()) {
              digitalWrite(led_lights, LOW);
              lights_on = 0;  // Turns the Lights Off
              times[i] = millis() - (start_loop + randomtime);
              if (debug) { Serial.println("The Epee has hit."); }
            }
          } else if (currentMode == SABER_MODE) {
            if (target_button.isPressed()) {
              digitalWrite(led_lights, LOW);
              lights_on = 0;  // Turns the Lights Off
              times[i] = millis() - (start_loop + randomtime);
              if (debug) { Serial.println("The Saber has hit."); }
            }
          } else if (currentMode == FOIL_CLASSIC_MODE) {
            if (target_button.isReleased()) {
              digitalWrite(led_lights, LOW);
              lights_on = 0;  // Turns the Lights Off
              times[i] = millis() - (start_loop + randomtime);
              if (debug) { Serial.println("The Foil Classic has hit."); }
            }
          }
        }
        start_wdt_loop = millis();
      }
      // times[i] = millis() - start_loop - randomtime;
      Serial.print("Reaction time for trial ");
      Serial.print(i);
      Serial.print(" is ");
      Serial.println(times[i]);
      lights_on = 0;
      display.clearDisplay();
      display.setCursor(48, 20);
      display.println("HIT");
      display.display();
    }

    int average_time = 0;
    for (int i = 0; i < 3; i++) {
      Serial.print(times[i]);
      Serial.print(",");
      average_time += times[i];
    }
    average_time = int(average_time / 3);
    Serial.println();
    Serial.println(average_time);
    display.clearDisplay();
    display.setCursor(48, 20);
    display.println(average_time);
    display.display();
    delay(3000);

    mode_state = 0;
    foil_epee_saber_button.resetCount();
    currentMode = currentMode - 1;
    iterate_mode();
  }

  // Resets the LED Lights after an interval of LED_loop_delay
  if (millis() > start_LED_loop + LED_loop_delay) {
    digitalWrite(2, LOW);
    lights_on = 0;
  }
}

void iterate_mode() {
  // Iterates the Current Mode
  if (currentMode == 3) {
    currentMode = 0;
  } else {
    currentMode++;
  }

  display.setTextSize(3);
  display.clearDisplay();
  display.setCursor(8, 12);

  // Sets the Debounce Time and Displays the Mode
  if (currentMode == 0) {
    //// Foil will activate when there is no connection between Line_B and Line_C ////
    ezButton target_button(B_Line);                       //Sets the target button to Line B
    pinMode(A_Line, INPUT);                               //Sets Line A as input
    pinMode(C_Line, OUTPUT);                              // Sets Line C as the output for target button
    pinMode(target, INPUT);                               //Sets Target line as input
    digitalWrite(C_Line, LOW);                            // Sets the C Line to Ground
    target_button.setDebounceTime(target_debounce_foil);  //Sets debounce time to foil
    display.println("Foil");
  } else if (currentMode == 1) {
    //// Epee will activate when there is a connection between Line_B and Line_A ////
    ezButton target_button(B_Line);                       // Sets the target button to Line B
    pinMode(A_Line, OUTPUT);                              // Sets Line A as the output for target button
    pinMode(C_Line, INPUT);                               // Sets Line C as the input for target button
    pinMode(target, INPUT);                               //Sets Target line as input
    digitalWrite(A_Line, LOW);                            // Sets the A Line to Ground
    target_button.setDebounceTime(target_debounce_epee);  // Sets debounce time to epee
    display.println("Epee");
  } else if (currentMode == 2) {
    //// Saber will activate when there is a connection between Line_B and Line_A on the opposite Lame ////
    ezButton target_button(B_Line);                        // Sets the target button to Line B
    pinMode(A_Line, INPUT);                                // Sets Line A as the input for target button
    pinMode(C_Line, INPUT);                                // Sets Line C as the input for target button
    pinMode(target, OUTPUT);                               //Sets Target line as ouput
    digitalWrite(target, LOW);                             // Sets the Target Line to Ground
    target_button.setDebounceTime(target_debounce_saber);  // Sets debounce time to epee
    display.println("Saber");
  } else if (currentMode == 3) {
    //// Foil will activate when there is no connection between Line_B and Line_C ////
    ezButton target_button(B_Line);                               //Sets the target button to Line B
    pinMode(A_Line, INPUT);                                       //Sets Line A as input
    pinMode(C_Line, OUTPUT);                                      // Sets Line C as the output for target button
    pinMode(target, INPUT);                                       //Sets Target line as input
    digitalWrite(C_Line, LOW);                                    // Sets the C Line to Ground
    target_button.setDebounceTime(target_debounce_foil_classic);  //Sets debounce time to foil
    display.println("Foil  Classic");
  } else {
    Serial.println("Current Mode in Failed Mode");
  }

  display.display();

  foil_epee_saber_button.resetCount();
}
