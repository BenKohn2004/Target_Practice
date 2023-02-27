/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <FastLED.h>

#include <ezButton.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// 0 mode is light turns on when target is hit
// 1 mode is a light turns on to signal to hit, gives average of three reaction times
bool mode_state = 0;
ezButton mode_button(14);  // D5 Button Input to Change of Mode
// const int mode_button = 14;                   // D5 Button Input to Change of Mode
const int mode_button_debounce_delay = 500;  // 500 millisecond debouce
// unsigned long mode_button_debounce_time = 0;  // The last time that the mode button was toggled
int mode_btnState = 0;

ezButton foil_epee_saber_button(14);                   // D5 Button Input to Change of debounce time for Epee and Saber
const int foil_epee_saber_button_debounce_delay = 10;  // 10 millisecond debouce

ezButton target_button(12);  // D6 Detects when the weapon hits the target
// const int target_button = 12;                   // D6 Detects when the weapon hits the target
int target_debounce_delay = 14;      // 14 ms for foil
const int target_debounce_epee = 2;  // Epee Debounce time
int target_btnState = 0;
// unsigned long target_debounce_time = 0;  // The last time that the target button was toggled

const int led_lights = 13;  // D7 Output for Lights Relay

bool turn_on_led = 0;
bool lights_on = 0;
bool foil = 1;
unsigned long start_loop = millis();
unsigned long randomtime = random(3000, 5000);
unsigned long count = 0;
// unsigned long start_timer = millis();

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // // PinMode Set up
  // pinMode(mode_button, INPUT_PULLUP);    // Sets the Mode Button to input and pullup
  // pinMode(target_button, INPUT_PULLUP);  // Sets the Target Button to input and pullup
  pinMode(led_lights, OUTPUT);  // Sets the led_lights pin to output

  mode_button.setDebounceTime(mode_button_debounce_delay);                        // set debounce time to 50 milliseconds
  target_button.setDebounceTime(target_debounce_delay);                           // set debounce time to 14 milliseconds
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
}

void timed_target() {
  // Runs Through Three Timed Hits and Returns the Reaction Time
  int times[3] = { 2000, 2000, 2000 };

  display.setTextSize(4);
  // display.clearDisplay();
  // display.println("Ready");
  // display.display();
  // delay(1000);

  for (int i = 3; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(48, 20);
    display.println(i);
    display.display();
    delay(1000);
  }

  Serial.print("Running Reaction Time Mode.");



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

    while ((millis() < start_loop + randomtime + 2000) and times[i] == 2000) {  // The Light only stays on for 2 seconds

      EVERY_N_MILLISECONDS(500) {
        wdt_reset();
      }

      if ((millis() > start_loop + randomtime) and (lights_on == 0)) {  //Turns on the Light at a Random Interval
        digitalWrite(led_lights, HIGH);                                 // Turns on LED Lights
        lights_on = 1;
        display.clearDisplay();
        display.setCursor(48, 20);
        display.println("GO");
        display.display();
      }

      target_button.loop();  // MUST call the loop() function first

      // if ((target_button.isReleased()) and (lights_on == 1)) {
      if (((target_button.isReleased()) and (lights_on == 1) and (foil == true)) or ((target_button.isPressed()) and (lights_on == 1) and (foil == false))) {
        times[i] = millis() - start_loop - randomtime;
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
    }
  }

  // Turns off the LED Lights following the 3rd Hit
  digitalWrite(led_lights, LOW);

  // Averages the Reaction Time Array
  int reaction_time = 0;
  for (int i = 0; i < 3; i++) {
    reaction_time += times[i];
  }

  Serial.println("Times is: ");
  Serial.print(times[0]);
  Serial.print(",");
  Serial.print(times[1]);
  Serial.print(",");
  Serial.println(times[2]);

  reaction_time = int(reaction_time / 3);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.clearDisplay();  // Displays the Reaction Time
  display.println("Reaction");
  display.println("Time");
  display.display();
  delay(1000);

  display.setTextSize(4);
  display.setCursor(10, 20);
  display.clearDisplay();
  display.println(reaction_time);
  display.display();
  delay(4000);

  mode_state = 0;  // Resets the Mode State

  // Resets Count to Zero so that it doesn't change the debounce timing
  foil_epee_saber_button.resetCount();
}

void loop() {

  target_button.loop();           // MUST call the loop() function first
  mode_button.loop();             // MUST call the loop() function first
  foil_epee_saber_button.loop();  // MUST call the loop() function first

  // if (target_button.isPressed())
  //   Serial.println("The button is pressed");

  // if (target_button.isPressed())
  //   Serial.println("The button is released");

  // mode_btnState = mode_button.getState();
  // target_btnState = mode_button.getState();

  // if ((target_button.isPressed()) and (!mode_state)) {
  if (((target_button.isReleased()) and (!mode_state) and (foil == true)) or ((target_button.isPressed()) and (!mode_state) and (foil == false))) {
    // if ((!target_btnState) and (!mode_state)) {
    display.setTextSize(2);
    Serial.println("The Target button is pressed");
    digitalWrite(led_lights, HIGH);  // turn the LED on (HIGH is the voltage level)
    display.clearDisplay();
    display.setCursor(48, 20);
    display.println("HIT");
    display.display();
    start_loop = millis();
  }

  // count = foil_epee_saber_button.getCount();

  //   if (foil_epee_saber_button.isPressed()){
  //     count += 1;
  //   }

  //   EVERY_N_MILLISECONDS(2000) {
  // count
  // }


  if ((foil_epee_saber_button.getCount() == 2) and (foil == true)) {
    display.setTextSize(3);
    target_debounce_delay = 2;
    target_button.setDebounceTime(target_debounce_delay);
    display.clearDisplay();
    display.setCursor(8, 2);
    display.println("Saber");
    display.setCursor(8, 36);
    display.println("Epee");
    display.display();
    foil = false;
    // count = 0;
    foil_epee_saber_button.resetCount();
  }

  if ((foil_epee_saber_button.getCount() == 2) and (foil == false)) {
    display.setTextSize(3);
    target_debounce_delay = 14;
    target_button.setDebounceTime(target_debounce_delay);
    display.clearDisplay();
    display.setCursor(12, 20);
    display.println("Foil");
    display.display();
    foil = true;
    // count = 0;
    foil_epee_saber_button.resetCount();
  }

  if (millis() > start_loop + 2000) {
    digitalWrite(led_lights, LOW);  // turn the LED on (HIGH is the voltage level)
    // display.clearDisplay();
    // display.display();
  }

  // if ((mode_button.isPressed()) and (!mode_state)) {
  if ((mode_button.isReleased()) and (!mode_state)) {
    // if ((!mode_btnState) and (!mode_state)) {
    Serial.println("The Mode button is pressed");
    mode_state = 1;  // Sets Mode to 1
    // timed_target();
  }

  if (mode_state) { timed_target(); }

  EVERY_N_MILLISECONDS(5000) {
    Serial.print("The Mode State is: ");
    Serial.println(mode_state);

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Target");
    display.setCursor(0, 30);
    display.println("Practice");
    display.display();
  }

  EVERY_N_MILLISECONDS(5000) {
    digitalWrite(led_lights, LOW);  // turn the LED off (LOW is the voltage level)
  }
}