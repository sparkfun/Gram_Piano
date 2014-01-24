/*
  The Gram Piano - A Capactive Touch Keyboard
  Created 1/24/13 by Jordan McConnell at SparkFun Electronics
  This code is beerware: feel free to use it, with or without attribution, in your own projects.
  Created with Arduino 1.5.2
  
  This sketch allows users to play an octave of notes on the Gram Piano board
  similar to a normal musical keyboard. The led will flash when the board is
  ready to be played. You can select between three octaves using the potentiometer.
  You can also play/stop a melody by pressing the button. 
*/

// Libraries for capactive sensing, note frequencies, and AVR registers/pins
#include <CapacitiveSensor.h>
#include "pitches.h"
#include <Arduino.h>

// Pins for LED, potentiometer, and speaker
const int led = A4;
const int pot = A3;
const int spkr = 5;

// Keyboard variables
long keys[13]; // Contains latest capacitive sense reading for each key
int threshold = 25; // Threshold for key press readings, a key plays sound if its equal to or greater than the threshold
float octave = 1.0; // Stores what octave/multiplier the key presses use, change with potentiometer

// Declaring a capactive sensor for each key on the keyboard
CapacitiveSensor CapSensors[13] =
{
  CapacitiveSensor(2,3),
  CapacitiveSensor(2,4),
  CapacitiveSensor(2,6),
  CapacitiveSensor(2,7),
  CapacitiveSensor(2,8),
  CapacitiveSensor(2,9),
  CapacitiveSensor(2,10),
  CapacitiveSensor(2,11),
  CapacitiveSensor(2,12),
  CapacitiveSensor(2,13),
  CapacitiveSensor(2,A0),
  CapacitiveSensor(2,A1),
  CapacitiveSensor(2,A2)
};

// An array of notes for the Intro to Sweet Child O'Mine
// These are played sequentially with a button press
int notes[48] = 
{
  NOTE_D4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4,
  NOTE_D4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4,
  NOTE_E4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4,
  NOTE_E4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4,
  NOTE_G4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4,
  NOTE_G4, NOTE_D5, NOTE_A4, NOTE_G4, NOTE_G5, NOTE_A4, NOTE_FS5, NOTE_A4
};

void setup()                    
{  
  // Setup button pin (PB6) as input (not a default Arduino pin)
  DDRB &= ~(1 << 6); // Set bit six of the data direction register b to 0
  
  // Blink LED to indicate keyboard is ready for use
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
}

void loop()                    
{
  // Measure each key press using capacitive sensing
  measureKeys();
    
  // Select one of three octaves based on position of potentiometer
  octave = readPot();
  
  // Play the note corresponding to the key press and octave
  // Higher pitch keys have priority if there were multiple presses
  playKeyPress();
  
  // Play melody and turn on LED if the button has been pressed
  if (buttonPressed())
  {
    digitalWrite(led,HIGH);
    playMelody();
  }
}

// Measure each key press using capacitive sensing
void measureKeys()
{
  for (int i = 0; i < 13; i++)
  {
    // Set sense pin, pin 2 to low for a bit to stabilize between readings
    pinMode(2,OUTPUT);
    digitalWrite(2,LOW);
    delayMicroseconds(300);
    
    // Read each capacitive sensor and plug value in to keys array
    keys[i] = CapSensors[i].capacitiveSensor(5); // 5 samples per key
  }
}

// Reads potentiometer and returns one of three octaves for the tones to use
float readPot()
{
  // Stores value read in from the potentiometer,  0-1023
  int potVal = analogRead(pot);
  
  // Select one of three octaves based on position of potentiometer
  if (potVal > 682) // 2/3 * 1023 = 682
  {
    return 2.0; // Upper third potentiometer position, high octave
  }
  else if (potVal > 341)
  {
    return 1.0; // Middle third potentiometer position, mid octave
  }
  else
  {
    return .5; // Lower third potentiometer position, low octave
  }
}

// Play the note corresponding to the key press and octave
// Higher pitch keys have priority if there were multiple presses
void playKeyPress()
{
  // keys[0] is left most key, keys[12] is right most key
  // If a key press was recorded to be higher than the threshold,
  // that key will be played. Higher pitch keys have priority.
  if (keys[12] > threshold) { tone(spkr,NOTE_C5 * octave,30); }
  else if (keys[11] > threshold) { tone(spkr,NOTE_B4 * octave,30); }
  else if (keys[10] > threshold) { tone(spkr,NOTE_AS4 * octave,30); }
  else if (keys[9] > threshold) { tone(spkr,NOTE_A4 * octave,30); }
  else if (keys[8] > threshold) { tone(spkr,NOTE_GS4 * octave,30); }    
  else if (keys[7] > threshold) { tone(spkr,NOTE_G4 * octave,30); }
  else if (keys[6] > threshold) { tone(spkr,NOTE_FS4 * octave,30); }
  else if (keys[5] > threshold) { tone(spkr,NOTE_F4 * octave,30); }
  else if (keys[4] > threshold) { tone(spkr,NOTE_E4 * octave,30); }    
  else if (keys[3] > threshold) { tone(spkr,NOTE_DS4 * octave,30); }
  else if (keys[2] > threshold) { tone(spkr,NOTE_D4 * octave,30); }
  else if (keys[1] > threshold) { tone(spkr,NOTE_CS4 * octave,30); }
  else if (keys[0] > threshold) { tone(spkr,NOTE_C4 * octave,30); }
}

// Checks if the button (on AVR pin PB6) has been pressed.
// Returns true if so
boolean buttonPressed()
{
  if (PINB & (1 << 6))
  {
    return false;
  }
  else
  {
    return true;
  }
}

// Plays the sequence of notes defined by the 'notes' array
void playMelody()
{
  for (int i = 0; i < 48; i++)
  {
    // Play each note in sequence for 300 ms
    tone(spkr,(float)notes[i] * octave,300);
    delay(300);
    
    // End melody if button is pressed during play
    if (buttonPressed())
    { 
      noTone(spkr); // Turn off sound
      i = 48; // End loop
      digitalWrite(led,LOW); // Turn off LED to indicate melody has stopped
      delay(1000); // Prevent restarting the melody accidentally
    }
  }
  digitalWrite(led,LOW); // Turn off LED to indicate melody has stopped
}
