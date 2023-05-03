# TTGO Bitcoin MemPool Monitor

This is a simple Bitcoin mempool monitor for the TTGO T-Display ESP32. It displays the current state of the Bitcoin mempool, including the number of unconfirmed transactions, the estimated transaction fee rates, the current block height, and a line chart showing the suggested fee rate over the past 24 hours.

## Installation

1. Clone this repository or download the ZIP file and extract it to your preferred location.
2. Open the `sketch.ino` file in the Arduino IDE.
3. Connect your TTGO T-Display to your computer using a USB cable.
4. Select your board and serial port in the Arduino IDE.
5. Upload the sketch to your board.

## Usage

After uploading the sketch to your board, the TTGO T-Display should display the current state of the Bitcoin mempool. You can use the two buttons on the side of the device to cycle through the different display modes.

### Display Modes


- **Fees**: Displays the current suggested fee rates for Bitcoin transactions, based on their estimated confirmation time.
- **LN Stats**: Display information about the lightning network like channel count, clear and tor node count, network capacity and median fee rate.
- **Blocks**: Displays the current block height of the Bitcoin blockchain.
- **Price**: Displays the current BTC price in USD, GBT, EUR.
- **Tx Vol**: Displays a line chart showing the number of transactions confirmed in each block over the previous 6 blocks. Trends denoted by color.
- **LN Growth**: Displays information showing the growth of Lightning Network capacity over 6 months. Lines for node types overlayed on bars showing capacity.
- **nodeBuddy**: Display LN+ stats about a node you specify by pubID.


![image](https://user-images.githubusercontent.com/25254103/235862178-d5e5c0a2-8d42-447d-ad94-d380e95d8a4f.png)
![image](https://user-images.githubusercontent.com/25254103/235862247-6aa55472-a04e-4a63-b147-7e0e72860682.png)
![image](https://user-images.githubusercontent.com/25254103/235862272-3a296325-cce6-4c10-ae14-6002987c92aa.png)
![image](https://user-images.githubusercontent.com/25254103/235862301-9e5f9909-933c-45d1-bb15-eeb52d7269c4.png)
![image](https://user-images.githubusercontent.com/25254103/235862329-c4de1ba5-1aad-4c68-9943-5bd8ae2bba40.png)
![image](https://user-images.githubusercontent.com/25254103/235862358-bb9e75d7-ff05-46bf-b34c-f3e1140e8c37.png)
![image](https://user-images.githubusercontent.com/25254103/235862378-3a433f51-49e3-40ea-88ad-d2c522b28c1a.png)





### Button Functions

- **Button 1**: Cycles +1 through the different display modes.
- **Button 2**: Cycles -1 through the different display modes

## Contributing

If you would like to contribute to this project, please feel free to submit a pull request or open an issue.

## Credits

This project uses the following libraries:

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) by Bodmer
- [HTTPClient](https://github.com/amcewen/HttpClient) by Adrian McEwen
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoit Blanchon

The line charts are generated using the [TFT_eSPI_Graph library](https://github.com/Bodmer/TFT_eSPI_Graph) by Bodmer.

This Project uses the following APIs:
- [mempool.space api](https://mempool.space/api)
- [coindesk BPI api](https://api.coindesk.com/v1/bpi/currentprice.json)
- [LN+ api](https://lightningnetwork.plus/api_documentation)

