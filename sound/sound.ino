/*
  SineWave

  Generates a pre-generated sawtooth-waveform.

  See the full documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/dac
*/

#include "analogWave.h" // Include the library for analog waveform generation

analogWave wave(DAC);   // Create an instance of the analogWave class, using the DAC pin

int freq = 600;  // in hertz, change accordingly

void setup() {
  Serial.begin(115200);  // Initialize serial communication at a baud rate of 115200
  wave.sine(freq);       // Generate a sine wave with the initial frequency
}

void loop() {

  wave.freq(freq);
  delay(1000);
  Serial.println(freq), DE; 

}
