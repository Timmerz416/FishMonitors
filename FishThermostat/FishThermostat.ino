// FishThermostat
//////////////////////////////////////////////////////////////////////
// A simple controller for the fish thermostat

// Includes
#include <LiquidCrystal.h>

// LCD Controller
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Theromostat control variables
const int dialInputPIN = A0;
const float volt_range = 5.0;
const float min_temp = 22.0;
const float max_temp = 28.0;
const float alpha = (max_temp - min_temp)/volt_range;
float temp_setting = 0.0;

void setup() {
    // Turn off onboard LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    
    // Start the LCD screen
    lcd.begin(16, 2);
}

void loop() {
    // Read the thermostat setting
    float scale = 5.0*analogRead(dialInputPIN)/1023.0;
    float new_temp_setting = min_temp + alpha*scale;
    
    if(new_temp_setting != temp_setting) {
        // Update temperature setting
        temp_setting = new_temp_setting;
        
        // Write the temperature to the screen
        lcd.setCursor(0,1);
        lcd.print("Set Temp:   ");
        lcd.print(temp_setting);
    }
    
    delay(50);
}

