# Bitcoin Network Data Display

This code is an Arduino sketch that uses the ESP32 board with the TFT_eSPI, HTTPClient, and ArduinoJson libraries to retrieve and display Bitcoin network data.

## Functionality

The program displays three pages of information that can be cycled through using two buttons connected to pins 37 and 38. On the first page, the program retrieves the suggested transaction fees for the Bitcoin network from the mempool.space API and displays them on the TFT display. On the second page, the program retrieves statistics about the Lightning Network from the same API and displays them on the TFT display. On the third page, the program retrieves the current block height from the same API and displays it on the TFT display.

## Libraries Used

The code uses the TFT_eSPI library to display text on the screen, the HTTPClient library to make HTTP requests to the API, and the ArduinoJson library to parse the JSON response. 

## Constants

The program defines constants for color schemes and URLs for the API requests.
