// Drawing
// https://easyeda.com/editor#id=810d7a8253ef49e3b34a33310a44feb1|f42ff98ec78b4bc6b06aadce82c868db|db2cd0d309fe48bc865ad76dae367a53

// TODO
// X Create Flag to turn foil hit target on
// O Look at Analog Values to make hits more reliable
// X Extend the hit time for target practice mode


//////////////////////
//// Define Debug ////
//////////////////////

// #define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTln(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTln(x)
#endif

/////////////////////////
//// Library Imports ////
/////////////////////////

#include <Wire.h>              // Wire for OLED Display
#include <Adafruit_GFX.h>      // OLED Graphics
#include <Adafruit_SSD1306.h>  // OLED Display
#include <Arduino.h>           // Arduino Framework
#include <FastLED.h>           // FastLED for LED Lights

//////////////////////
//// OLED SSD1306 ////
//////////////////////

#define SCREEN_WIDTH 128                                           // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                           // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  // Sets Data for OLED Display
// D1 and D2 are SCL and SDA for the SSD1306 by default

//////////////////
//// FastLED ////
/////////////////

#define NUM_LEDS 300  // FastLED definitions
#define LED_PIN D8

int bpm = 60;                                   // Beats per minute for the LED Lights
int fade_rate = 7;                              // Rate at which the LED Lights fade to black
int Brightness = 255;                           // 0-255 LED brightness scale
int PowerLimit = 900;                           // 900mW Power Limit
int LED_arg = 0;                                // A global variable to be used as an argument in LED Lighting
bool green_run = 1;                             // Flag to Run Green once and then stop once it has faded to black
bool allow_red_leds = 1;                        // Allows the Red LED Indication to Hit Sequence
bool allow_green_leds = 0;                      // Allows the Green LED Hit Indication Sequence
bool correct_target_struck = 0;                 // Shows if the correct target was hit
int randomNum[4];                               // Creates an array of three random integers 1,2,3
CRGB leds[NUM_LEDS];                            // Defines leds as the LED Strip
unsigned long green_reaction_timer = millis();  // Timer for green reaction timer
unsigned long green_reaction_delay = 1000;      // 1000 ms delay

/////////////////
//// LED Pin ////
/////////////////

const int LED_Lights = D8;                 // GPIO15, Pulled to Ground, Boot Fails if Pulled High, LED Light Pin also Data In
bool LED_Light_Flag = 0;                   // Indicates when the light should toggle
unsigned long int LED_Light_Delay = 2000;  // How long the LED Light will stay lit
unsigned long int LED_Light_Timer = 0;     // Timer the LED Light

bool multi_light_flag = 0;                  // Flag to determine multiple or single LED lights during setup
bool multi_light = 0;                       // 0 is a single LED, 1 is multiple LEDs in a strip
bool sleep_mode_flag_target = 1;            // Turns off the light when the foil has been activated for a set period of time
                                            // The variable has the _target added to the end so as not to conflict with a similar definition in Wire.h
bool foil_on_target_hit_flag = 0;           // If foil hits on target, then the color will hit until the light resets
bool foil_on_target_hit_flag_prev = 0;      // If foil hits on target, then the color will hit until the light resets
unsigned long foil_on_target_timer = 0;     // Starts the foil on target timer
unsigned long foil_on_target_delay = 2000;  // The delay for how long the foil light is on
bool sleep_mode_flag_prev = 0;              // Tracks Previous sleep mode Flag so that the timer starts when sleep mode goes from 0 to 1
bool sleep_mode_active = 0;                 // Prevents the LED Lights from turning on
bool foil_off_target_flag = 0;              // Flag for Foil Off Target
int sleep_mode_delay = 5000;                // The time that foil will be activating before it stops. 10 seconds.
unsigned long sleep_mode_timer = 0;         // Timer for foil sleep mode

////////////////
//// A_Line ////
////////////////

const int A_Line = D5;  // GPIO14 for the A_Line, Acts as a ground for the B_Line

////////////////
//// B_Line ////
////////////////

const int B_Line = A0;                  // Analog input for the B_Line
bool B_Line_State = LOW;                // Establishes B_Line_State
bool B_Line_State_Prev = LOW;           // Tracks Previous Value so that the button can actuate on falling or rising edge
unsigned long int B_Line_Timer = 0;     // The Debounce Timer for the B Line
unsigned long int B_Line_Debounce = 2;  // The Debounce time for the B Line, starts with Epee timing by default
float analog_sensitivity = 0.9;         // The fraction of max voltage that will trigger a hit

////////////////
//// C_Line ////
////////////////

const int C_Line = D6;  // GPIO12 for the C_Line, Acts as a ground for the B_Line

//////////////////////
//// Target Lines ////
//////////////////////

const int Target_1 = D7;   // GPIO13, Optional Pulled Up, Target 1
bool Target_1_Flag = 0;    // Flag for Target 1
const int Target_2 = D4;   // GPIO2 Pulled Up, Target 2
bool Target_2_Flag = 0;    // Flag for Target 2
const int Target_3 = D3;   // GPIO0 Pulled Up, Target 3
bool Target_3_Flag = 0;    // Flag for Target 3
bool Off_Target_Flag = 0;  // Flag for Off_Target

//////////////////////
//// Target Timer ////
//////////////////////

// One Timer is used for all Saber Targets
bool Target_State = LOW;                // Establishes Target_Line_State
bool Target_State_Prev = LOW;           // Tracks Previous Value so that the button can actuate on falling or rising edge
unsigned long int Target_Timer = 0;     // The Debounce Timer for the Target Line
unsigned long int Target_Debounce = 4;  // The Debounce time for the Target Line

//////////////////
//// Mode_Pin ////
//////////////////

const int Mode_Button = D0;                   // Optionally Internally Pulled Down Pin 16
bool Mode_Button_State = LOW;                 // Establishes Mode_Button_State
bool Mode_Button_State_Prev = LOW;            // Tracks Previous Value so that the button can actuate on falling or rising edge
bool Mode_Button_Flag = 0;                    // Sets a Flag for when the Mode Button is pressed
unsigned long int Mode_Button_Timer = 0;      // The Debounce Timer for the Mode Button
unsigned long int Mode_Button_Debounce = 30;  // The Debounce time for the Mode Button
unsigned long int long_button_press = 1000;   // The time difference between a long and short button press
unsigned long int weapon_mode = 3;            // Weapon Mode Determines how touches are scored. Starts at 3 so that the first iteration goes to epee
                                              // 0 is Epee
                                              // 1 is Foil
                                              // 2 is Saber
                                              // 3 is Foil_Classic

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  DEBUG_PRINTln("");
  DEBUG_PRINTln("Starting Setup.");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    DEBUG_PRINTln(F("SSD1306 allocation failed"));   // OLED Setup
    for (;;)
      ;
  }

  ///////////////////////
  //// FastLED Setup ////
  ///////////////////////

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // Add our LED strip to the FastLED library
  FastLED.setBrightness(Brightness);                       // Sets Brightness
  set_max_power_indicator_LED(LED_BUILTIN);                // Light the builtin LED if we power throttle
  FastLED.setMaxPowerInMilliWatts(PowerLimit);             // Set the power limit, above which brightness will be throttled

  ///////////////////
  //// Pin Modes ////
  ///////////////////

  pinMode(Mode_Button, INPUT_PULLDOWN_16);  // Sets the mode pin to output and uses the pulldown for the mode pin
  pinMode(A_Line, OUTPUT);                  // set pin D5 as an output pin
  digitalWrite(A_Line, LOW);                // set pin D5 to output a LOW voltage signal
  pinMode(C_Line, OUTPUT);                  // set pin D6 as an output pin
  digitalWrite(C_Line, LOW);                // set pin D6 to output a LOW voltage signal
  pinMode(LED_Lights, OUTPUT);              // LED Output
  pinMode(Target_1, INPUT_PULLUP);          // Target 1 is optional Pull_UP
  pinMode(Target_2, INPUT);                 // Target 2 is Pull_UP by default
  pinMode(Target_3, INPUT);                 // Target 3 is Pull_UP by default

  /////////////////////////
  //// LED Strip Reset ////
  ////////////////////////

  for (int i = 0; i < 50; i++) {
    fadeToBlackBy(leds, NUM_LEDS, fade_rate);  // Fades all LEDs to black gradually to give the impression of motion
    FastLED.show();
  }

  ///////////////////
  //// Multi LED ////
  ///////////////////

  display.clearDisplay();             // Clears Display
  display.setTextSize(1);             // Sets Text Size
  display.setTextColor(WHITE);        // Sets Text Color
  display.setCursor(0, 10);           // Sets Display Cursor
  display.println("Single LED");      // Text to Display
  display.setCursor(0, 20);           // Sets Display Cursor
  display.println("...Short Press");  // Text to Display
  display.setCursor(0, 40);           // Sets Display Cursor
  display.println("LED String");      // Text to Display
  display.setCursor(0, 50);           // Sets Display Cursor
  display.println("...Long Press");   // Text to Display
  display.display();                  // Display previous input data

  while (multi_light_flag == 0) {                // Similar to Mode Button Press
    yield();                                     // Prevent watchdog timer (WDT) soft reset
    Mode_Button_State_Prev = Mode_Button_State;  // Stores the Mode State in the Previous State before updating
    if (digitalRead(Mode_Button) == HIGH) {      // Tests if the Mode Button is Pressed. Pin is Pulled Down, so HIGH is the Button Pressed.
      Mode_Button_State = LOW;
    } else {
      Mode_Button_State = HIGH;
    }

    if ((Mode_Button_State == LOW) && (Mode_Button_State_Prev == HIGH)) {  // Current Mode State is Pressed and Previous is unpressed, Falling Edge
      Mode_Button_Timer = millis();                                        // Starts the Mode Button Timer
    }

    if ((millis() > (Mode_Button_Timer + Mode_Button_Debounce)) && (Mode_Button_State == LOW) && (Mode_Button_Flag == 0)) {
      // Checks the Debounce Timing and the Mode Button is pressed
      Mode_Button_Flag = 1;
    }

    if ((Mode_Button_State == HIGH) && (Mode_Button_State_Prev == LOW) && (Mode_Button_Flag == 1)) {  // Rising Edge after Debounce Check
      if (millis() > (Mode_Button_Timer + long_button_press)) {                                       // Tests for Long Button Press on Rising Edge following Debounce
        DEBUG_PRINTln("LED Lights have been set to Multiple LEDS.");
        multi_light = 1;  // Sets Multi Light to Mulitple LED Lights.
      } else {
        DEBUG_PRINTln("There can be only one...LED Light.");
        multi_light = 0;  // Sets Multi Light to 1 Light
      }
      DEBUG_PRINTln("Turning on LED Light Flag.");
      LED_Light_Delay = millis() - Mode_Button_Timer;
      multi_light_flag = 1;  // Sets the multi_light_flag to 1 so that the setup will continue
      Mode_Button_Flag = 0;  // Resets the Mode Button Flag

      display.clearDisplay();       // Clears Display
      display.setTextSize(1);       // Sets Text Size
      display.setTextColor(WHITE);  // Sets Text Color
      if (multi_light == 0) {
        display.setCursor(0, 5);          // Sets Display Cursor
        display.println("There can be");  // Text to Display
        display.setCursor(30, 25);        // Sets Display Cursor
        display.println("only one");      // Text to Display
        display.setCursor(60, 45);        // Sets Display Cursor
        display.println("...LED");        // Text to Display
      } else {
        display.setCursor(0, 10);           // Sets Display Cursor
        display.println("LED string....");  // Text to Display
        display.setCursor(30, 30);          // Sets Display Cursor
        display.println("Enabled.");        // Text to Display
      }
      display.display();  // Display previous input data
      delay(2000);        // Allows time to read the message
    }
  }

  iterate_mode();  // Iterates the Mode to start the Loop

  ////////////////////////
  //// Setup Complete ////
  ////////////////////////

  DEBUG_PRINTln("");
  DEBUG_PRINTln("Setup Complete.");
}

void loop() {
  // put your main code here, to run repeatedly:

  EVERY_N_MILLISECONDS(250) {
    int sensorValue = analogRead(B_Line);          // read the analog input on A0
    float voltage = sensorValue * (3.3 / 1023.0);  // convert the sensor value to voltage
    DEBUG_PRINT("The Analog voltage is ");
    DEBUG_PRINT(voltage);
    DEBUG_PRINTln(" out of 3.3V.");
  }

  //////////////////////
  //// Mode Button ////
  /////////////////////

  Mode_Button_State_Prev = Mode_Button_State;  // Stores the Mode State in the Previous State before updating
  if (digitalRead(Mode_Button) == HIGH) {      // Tests if the Mode Button is Pressed. Pin is Pulled Down, so HIGH is the Button Pressed.
    Mode_Button_State = LOW;
  } else {
    Mode_Button_State = HIGH;
  }

  if ((Mode_Button_State == LOW) && (Mode_Button_State_Prev == HIGH)) {  // Current Mode State is Pressed and Previous is unpressed, Falling Edge
    Mode_Button_Timer = millis();                                        // Starts the Mode Button Timer to find how long the Mode Button is pressed
  }

  if ((millis() > (Mode_Button_Timer + Mode_Button_Debounce)) && (Mode_Button_State == LOW) && (Mode_Button_Flag == 0)) {
    // Checks the Debounce Timing and the Mode Button is pressed
    Mode_Button_Flag = 1;
  }

  if ((Mode_Button_State == HIGH) && (Mode_Button_State_Prev == LOW) && (Mode_Button_Flag == 1)) {  // Rising Edge after Debounce Check
    if (millis() > (Mode_Button_Timer + long_button_press)) {                                       // Tests for Long Button Press on Rising Edge following Debounce
      DEBUG_PRINT("Long Button Press of time ");
      DEBUG_PRINT(millis() - Mode_Button_Timer);
      DEBUG_PRINTln(" milliseconds.");
      timed_target();  // Reaction Time Function
    } else {
      DEBUG_PRINT("Short Button Press of time ");
      DEBUG_PRINT(millis() - Mode_Button_Timer);
      DEBUG_PRINTln(" milliseconds.");
      iterate_mode();  // Iterates through the Fencing Modes
    }
    DEBUG_PRINTln("Turning on LED Light Flag.");
    LED_Light_Delay = millis() - Mode_Button_Timer;
    LED_Light_Flag = 1;    // Flag to turn on the LED
    Mode_Button_Flag = 0;  // Resets the Mode Button Flag
  }

  if (LED_Light_Flag == 1) {  // Turns on LED if LED Flag is on and sleep mode is not active
    if (multi_light == 0) {                                   // Uses a Single LED if One LED is selected
      digitalWrite(LED_Lights, HIGH);                         // Turns on the LED
      LED_Light_Flag = 0;                                     //Sets the LED Flag to off
      LED_Light_Timer = millis();                             // Starts the timer for the LED Light
    } else {                                                  // Uses the Strip of LED if LED multiple LEDs is selected
      green_run = 1;
      if (sleep_mode_active == 1) {  // Effectively Sets the color of the LEDs to dark
        LED_arg = 3;
        } else if (foil_off_target_flag == 1 && foil_on_target_hit_flag == 0) {  // Only shows off target blue if off target has been hit and on target has not been hit
        LED_arg = 2;
      } else {
        LED_arg = 0;
      }
    }
    Green_Sine_Up_and_Fade(LED_arg);
  }

  if (foil_on_target_hit_flag_prev == 0 && foil_on_target_hit_flag == 1) {
    foil_on_target_timer = millis();  // Starts the foil_on_target_timer
  }

  if (millis() > foil_on_target_timer + foil_on_target_delay) {
    foil_on_target_hit_flag = 0;  // Sets Foil on Target Flag Back to Zero
  }

  if (millis() > LED_Light_Timer + LED_Light_Delay) {  // Turns off the LED at timer expiration
    digitalWrite(LED_Lights, LOW);                     // Turns off the LED
  }

  if (weapon_mode == 0) {
    epee();
  } else if (weapon_mode == 1) {
    foil();
  } else if (weapon_mode == 2) {
    saber(1);  // The Argument 1 is irrelevant in target mode
  } else if (weapon_mode == 3) {
    foil();
  }

  /////////////////////
  //// Sleep Mode ////
  ////////////////////

  if (LED_Light_Flag == 1) {                                         // When the LED Flag lights then the sleep mode timer is activated
    sleep_mode_flag_prev = sleep_mode_flag_target;                   // Detects when the sleep mode is first activated
    sleep_mode_flag_target = 1;                                      // Sets Sleep Mode Flag to 1 if LED Lights are Lit
    if (sleep_mode_flag_prev == 0 && sleep_mode_flag_target == 1) {  // Checks when LED Lights are first lit
      sleep_mode_timer = millis();                                   // Starts the Sleep Mode timer
    }                                                                //
    if (sleep_mode_flag_prev == 1 && sleep_mode_flag_target == 0) {  // Checks when LED Lights are first lit
      sleep_mode_active = 0;                                         // Deactivates Sleep Mode
    }                                                                //
    if (millis() > sleep_mode_timer + sleep_mode_delay) {            // Checks if the LED Flag has been on the set amount of time, 10 sec
      sleep_mode_active = 1;                                         // Activates Sleep Mode
      for (int i = 0; i < NUM_LEDS; i++) {                           // Sets all the LEDs in the strip to Dark
        leds[i] = CRGB(0, 0, 0);                                     //
      }                                                              //
      FastLED.show();                                                // Displays the darkened LEDs
    }
  }
  if (LED_Light_Flag == 0) {
    sleep_mode_timer = millis();
    sleep_mode_active = 0;
  }

  EVERY_N_MILLISECONDS(2000) {
    DEBUG_PRINT("Sleep Mode is ");
    if (sleep_mode_active) {
      DEBUG_PRINTln("Active.");
    } else {
      DEBUG_PRINTln("Disabled.");
    }
    DEBUG_PRINT("LED_Light_Flag is ");
    if (LED_Light_Flag) {
      DEBUG_PRINTln("On.");
    } else {
      DEBUG_PRINTln("Off.");
    }
  }
}

void iterate_mode() {
  // Iterates the Weapon Mode
  if (weapon_mode == 3) {
    weapon_mode = 0;
  } else {
    weapon_mode++;
  }

  if (weapon_mode == 0 || weapon_mode == 2) {  // Ensures foil off target flag only applies to foil
    foil_off_target_flag = 0;
  }

  DEBUG_PRINT("Weapon Mode is ");
  display.clearDisplay();         // Clears Display
  display.setTextColor(WHITE);    // Sets Text Color
  if (weapon_mode == 0) {         // Displays that Epee is selected
    display.setTextSize(4);       // Sets the Text Size to 2
    DEBUG_PRINT("Epee");          // Prints the Weapon Mode
    display.setCursor(10, 15);    // Sets Display Cursor (x/128,y/64)
    display.println("Epee");      // Text to Display
    B_Line_Debounce = 2;          // Sets Epee Debounce to 2 ms
  } else if (weapon_mode == 1) {  // Displays that Foil is selected
    display.setTextSize(4);       // Sets the Text Size to 2
    DEBUG_PRINT("Foil");          // Prints the Weapon Mode
    display.setCursor(15, 20);    // Sets Display Cursor (x/128,y/64)
    display.println("Foil");      // Text to Display}
    B_Line_Debounce = 14;         // Sets Foil Debounce to 14 ms
  } else if (weapon_mode == 2) {  // Displays that Saber is selected
    display.setTextSize(4);       // Sets the Text Size to 2
    DEBUG_PRINT("Saber");         // Prints the Weapon Mode
    display.setCursor(5, 15);     // Sets Display Cursor (x/128,y/64)
    display.println("Saber");     // Text to Display}
    Target_Debounce = 2;          // Sets Target Debounce Time for Saber
  } else if (weapon_mode == 3) {  // Displays that Foil_Classic is selected
    display.setTextSize(3);       // Sets the Text Size to 2
    DEBUG_PRINT("Foil Classic");  // Prints the Weapon Mode
    display.setCursor(0, 5);      // Sets Display Cursor
    display.println("Foil");      // Text to Display}
    display.setCursor(0, 35);     // Sets Display Cursor
    display.println("Classic");   // Text to Display}
    B_Line_Debounce = 2;          // Sets Foil Debounce to 2 ms
  }
  DEBUG_PRINTln(".");  // Prints the Weapon Mode
  display.display();   // Display previous input data

  sleep_mode_flag_target = 0;  // Resets Sleep Mode
}

void saber(int target_number) {

  ///////////////////////
  //// Saber Button ////
  //////////////////////

  Target_State_Prev = Target_State;  // Stores the Target State in the Previous State before updating
  if ((digitalRead(Target_1) == LOW) or (digitalRead(Target_2) == LOW) or (digitalRead(Target_3) == LOW)) {
    Target_State = LOW;  // Tests if a Target is hit and assigns LOW or HIGH value to Target_State
  } else {
    Target_State = HIGH;
  }
  if ((Target_State == LOW) && (Target_State_Prev == HIGH)) {  // Current Target State is Hit but Previous was not, then Falling Edge
    Target_Timer = millis();                                   // Starts the Target Timer
  }

  if (millis() > (Target_Timer + Target_Debounce) && Target_State == LOW) {  // Checks the Debounce Timing to turn on the LED
    LED_Light_Flag = 1;                                                      // Turns on the LED Flag
  }

  if (LED_Light_Flag == 1) {                                       // Turns on LED if LED Flag is on
    LED_arg = 0;                                                   // Green Color for Sine Glow and Fade
    if ((digitalRead(Target_1) == LOW) && (Target_1_Flag == 0)) {  // Prints that Target 1 was Hit
      Target_1_Flag = 1;                                           // Sets Target 1 Flag to 1
      // DEBUG_PRINTln("Target 1 was Hit.");
    }
    if ((digitalRead(Target_2) == LOW) && (Target_2_Flag == 0)) {  // Prints that Target 2 was Hit
      Target_2_Flag = 1;                                           // Sets Target 2 Flag to 1
      DEBUG_PRINTln("Target 2 was Hit.");
    }
    if ((digitalRead(Target_3) == LOW) && (Target_3_Flag == 0)) {  // Prints that Target 3 was Hit
      Target_3_Flag = 1;                                           // Sets Target 3 Flag to 1
      DEBUG_PRINTln("Target 3 was Hit.");
    }
  }

  if ((Target_1_Flag == 1 && randomNum[target_number] == 1) || (Target_2_Flag == 1 && randomNum[target_number] == 2) || (Target_3_Flag == 1 && randomNum[target_number] == 3)) {
    correct_target_struck = 1;  // Checks if the correct target was struck and updates the global variable correct_target_struck
  } else {
    correct_target_struck = 0;
  }

  if (millis() > LED_Light_Timer + LED_Light_Delay) {  // Turns off the LED at timer expiration
    digitalWrite(LED_Lights, LOW);                     // Turns off the LED
    Target_1_Flag = 0;                                 // Resets Target 1 Flag
    Target_2_Flag = 0;                                 // Resets Target 2 Flag
    Target_3_Flag = 0;                                 // Resets Target 3 Flag
    Off_Target_Flag = 0;                               // Resets Off Target Flag
  }
}

void epee() {

  /////////////////////
  //// Epee Button ////
  /////////////////////

  int sensorValue = analogRead(B_Line);          // read the analog input on A0
  float voltage = sensorValue * (3.3 / 1023.0);  // convert the sensor value to voltage
  B_Line_State_Prev = B_Line_State;              // Stores the B_Line in the Previous State before updating
  if (voltage < (3.3 * analog_sensitivity)) {    // Tests Voltage and assigns a LOW or HIGH value to the B_Line
    B_Line_State = LOW;
  } else {
    B_Line_State = HIGH;
  }

  if ((B_Line_State == LOW) && (B_Line_State_Prev == 1)) {  // if voltage is above half of 3.3V
    B_Line_Timer = millis();                                // Starts the B_Line Timer
  }

  if (millis() > (B_Line_Timer + B_Line_Debounce) && B_Line_State == LOW && digitalRead(Target_1) == HIGH && digitalRead(Target_2) == HIGH && digitalRead(Target_3) == HIGH) {
    // Checks if B Line is still low at timer expiration, as well as the Targets not being hit
    LED_Light_Flag = 1;    // Flags the LED to turn on
    foil_on_target_timer;  // Uses the foil timer to keep the epee light on
  }

  if (LED_Light_Flag == 1) {                                       //Turns on LED if LED Flag is on
    LED_arg = 0;                                                   // Green Color for Sine Glow and Fade
    if ((digitalRead(Target_1) == LOW) && (Target_1_Flag == 0)) {  //Prints that Target 1 was Hit
      Target_1_Flag = 1;                                           // Sets Target 1 Flag to 1
      DEBUG_PRINTln("Target 1 was Hit.");
    }
    if ((digitalRead(Target_2) == LOW) && (Target_2_Flag == 0)) {  //Prints that Target 2 was Hit
      Target_2_Flag = 1;                                           // Sets Target 2 Flag to 1
      DEBUG_PRINTln("Target 2 was Hit.");
    }
    if ((digitalRead(Target_3) == LOW) && (Target_3_Flag == 0)) {  //Prints that Target 3 was Hit
      Target_3_Flag = 1;                                           // Sets Target 3 Flag to 1
      DEBUG_PRINTln("Target 3 was Hit.");
    }
    if ((digitalRead(Target_1) == HIGH) && (digitalRead(Target_2) == HIGH) && (digitalRead(Target_3) == HIGH) && (Off_Target_Flag == 0)) {
      //Prints that Off Target was Hit
      Off_Target_Flag = 1;  // Sets Off Target Flag to 1
      // DEBUG_PRINTln("Off Target was Hit.");  // For epee this is the hitting a target that is not grounded such as the bellguard.
    }
  }

  if (millis() > LED_Light_Timer + LED_Light_Delay) {  // Turns off the LED a timer expiration
    digitalWrite(LED_Lights, LOW);                     // Turns off the LED
    Target_1_Flag = 0;                                 // Resets Target 1 Flag
    Target_2_Flag = 0;                                 // Resets Target 2 Flag
    Target_3_Flag = 0;                                 // Resets Target 3 Flag
    Off_Target_Flag = 0;                               // Resets Off Target Flag
  }
}

void foil() {

  /////////////////////
  //// Foil Button ////
  /////////////////////

  int sensorValue = analogRead(B_Line);          // read the analog input on A0
  float voltage = sensorValue * (3.3 / 1023.0);  // convert the sensor value to voltage
  B_Line_State_Prev = B_Line_State;              // Stores the B_Line in the Previous State before updating
  if (voltage < 3.3 * analog_sensitivity) {      // Tests Voltage and assigns a LOW or HIGH value to the B_Line
    B_Line_State = LOW;
  } else {
    B_Line_State = HIGH;
  }

  if ((B_Line_State == HIGH) && (B_Line_State_Prev == LOW)) {  // if voltage is above half of 3.3V and is Rising Edge
    B_Line_Timer = millis();                                   // Starts the B_Line Timer
  }

  if (millis() > (B_Line_Timer + B_Line_Debounce) && (B_Line_State == HIGH)) {  // Checks if B Line is still HIGH at timer expiration
    LED_Light_Flag = 1;                                                         // Flags the LED to turn on
  }

  foil_on_target_hit_flag_prev = foil_on_target_hit_flag;  // Sets the current Foil on Target to Previous

  if (LED_Light_Flag == 1) {                                       //Turns on LED if LED Flag is on
    if ((digitalRead(Target_1) == LOW) && (Target_1_Flag == 0)) {  //Prints that Target 1 was Hit
      Target_1_Flag = 1;                                           // Sets Target 1 Flag to 1
      DEBUG_PRINTln("Target 1 was Hit.");
      LED_arg = 0;                  // Green Color for Sine Glow and Fade
      foil_on_target_hit_flag = 1;  // Foil on target hit
    }
    if ((digitalRead(Target_2) == LOW) && (Target_2_Flag == 0)) {  //Prints that Target 2 was Hit
      Target_2_Flag = 1;                                           // Sets Target 2 Flag to 1
      DEBUG_PRINTln("Target 2 was Hit.");
      LED_arg = 0;                  // Green Color for Sine Glow and Fade
      foil_on_target_hit_flag = 1;  // Foil on target hit
    }
    if ((digitalRead(Target_3) == LOW) && (Target_3_Flag == 0)) {  //Prints that Target 3 was Hit
      Target_3_Flag = 1;                                           // Sets Target 3 Flag to 1
      DEBUG_PRINTln("Target 3 was Hit.");
      LED_arg = 0;                  // Green Color for Sine Glow and Fade
      foil_on_target_hit_flag = 1;  // Foil on target hit
    }
    if ((digitalRead(Target_1) == HIGH) && (digitalRead(Target_2) == HIGH) && (digitalRead(Target_3) == HIGH) && (Off_Target_Flag == 0)) {  //Prints that Off Target was Hit
      Off_Target_Flag = 1;                                                                                                                  // Sets Off Target Flag to 1
      // DEBUG_PRINTln("Off Target was Hit.");
      foil_off_target_flag = 1;
      // LED_arg = 2;  // Blue Color for Sine Glow and Fade
    } else {
      foil_off_target_flag = 0;
    }
  }

  if (millis() > LED_Light_Timer + LED_Light_Delay) {  // Turns off the LED a timer expiration
    digitalWrite(LED_Lights, LOW);                     // Turns off the LED
    Target_1_Flag = 0;                                 // Resets Target 1 Flag
    Target_2_Flag = 0;                                 // Resets Target 2 Flag
    Target_3_Flag = 0;                                 // Resets Target 3 Flag
    Off_Target_Flag = 0;
  }
}

void Red_Indication_to_Hit(int target_number) {

  ///////////////////////////
  //// Red Lights to Hit ////
  ///////////////////////////

  // Target Number is for Saber, denoting which target is lit

  // while (Serial.available() > 0) { Serial.read(); }  // Flush Serial Buffer

  // Glows Bright Red Originating from 3 locations
  if (weapon_mode != 2 || (weapon_mode == 2 && target_number + 1 != 1)) {  // For Saber, the only target is the one that is not lit
                                                                           // if (weapon_mode != 2 && target_number != 1) {  // For Saber, the only target is the one that is not lit
    // LEDs from 0 to 50
    uint16_t sinBeat_0_to_50 = beatsin16(bpm, 0, 50, 0, 16384);  //Phase Shift by 1/4 wavelength 65535*(1/4) so that it starts at the maximum value
    // LEDs from 50 to 100
    uint16_t sinBeat_50_to_100 = beatsin16(bpm, 50, 100, 0, 49151);  // Phase Shift by 3/4 wavelenth 65535*(3/4) so that it starts at the minimum value
    leds[sinBeat_0_to_50] = CRGB::Red;
    leds[sinBeat_50_to_100] = CRGB::Red;
  }
  if (weapon_mode != 2 || (weapon_mode == 2 && target_number + 1 != 2)) {  // For Saber, the only target is the one that is not lit
                                                                           // if (weapon_mode != 2 && target_number != 2) {  // For Saber, the only target is the one that is not lit
    // LEDs from 100 to 150
    uint16_t sinBeat_100_to_150 = beatsin16(bpm, 100, 150, 0, 16384);  //Phase Shift by 1/4 wavelength 65535*(1/4) so that it starts at the maximum value
    // LEDs from 150 to 200
    uint16_t sinBeat_150_to_200 = beatsin16(bpm, 150, 200, 0, 49151);  // Phase Shift by 3/4 wavelenth 65535*(3/4) so that it starts at the minimum value
    leds[sinBeat_100_to_150] = CRGB::Red;
    leds[sinBeat_150_to_200] = CRGB::Red;
  }
  if (weapon_mode != 2 || (weapon_mode == 2 && target_number + 1 != 3)) {  // For Saber, the only target is the one that is not lit
                                                                           // if (weapon_mode != 2 && target_number != 3) {  // For Saber, the only target is the one that is not lit
    // LEDs from 200 to 250
    uint16_t sinBeat_200_to_250 = beatsin16(bpm, 200, 250, 0, 16384);  //Phase Shift by 1/4 wavelength 65535*(1/4) so that it starts at the maximum value
    // LEDs from 250 to 300
    uint16_t sinBeat_250_to_300 = beatsin16(bpm, 250, 300, 0, 49151);  // Phase Shift by 3/4 wavelenth 65535*(3/4) so that it starts at the minimum value
    leds[sinBeat_200_to_250] = CRGB::Red;
    leds[sinBeat_250_to_300] = CRGB::Red;
  }

  fadeToBlackBy(leds, NUM_LEDS, fade_rate);  // Fades all LEDs to black gradually to give the impression of motion

  FastLED.show();
}

void Green_Sine_Up_and_Fade(int LED_Color) {

  // while (Serial.available() > 0) { Serial.read(); }  // Flush Serial Buffer

  // Green Pulses and then goes out
  uint8_t sinBeat = beatsin8(int(bpm/2), 0, 255, 0, 0);  //All LEDs, no phase shift, Uses uint_8

  if ((sinBeat < 250) && (green_run == 1)) {  // Runs if on upwards swing in Sine Curve and the First iteration
    for (int i = 0; i < NUM_LEDS; i++) {      // Sets all the LEDs in the strip to color LED_Color 0 is Green, 1 is Red, 2 is Blue
      if (LED_Color == 0) {
        leds[i] = CRGB(0, sinBeat, 0);  // Uses the Sine Curve to change the intensity of the Green Color
      } else if (LED_Color == 1) {
        leds[i] = CRGB(sinBeat, 0, 0);  // Uses the Sine Curve to change the intensity of the Red Color
      } else if (LED_Color == 2) {
        leds[i] = CRGB(0, 0, sinBeat);  // Uses the Sine Curve to change the intensity of the Blue Color
      } else if (LED_Color == 3) {
        leds[i] = CRGB(0, 0, 0);  // Uses the Sine Curve to change the intensity of the Blue Color
      }
    }
  }

  if (sinBeat < 5) {                      // Stops the cycle once the LEDs have faded to below 10 due to the Sine Curve
    green_run = 0;                        // Stops Running this section of code once green has faded to black
    for (int i = 0; i < NUM_LEDS; i++) {  // Sets all the LEDs in the strip to Dark
      leds[i] = CRGB(0, 0, 0);
      LED_Light_Flag = 0;
    }
  }
  FastLED.show();
}

void Light_Solid_Color(int LED_Color) {

  while (Serial.available() > 0) { Serial.read(); }  // Flush Serial Buffer

  if (LED_Color == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {  // Sets all the LEDs in the strip to Green
      leds[i] = CRGB(0, 255, 0);          // Uses the Sine Curve to change the intensity of the Green Color
    }
  }
  if (LED_Color == 1) {
    for (int i = 0; i < NUM_LEDS; i++) {  // Sets all the LEDs in the strip to Green
      leds[i] = CRGB(255, 0, 0);          // Uses the Sine Curve to change the intensity of the Green Color
    }
  }
  if (LED_Color == 2) {
    for (int i = 0; i < NUM_LEDS; i++) {  // Sets all the LEDs in the strip to Green
      leds[i] = CRGB(0, 0, 255);          // Uses the Sine Curve to change the intensity of the Green Color
    }
  }
  FastLED.show();
}

void Light_Solid_Red_Targets(int target_number) {

  while (Serial.available() > 0) { Serial.read(); }  // Flush Serial Buffer

  if (target_number == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {  // Lights all Regions
      leds[i] = CRGB(255, 0, 0);          //
    }
  }
  if (target_number == 1) {
    for (int i = int(NUM_LEDS / 3); i < NUM_LEDS; i++) {  // Sets Region 2 and 3 LEDs to Red
      leds[i] = CRGB(255, 0, 0);                          //
    }
  }
  if (target_number == 2) {
    for (int i = 0; i < int(NUM_LEDS / 3); i++) {             // Sets Region 1 to Red
      leds[i] = CRGB(255, 0, 0);                              //
    }                                                         //
    for (int i = int(NUM_LEDS * 2 / 3); i < NUM_LEDS; i++) {  // Sets Region 3 to Red
      leds[i] = CRGB(255, 0, 0);                              //
    }
  }
  if (target_number == 3) {
    for (int i = 0; i < int(NUM_LEDS * 2 / 3); i++) {  // Sets Region 1 and 2 to Red
      leds[i] = CRGB(255, 0, 0);                       //
    }
  }
  FastLED.show();
}

void timed_target() {

  ////////////////////////////
  //// Reaction Time Mode ////
  ////////////////////////////

  bool lights_on = 0;                     // Tracks when the lights are on, waiting for the fencer to hit
                                          // LED_Light_Flag is a bool that is activated by the individual weapon functions
  bool reaction_loop_flag = 0;            //Indicates when the loop has been entered
  bool red_light_flag = 0;                // Allows the Red Lights to be turned on
  bool green_light_flag = 0;              // Allows the Green Lights to be turned on
  unsigned long start_loop = millis();    // Measures the start loop for reaction time
  unsigned long start_loop_delay = 3000;  // Adds time to allow the green lights to finish
  int randomtime[3];                      // Creates an array of three int for random times
  int times[3] = { 3000, 3000, 3000 };    // Stores the Reaction Times, default to 3 seconds


  for (int i = 0; i < 3; i++) {
    randomtime[i] = random(1000, 3000);  // Assigns three random times to randomtime array
  }

  for (int i = 0; i < 4; i++) {
    int randNumber = random(1000);
    randomNum[i] = randNumber % 3 + 1;
    DEBUG_PRINTln(randNumber % 3 + 1);
  }

  for (int i = 3; i > 0; i--) {  // Initial Countdown before starting timed_target mode
    display.setTextSize(4);
    display.clearDisplay();
    display.setCursor(48, 20);
    display.println(i);
    display.display();
    delay(1000);
  }

  for (int i = 0; i < 3; i++) {

    start_loop = millis();        // Measures the time for a single loop of three during the reaction time
    reaction_loop_flag = 1;       // Sets the Reaction Loop flag to 1
    red_light_flag = 1;           // Allows the Red Lights to be turned on
    green_light_flag = 0;         // Allows the Green Lights to be turned on
    LED_Light_Flag = 0;           // Resets the LED Light Flag to 0
    correct_target_struck = 0;    // Resets the Correct Target Struck
    foil_on_target_hit_flag = 0;  // Resets Foil on target hit

    while (Serial.available() > 0) { Serial.read(); }  // Flush Serial Buffer
    for (int j = 0; j < NUM_LEDS; j++) {               // Resets the LEDs for another iteration
      leds[j] = CRGB(0, 0, 0);                         // Sets all the LEDs in the strip to Dark
    }                                                  //
    FastLED.show();                                    // Displays the stored FastLED data on the LED Strip

    while (millis() < start_loop + randomtime[i] + start_loop_delay && reaction_loop_flag == 1)  // While loop will continue until loop time out or reaction loop flag is changed
    {
      EVERY_N_MILLISECONDS(500) {  // Prevents wdt reset during while loop
        wdt_reset();
      }

      if (millis() > start_loop + randomtime[i] && LED_Light_Flag == 0) {  // Cycles through the appropriate weapon function while the LED_Light_Flag is not lit
        if (weapon_mode == 0) {
          epee();
        } else if (weapon_mode == 1) {
          foil();
        } else if (weapon_mode == 2) {
          saber(i + 1);
        } else if (weapon_mode == 3) {
          foil();
        }
      }

      if (millis() < start_loop + randomtime[i] && LED_Light_Flag == 0) {  // States Ready on the OLED prior to the Red Light
        display.setTextSize(3);
        display.clearDisplay();
        display.setCursor(20, 20);
        display.println("Ready");
        display.display();
      }

      if (millis() > start_loop + randomtime[i] && LED_Light_Flag == 0) {  // Starts by running the Red Indication to Hit
        if (weapon_mode == 2) {                                            //
          Light_Solid_Red_Targets(randomNum[i + 1]);                       // Uses a random target for Saber only
        } else {                                                           //
          Light_Solid_Red_Targets(0);                                      // Uses a Solid Indication for all weapons except Saber
        }

        display.setTextSize(3);  // States to Hit on the OLED
        display.clearDisplay();
        display.setCursor(20, 20);
        display.println("Go");
        display.display();
      }

      if (LED_Light_Flag == 1) {
        if (red_light_flag == 1) {  // Uses the change in red_light_flag to iterate only once
          DEBUG_PRINT("The Random Target was ");
          DEBUG_PRINT(randomNum[i + 1]);
          DEBUG_PRINTln(".");

          if (correct_target_struck == 1) {
            DEBUG_PRINTln("The correct target was struck.");
          } else {
            DEBUG_PRINTln("The wrong target was hit.");
          }

          times[i] = millis() - (start_loop + randomtime[i]);  // Stores the Reaction time for the iteration
          for (int i = 0; i < NUM_LEDS; i++) {                 // Resets the LEDs for another iteration
            leds[i] = CRGB(0, 0, 0);                           // Sets all the LEDs in the strip to Dark
          }                                                    //
          FastLED.show();                                      // Displays the stored FastLED data on the LED Strip
          green_light_flag = 1;                                // Allows Green Lights to Light
          display.setTextSize(3);                              // States that a Hit has occurred on the OLED
          display.clearDisplay();
          display.setCursor(30, 20);  // Sets Display Cursor (x/128, y/64)
          display.println("Hit!");
          display.display();
        }
        red_light_flag = 0;  // Signals that Red Lights have stopped
      }
      if (green_light_flag == 1) {
        // Green_Sine_Up_and_Fade(LED_arg);
        if (weapon_mode == 2) {  // For Saber
          if (correct_target_struck == 1) {
            Light_Solid_Color(0);  // Green Color for Correct
          } else {
            Light_Solid_Color(2);  // Blue for Incorrect
          }
        } else {
          Light_Solid_Color(LED_arg);
        }
      }
    }
  }

  int reaction_time = 0;  // Averages the Reaction Time Array
  for (int k = 0; k < 3; k++) {
    reaction_time += times[k];
  }

  DEBUG_PRINT("The random number array was: [");
  DEBUG_PRINT(randomNum[0]);
  DEBUG_PRINT(",");
  DEBUG_PRINT(randomNum[1]);
  DEBUG_PRINT(",");
  DEBUG_PRINT(randomNum[2]);
  DEBUG_PRINT(",");
  DEBUG_PRINT(randomNum[3]);
  DEBUG_PRINTln("]");

  DEBUG_PRINT("The average reaction time is: ");
  DEBUG_PRINT(int(reaction_time / 3));
  DEBUG_PRINTln(".");

  display.setTextSize(2);
  display.setCursor(10, 10);  // Sets Display Cursor (x/128, y/64)
  display.clearDisplay();     // Displays the Reaction Time
  display.println("Reaction");
  display.setCursor(40, 35);
  display.println("Time");
  display.display();
  delay(1000);

  for (int i = 0; i < 50; i++) {
    fadeToBlackBy(leds, NUM_LEDS, fade_rate);  // Fades all LEDs to black
    FastLED.show();
  }

  display.setTextSize(4);
  display.setCursor(20, 15);  // Sets Display Cursor (x/128, y/64)
  display.clearDisplay();
  display.println(int(reaction_time / 3));
  display.display();
  delay(4000);

  if (weapon_mode == 0) {  // Resets the Weapon Target Screen
    weapon_mode = 3;
    iterate_mode();
  } else {
    weapon_mode = weapon_mode - 1;
    iterate_mode();
  }
}