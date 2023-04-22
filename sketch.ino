#include <TFT_eSPI.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define BUTTON_1 35  // Button 1 pin
#define BUTTON_2 0  // Button 2 pin

// Initialize the TFT display
TFT_eSPI tft = TFT_eSPI();

// Color scheme
#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define ACCENT_COLOR TFT_CYAN
#define FASTEST_COLOR TFT_RED
#define HALF_HOUR_COLOR TFT_ORANGE
#define HOUR_COLOR TFT_GREEN



// API URLs
const char* mempool_api_url = "https://mempool.space/api/v1/fees/recommended";
const char* lightning_api_url = "https://mempool.space/api/v1/lightning/statistics/latest";
const char* block_api_url = "https://mempool.space/api/blocks/";
const char* bitcoin_price_api_url = "https://api.coindesk.com/v1/bpi/currentprice.json"; 

// Default suggested fees
int fastestFee = 0;
int halfHourFee = 0;
int hourFee = 0;

// Lightning network statistics
int channel_count = 0;
int node_count = 0;
int total_capacity = 0;
int med_fee_rate = 0;

// Current block height
int current_block = 0;

// Button variables
int button1State;
int button2State;
unsigned long lastButtonPress = 0;
int currentDisplay = 0;
int maxDisplay = 4;

void setup() {
  // Initialize the TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  
  // Initialize the buttons
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
}

void loop() {
  // Get the current time
  unsigned long currentTime = millis();

  // Check if button 1 was pressed
  button1State = digitalRead(BUTTON_1);
  if (button1State == LOW && currentTime - lastButtonPress > 500) {
    currentDisplay--;
    if (currentDisplay < 0) {
      currentDisplay = maxDisplay;
    }
    lastButtonPress = currentTime;
  }

  // Check if button 2 was pressed
  button2State = digitalRead(BUTTON_2);
  if (button2State == LOW && currentTime - lastButtonPress > 500) {
    currentDisplay++;
    if (currentDisplay > maxDisplay) {
      currentDisplay = 0;
    }
    lastButtonPress = currentTime;
  }

  // Display the current page
  switch (currentDisplay) {
    case 0:
      // Display the suggested fees on the TFT display using the TFT_eSPI library
      displaySuggestedFees();
      break;

    case 1:
      // Display the lightning network statistics on the TFT display using the TFT_eSPI library
      displayLightningStats();
      break;

    case 2:
      // Display the current block height on the TFT display using the TFT_eSPI library
      displayBlockHeight();
      break;

    case 3:
      // Disply the current bitcoin price index
      displayBitcoinPrice();
      break;

    case 4:
      // Display transaction volume over time
      displayTransactionVolumeOverTime();
      break;

    case 5:
    // Display capacity growth of the lightning network over time
    displayCapacityGrowth();
    break;

    case 6:
    // Display bitcoin price
    displayBitcoinPrice();
    break;
  }
}

void displaySuggestedFees() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request
  HTTPClient http;
  http.begin(mempool_api_url);
  int httpCode = http.GET();

  // If the request was successful, parse the JSON response
  if (httpCode == HTTP_CODE_OK) {
    String jsonStr = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonStr);

    fastestFee = doc["fastestFeerate"];
    halfHourFee = doc["halfHourFeerate"];
    hourFee = doc["hourFeerate"];

    // Display the suggested fees on the TFT display
    tft.setCursor(0, 0);
    tft.print("Suggested Fees (sats/vB)\n");
    tft.setTextColor(ACCENT_COLOR);
    tft.print("Fastest: ");
    tft.setTextColor(TEXT_COLOR);
    tft.print(fastestFee);
    tft.print(" sats/vB");
    tft.print("\nHalf Hour: ");
    tft.setTextColor(TEXT_COLOR);
    tft.print(halfHourFee);
    tft.print(" sats/vB");
    tft.print("\nHour: ");
    tft.setTextColor(TEXT_COLOR);
    tft.print(hourFee);
    tft.print(" sats/vB");

    // Display a graph of the suggested fees
    int graphWidth = 200;
    int graphHeight = 100;
    int graphX = (tft.width() - graphWidth) / 2;
    int graphY = tft.height() - graphHeight - 20;
    tft.drawRect(graphX, graphY, graphWidth, graphHeight, ACCENT_COLOR);
    int fastestBarHeight = map(fastestFee, 0, 300, 0, graphHeight);
    int halfHourBarHeight = map(halfHourFee, 0, 300, 0, graphHeight);
    int hourBarHeight = map(hourFee, 0, 300, 0, graphHeight);
    tft.fillRect(graphX + 10, graphY + graphHeight - fastestBarHeight, 20, fastestBarHeight, FASTEST_COLOR);
    tft.fillRect(graphX + 70, graphY + graphHeight - halfHourBarHeight, 20, halfHourBarHeight, HALF_HOUR_COLOR);
    tft.fillRect(graphX + 130, graphY + graphHeight - hourBarHeight, 20, hourBarHeight, HOUR_COLOR);
  } else {
    // If the request failed, display an error message on the TFT display
    tft.setCursor(0, 0);
    tft.print("Error: Failed to retrieve suggested fees.");
  }

  // End the HTTP connection
  http.end();
}

void displayTransactionVolumeOverTime() {
  // Initialize HTTP client and JSON document
  HTTPClient http;
  DynamicJsonDocument doc(2048);

  // Send request to mempool.space API
    http.begin(block_api_url);
  int httpCode = http.GET();

  // If request was successful, parse JSON data
  if (httpCode == HTTP_CODE_OK) {
    String jsonStr = http.getString();
    deserializeJson(doc, jsonStr);

    // Extract data for each block
    int numBlocks = doc.size();
    int maxTxs = 0;
    float txsPerBlock[numBlocks];
    for (int i = 0; i < numBlocks; i++) {
      int txCount = doc[i]["tx_count"];
      txsPerBlock[i] = (float)txCount;
      if (txCount > maxTxs) {
        maxTxs = txCount;
      }
    }

    // Initialize variables for drawing the chart
    int chartX = 0;
    int chartY = 40;
    int chartW = tft.width();
    int chartH = tft.height() - chartY - 10;
    int chartPadding = 10;
    int numPoints = numBlocks;
    float xStep = (float)(chartW - 2*chartPadding) / (float)(numPoints - 1);
    float yScale = (float)(chartH - 2*chartPadding) / (float)maxTxs;

    // Draw chart axis and labels
    tft.fillRect(0, chartY, tft.width(), chartH, TFT_BLACK);
    tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_WHITE);
    tft.drawFastVLine(chartPadding, chartY + chartH - chartPadding, -chartH + 2*chartPadding, TFT_WHITE);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(chartPadding, chartY + chartH - chartPadding + 5);
    tft.print("Time");
    tft.setCursor(5, chartY + chartH/2);
    tft.print("Num Transactions");
    tft.drawFastVLine(chartPadding, chartY + chartPadding, chartH - 2*chartPadding, TFT_WHITE);
    tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_WHITE);

    // Draw chart line
    tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_YELLOW);
    for (int i = 0; i < numPoints-1; i++) {
      float x1 = chartPadding + i*xStep;
      float x2 = chartPadding + (i+1)*xStep;
      float y1 = chartY + chartH - chartPadding - yScale*txsPerBlock[i];
      float y2 = chartY + chartH - chartPadding - yScale*txsPerBlock[i+1];
      tft.drawLine(x1, y1, x2, y2, TFT_GREEN);
    }
  }

  // Close HTTP connection
  http.end();
}

void displayLightningStats() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request
  HTTPClient http;
  http.begin(lightning_api_url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    // Parse the JSON response
    String payload = http.getString();
    const size_t capacity = JSON_OBJECT_SIZE(4) + 70;
    StaticJsonDocument<capacity> doc;
    DeserializationError error = deserializeJson(doc, payload);

    // If parsing the JSON was successful
    if (!error) {
      // Extract the relevant information from the JSON response
      channel_count = doc["channels"];
      node_count = doc["nodes"];
      total_capacity = doc["total_capacity"];
      med_fee_rate = doc["median_fee_sat_per_byte"];

      // Display the Lightning Network statistics on the TFT display
      tft.setCursor(0, 0);
      tft.println("Lightning Network Statistics");
      tft.setTextColor(ACCENT_COLOR);
      tft.println("_______________________________");
      tft.setTextColor(TEXT_COLOR);
      tft.print("Channels: ");
      tft.print(channel_count);
      tft.println();
      tft.print("Nodes: ");
      tft.print(node_count);
      tft.println();
      tft.print("Total Capacity: ");
      tft.print(total_capacity);
      tft.println(" sats");
      tft.print("Median Fee Rate: ");
      tft.print(med_fee_rate);
      tft.println(" sats/byte");
      
    } 
  } else {
    // If the API request failed, display an error message
    tft.println("Error: Failed to retrieve Lightning Network statistics");
  }

  // Clean up
  http.end();
}

void displayCapacityGrowth() {
  // Make an HTTP request to the statistics endpoint
  HTTPClient http;
  http.begin(lightning_api_url);
  int httpCode = http.GET();
  if (httpCode != 200) {
    tft.println("HTTP request failed");
    return;
  }
  
  // Parse the JSON response
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, http.getString());
  http.end();
  
  // Get the capacity data and create a graph
  int numPoints = doc["capacity"]["history"].size();
  int xIncrement = 240 / numPoints;
  int x = 0;
  int yPrev = 0;
  for (int i = 0; i < numPoints; i++) {
    int y = doc["capacity"]["history"][i]["total_capacity"].as<int>() / 1000000000;
    tft.drawLine(x, 120 - yPrev, x + xIncrement, 120 - y, TFT_WHITE);
    x += xIncrement;
    yPrev = y;
  }
  
  // Add labels and title to the graph
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Capacity Growth", 5, 15, 2);
  tft.drawString("Time (latest to oldest)", 80, 110);
  tft.drawString("Capacity (in billions of sats)", 0, 50, 90);
}



void displayBlockHeight() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request
  HTTPClient http;
 http.begin(block_api_url);
  int httpCode = http.GET();

  // Check if the request was successful
  if (httpCode == 200) {
    // Parse the JSON response
    String response = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    // Get the current block height from the response
    int current_block = doc[0]["height"];

    // Calculate blocks and time until halving
    int blocks_since_last_halving = current_block % 210000;
    int blocks_until_halving = 210000 - blocks_since_last_halving;
    int time_until_halving = blocks_until_halving * 10;

    // Display the current block height and some additional information on the TFT display
    tft.setCursor(0, 0);
    tft.println("Current Block Height");
    tft.println();
    tft.print("Block Height: ");
    tft.println(current_block);
    tft.print("Blocks Until Halving: ");
    tft.println(blocks_until_halving);
    tft.print("Time Until Halving: ");
    tft.print(time_until_halving / 60);
    tft.print("m ");
    tft.print(time_until_halving % 60);
    tft.print("s");
  }
  else {
    // Display an error message on the TFT display
    tft.setCursor(0, 50);
    tft.println("Error: Unable to retrieve block height");
  }

  // End the HTTP connection
  http.end();
}

void displayBitcoinPrice() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request
  HTTPClient http;
  http.begin(bitcoin_price_api_url);
  int httpCode = http.GET();

  // Check if the request was successful
  if (httpCode == 200) {
    // Parse the JSON response
    String response = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    
    // Get the current bitcoin price from the response
    float bitcoin_price = doc["bpi"]["USD"]["rate_float"];

    // Display the current bitcoin price on the TFT display
    tft.setCursor(0, 0);
    tft.println("Current Bitcoin Price");
    tft.println();
    tft.print("Price: $");
    tft.println(bitcoin_price, 2);
  }
  else {
    // Display an error message on the TFT display
    tft.setCursor(0, 50);
    tft.println("Error: Unable to retrieve bitcoin price");
  }

  // End the HTTP connection
  http.end();
}

