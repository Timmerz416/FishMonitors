// FishThermostat
//////////////////////////////////////////////////////////////////////
// A simple controller for the fish thermostat

// Includes
#include <LiquidCrystal.h>
#include <OneWire.h>

// LCD Controller
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
boolean update = false;

// Theromostat control variables
const int dialInputPIN = A0;
const float volt_range = 5.0;
const float min_temp = 22.0;
const float max_temp = 28.0;
const float alpha = (max_temp - min_temp)/volt_range;
float temp_setting = 0.0;

// Temperature Sensor variables
const int tempPIN = 6;
float measured_temp = 20.0;
OneWire probe(tempPIN);

// Heater variables
const int heaterPIN = 7;
const float tolerance = 0.1;

void setup() {
    // Turn off onboard LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    
    // Setup heater control pin
    pinMode(heaterPIN, OUTPUT);
    
    // Start the LCD screen
    lcd.begin(16, 2);
}

void loop() {
    // Read the thermostat setting
    float scale = 5.0*analogRead(dialInputPIN)/1023.0;
    float new_temp_setting = min_temp + alpha*scale;
    
    if(new_temp_setting != temp_setting) {
        // Update temperature setting and indicate refresh
        temp_setting = new_temp_setting;
        update = true;
    }
    
    // Get the temperature reading
    float cur_temp = getTemp();
    if(cur_temp == -1000) digitalWrite(13, HIGH);
    else digitalWrite(13, LOW);
    
    if(cur_temp != measured_temp) {
        measured_temp = cur_temp;
        update = true;
    }
    
    // Evaluate if the heater is on/off
    if(measured_temp > (temp_setting + tolerance)) digitalWrite(heaterPIN, LOW);
    if(measured_temp < (temp_setting - tolerance)) digitalWrite(heaterPIN, HIGH);
    
    // Update display
    if(update) UpdateLCD();
    
    delay(50);
}

void UpdateLCD() {
    // Write the temperature setting to the screen
    lcd.setCursor(0,0);
    lcd.print("Set Temp:   ");
    lcd.print(temp_setting);
    
    // Write the measured temperature to the screen
    lcd.setCursor(0,1);
    lcd.print("Meas Temp:  ");
    lcd.print(measured_temp);
    
    update = false;
}

float getTemp(){
     //returns the temperature from one DS18S20 in DEG Celsius
     byte data[12];
     byte addr[8];
    
     if ( !probe.search(addr)) {
         // no more sensors on chain, reset search
         probe.reset_search();
         return -9.0;
     }
    
     if ( OneWire::crc8( addr, 7) != addr[7]) {
         return -9.1;
     }
    
     if ( addr[0] != 0x10 && addr[0] != 0x28) {
         return -9.2;
     }
    
     probe.reset();
     probe.select(addr);
     probe.write(0x44,1); // start conversion, with parasite power on at the end
    
     byte present = probe.reset();
     probe.select(addr);  
     probe.write(0xBE); // Read Scratchpad
    
     
     for (int i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = probe.read();
     }
     
     probe.reset_search();
     
     byte MSB = data[1];
     byte LSB = data[0];
    
     float tempRead = ((MSB << 8) | LSB); //using two's compliment
     float TemperatureSum = tempRead / 16;
     
     return TemperatureSum;
}
