#include <LiquidCrystal.h> // Imports the Liquid Crystal library
const int interruptPin = 2; // Sets a constant int called interruptPin equal to 2
int breakCount = 0; // Sets an int called breakCount equal to 0
unsigned long startTime = 0; // Sets an unsigned long called startTime equal to 0
unsigned long endTime = 0; // Sets an unsigned long called endTime equal to 0
int rpm = 0; // Sets an int called rpm to 0
int numberOfBlades = 4; // Sets an int called numberOfBlades equal to 4
int displaySetting = 1; // Sets an int called displaySetting equal to 1
const int MAX_DISPLAYS = 3; // Sets a constant int called MAX_DISPLAYS to 3
const int buttonPin = 3; // Sets a constant int called buttonPin to 3
unsigned long lastDisplaySwitch = millis(); // Sets an unsigned long called lastDisplaySwitch equal to the running time in milliseconds
const int DISPLAY_DELAY = 250; // Sets a constant int called DISPLAY_DELAY to 250
LiquidCrystal lcd(8,9,10,11,12,13); // Initializes a Liquid Crystal object with pin numbers as parameters
double n = 0, p_out = 0, p_in = 0, t_in = 0, n_in = 0, t_out_nm = 0, t_out_gcm= 0, n_out = 0, g_r = 0, nt_in = 0, nt_out = 0, n_motor = 0, p_inmotor = 0; // Sets a double with all named variables equal to 0

void setup() {
  pinMode(2, INPUT); // Sets pin number 2 to input data
  attachInterrupt(digitalPinToInterrupt(interruptPin), broken, FALLING); // Attaches an interrupt to the interruptPin to automate the pin that calls the broken method
  lcd.begin(16,2); // Starts the lcd screen using parameters width and height
  lcd.display(); // Displays the lcd screen
  pinMode(buttonPin, INPUT); // Sets the buttonPin to input data
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeDisplaySetting, FALLING); // Attaches an interrupt to the buttonPin to automate the pin that calls the changeDisplaySetting method
  Serial.begin(9600); // Starts the Serial
}

void loop() {
  if (breakCount >= 30){ // Checks to see if breakCount is greater than or equal to 30
    endTime = millis(); // Sets endTime equal to the current milliseconds
    double x = (double)breakCount/numberOfBlades; // Sets x equal to breakCount divided by numberOfBlades as a double
    double y = (double)(endTime-startTime)/60000; // Sets y equal to quantity endTime minus startTime divided by 60000 as a double
    rpm = x/y; // Equates and sets the rpm as an integer of the ratio of x and y
    breakCount = 0; // Resets the breakCount to 0
  }
  delay(1000); // Delays the program by 1000 milliseconds
  displayLCD(); // Runs the local displayLCD method
  n_in = rpm; // Sets n_in to the rpm from the tachometer
  nt_in = 64; // Sets nt_in to the # of gear teeth on the DC motor input shaft
  nt_out = 8; // Sets nt_out to the # of gear teeth on the windmill output shaft
  g_r = nt_in / nt_out; // Equates the gear ratio as the ratio of nt_in to nt_out
  n_out = g_r * n_in; // Equates the efficiency as the product of g_r and n_in
  t_out_gcm = (n_out-12800)/-40; // Equates the torque output using the linear relationship
  t_out_nm = t_out_gcm * 0.0000980655; // Converts the torque output to N*m
  t_in = t_out_nm * n_out / n_in; // Equates the torque input from the product relationship
  p_in = t_in * n_in / 9549 * 1000; // Equates the power input in Watts
  if (t_out_gcm <= 48){ // Checks to see if t_out_gcm is less than or equal to 48
    n_motor = (-0.001618 * t_out_gcm * t_out_gcm + 0.363542 * t_out_gcm + 49.376500)/100; // Equates the efficiency of the motor using the relationship curve of t_out and n_motor
  }else{ // Checks to see if t_out_gcm is greater than 48
    n_motor = (-0.000434 * t_out_gcm * t_out_gcm - 0.072269 * t_out_gcm + 67.566800)/100; // Equates the efficiency of the motor using the relationship curve of t_out and n_motor
  }
  p_inmotor = -0.000428 * t_out_gcm * t_out_gcm + 0.136943 * t_out_gcm + 0.000739; // Equates the power of the input motor using the relationship curve of t_out and p_inmotor
  p_out = n_motor * p_inmotor; // Equates the power output using the product of n_motor and p_inmotor
  n = p_out / p_in; // Equates the efficiency using the ratio of p_out to p_in as a decimal
}

void broken(){
  if (breakCount == 0){ // Checks to see if breakCount is equal to 0
    startTime = millis(); // Sets start time equal to the current milliseconds
  }
  breakCount++; // Adds 1 to the breakCount
}

void displayLCD(){
  switch (displaySetting){ // Creates a switch statement for the displaySetting
    case 1: // Checks if displaySetting is 1
      lcd.clear(); // Clears the LCD
      lcd.print("RPM: "); // Prints "RPM: " to the lcd
      lcd.print(rpm); // Prints the rpm to the lcd
      break; // Breaks the switch statement
    case 2: // Checks if displaySetting is 2
      lcd.clear(); // Clears the LCD
      lcd.print("P_out: "); // Prints "P_out: " to the lcd
      lcd.print(p_out); // Prints the power out to the lcd
      lcd.print("W"); // Prints "W" to the lcd
      break; // Breaks the switch statement
    case 3: // Checks if displaySetting is 3
      lcd.clear(); // Clears the LCD
      lcd.print("N: "); // Prints "N: " to the lcd
      lcd.print(n*100); // Prints the efficiency to the lcd
      lcd.print("%");
      break; // Breaks the switch statement
    default: // Checks if displaySetting is none of the above
      lcd.clear(); // Clears the LCD
      lcd.print("Unknown setting"); // Prints "Unknown Setting" to the lcd
  }
}

void changeDisplaySetting(){
  if (lastDisplaySwitch + DISPLAY_DELAY < millis()){ // Checks if lastDisplaySwitch plus DisplayDelay is less that the current milliseconds
    lastDisplaySwitch = millis(); // Sets lastDisplaySwitch equal to the current milliseconds
    Serial.println("Switching display"); // Prints "Switching display" to the Serial monitor
    displaySetting++; // Adds 1 to displaySetting
    if (displaySetting > MAX_DISPLAYS){ // Checks if displaySetting is greater than MAX_DISPLAYS
      displaySetting = 1; // Sets displaySetting equal to 1
    }
    displayLCD(); // Runs the local displayLCD method
  }
}