#include <Keypad.h>
#include "src/CustomArduino.h"
#include "src/Audio.h"
#include "SD.h"
#include "FS.h"

// Define I/O Pins for microSD card and I2S audio interface
#define SD_CS 18          // Chip Select pin for the microSD card
#define SPI_MOSI 19       // SPI MOSI pin
#define SPI_MISO 23       // SPI MISO pin
#define SPI_SCK 5         // SPI Clock pin

#define I2S_DOUT 25       // I2S Data Out pin
#define I2S_BCLK 27       // I2S Bit Clock pin
#define I2S_LRC 26        // I2S Left/Right Clock pin

#define LED_BUILTIN 2     // Built-in LED pin
#define POTENTIOMETER 35  // Potentiometer analog input pin

Audio audio;              // Audio object for managing playback

int potentioValue;        // Variable to store the raw potentiometer value
int mappedValue;          // Variable to store the mapped volume value

// Keypad configuration
const uint8_t ROWS = 4;   // Number of rows on the keypad
const uint8_t COLS = 4;   // Number of columns on the keypad

/* Define the symbols on the buttons of the keypad */
char hexaKeys[ROWS][COLS] = {
  { 'F', 'B', '4', '3' },
  { 'E', 'A', '5', '2' },
  { 'D', '9', '6', '1' },
  { 'C', '8', '7', '0' }
};

uint8_t rowPins[ROWS] = { 12, 13, 14, 15 }; // Row pin connections for the keypad
uint8_t colPins[COLS] = { 2, 3, 21, 22 };  // Column pin connections for the keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); // Keypad object

void setup() {
  // Set up the microSD Card Chip Select (CS) pin as OUTPUT and set it HIGH
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Set up the built-in LED pin as OUTPUT and turn it on initially
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize the SPI bus for microSD card communication
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  // Start Serial communication for debugging
  Serial.begin(115200);

  // Initialize the microSD card
  if (!SD.begin(SD_CS)) {
    Serial.println("Error accessing microSD card!"); // Print error if initialization fails
  } else {
    Serial.println("Successfully accessed microSD card!"); // Print success message
  }

  // Set up I2S audio interface
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Read the initial potentiometer value and map it to the audio volume range
  potentioValue = analogRead(POTENTIOMETER);
  mappedValue = map(potentioValue, 0, 4095, 0, 21); // Map raw value to volume range (0-21)
  audio.setVolume(mappedValue); // Set initial volume
}

void loop() {
  // Continuously read the potentiometer value and adjust the audio volume
  potentioValue = analogRead(POTENTIOMETER);
  mappedValue = map(potentioValue, 0, 4095, 0, 21); // Map raw value to volume range (0-21)
  audio.setVolume(mappedValue); // Adjust volume based on potentiometer

  // Read the keypad input
  char customKey = customKeypad.getKey();

  if (customKey) {  // Check if a key is pressed
    Serial.println(customKey); // Print the pressed key to the Serial monitor

    // Generate the filename based on the pressed key
    String filename = String(customKey) + ".wav";

    // Stop any currently playing audio
    audio.stopSong();
    Serial.println("Stopped current playback."); // Debug message for stopping audio

    // Attempt to play the new file from the SD card
    const char* filePath = filename.c_str();
    if (audio.connecttoFS(SD, filePath)) {
      Serial.println("Playing file: " + filename); // Print success message
      digitalWrite(LED_BUILTIN, HIGH); // Turn on the LED during playback
    } else {
      Serial.println("Failed to play file: " + filename); // Print error message
      digitalWrite(LED_BUILTIN, LOW); // Turn off the LED if playback fails
    }
  }

  // Process audio playback in the background
  audio.loop();
}