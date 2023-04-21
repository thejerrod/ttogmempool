# TTGO Bitcoin MemPool Monitor

This is a simple Bitcoin mempool monitor for the TTGO T-Display ESP32. It displays the current state of the Bitcoin mempool, including the number of unconfirmed transactions, the estimated transaction fee rates, the current block height, and a line chart showing the suggested fee rate over the past 24 hours.

## Installation

1. Clone this repository or download the ZIP file and extract it to your preferred location.
2. Open the `ttgomempool.ino` file in the Arduino IDE.
3. Connect your TTGO T-Display to your computer using a USB cable.
4. Select your board and serial port in the Arduino IDE.
5. Upload the sketch to your board.

## Usage

After uploading the sketch to your board, the TTGO T-Display should display the current state of the Bitcoin mempool. You can use the two buttons on the side of the device to cycle through the different display modes.

### Display Modes

- **Transaction Count**: Displays the current number of unconfirmed transactions in the Bitcoin mempool.
- **Suggested Fee Rates**: Displays the current suggested fee rates for Bitcoin transactions, based on their estimated confirmation time.
- **Current Block**: Displays the current block height of the Bitcoin blockchain.
- **Fee Rate Over Time**: Displays a line chart showing the suggested fee rate over the past 24 hours.
- **Transaction Volume Over Time**: Displays a line chart showing the number of transactions confirmed in each block over the past 24 hours.
- **Capacity Growth**: Displays a line chart showing the growth of Lightning Network capacity over time.

### Button Functions

- **Button 1**: Cycles through the different display modes.
- **Button 2**: Toggles the display of the chart legend in the "Fee Rate Over Time", "Transaction Volume Over Time", and "Capacity Growth" display modes.

## Contributing

If you would like to contribute to this project, please feel free to submit a pull request or open an issue.

## Credits

This project uses the following libraries:

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) by Bodmer
- [HTTPClient](https://github.com/amcewen/HttpClient) by Adrian McEwen
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoit Blanchon

The line charts are generated using the [TFT_eSPI_Graph library](https://github.com/Bodmer/TFT_eSPI_Graph) by Bodmer.
