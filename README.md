This Arduino sketch is designed to display various Bitcoin network statistics on a TFT display. The sketch uses the TFT_eSPI, SPI, HTTPClient, and ArduinoJson libraries. The sketch is designed to be used with an ESP32 microcontroller.

Hardware requirements
---------------------

-   ESP32 microcontroller
-   TFT display
-   Two buttons connected to pins 37 and 38

Libraries used
--------------

-   TFT_eSPI - Used to control the TFT display
-   SPI - Used for communication with the TFT display
-   HTTPClient - Used for making HTTP requests to the Bitcoin API
-   ArduinoJson - Used for parsing JSON responses from the Bitcoin API

Installation
------------

1.  Install the required libraries using the Arduino library manager.
2.  Download and open the sketch in the Arduino IDE.
3.  Upload the sketch to the ESP32 microcontroller.

Usage
-----

The sketch displays four different Bitcoin network statistics pages that can be cycled through using the two buttons connected to pins 37 and 38. The four pages are:

-   Suggested fees
-   Lightning network statistics
-   Current block height
-   Halving info

The suggested fees page displays the suggested Bitcoin transaction fees in satoshis per virtual byte (sats/vB) for the fastest, half-hour, and hour confirmation times. The page also includes a graph displaying the suggested fees for each confirmation time.

The lightning network statistics page displays the number of channels, number of nodes, total capacity, and median fee rate for the Lightning Network.

The current block height page displays the current block height of the Bitcoin blockchain.

The halving info page displays the number of blocks until the next Bitcoin halving event, which is an event that occurs approximately every four years in which the block reward for Bitcoin miners is cut in half.

Limitations
-----------

-   The sketch only displays information for the Bitcoin network and is not designed to display information for other cryptocurrencies.
-   The sketch relies on the availability and accuracy of the Bitcoin API being used. If the API is unavailable or returns inaccurate information, the information displayed on the TFT display may be incorrect or outdated.
-   The sketch is designed to be used with an ESP32 microcontroller and may not be compatible with other microcontrollers.
-   The sketch may require modifications to the pin numbers used for the buttons depending on the specific ESP32 development board being used.
