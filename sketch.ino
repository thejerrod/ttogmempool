#include <TFT_eSPI.h>
#include <WiFi.h>
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

// Define font sizes and colors
#define TITLE_SIZE 3
#define TITLE_COLOR TFT_WHITE
#define LABEL_SIZE 2
#define LABEL_COLOR TFT_LIGHTGREY
#define VALUE_SIZE 2
#define VALUE_COLOR ACCENT_COLOR

// wifi setup
const char* ssid     = "wifi ssid here";       ///EDIIIT
const char* password = "wifi password here"; //edit

// API URLs
const char* mempool_api_url = "https://mempool.space/api/v1/fees/recommended";
const char* lightning_api_url = "https://mempool.space/api/v1/lightning/statistics/latest";
const char* block_api_url = "https://mempool.space/api/blocks/";
const char* bitcoin_price_api_url = "https://api.coindesk.com/v1/bpi/currentprice.json"; 
const char* lightning_api_3m_url = "https://mempool.space/api/v1/lightning/statistics/1w"; 

// Default suggested fees
int fastestFee = 0;
int halfHourFee = 0;
int hourFee = 0;
int economyFee = 0;

// Lightning network statistics
int channel_count = 0;
int clearnet_nodes = 0;
int tor_nodes = 0;
int total_capacity = 0;
int med_fee_rate = 0;

// Current block height
int current_block = 0;

// Button variables
int button1State;
int button2State;
unsigned long lastButtonPress = 0;
int currentDisplay = 0;
int maxDisplay = 6;

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
  
  // Connect wifi
  WiFi.begin(ssid, password);
  tft.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    tft.print(".");
   }
  tft.print("CONECTED!!");
  delay(1000);
}

void loop() {
  updateDisplay();
}

void updateDisplay(){
 // Get the current time
  unsigned long currentTime = millis();

 // Check if button 1 was pressed
 button1State = digitalRead(BUTTON_1);
 if (button1State == LOW && currentTime - lastButtonPress > 50000) {
   currentDisplay--;
   if (currentDisplay < 0) {
     currentDisplay = maxDisplay;
   }
   lastButtonPress = currentTime;
 }

 // Check if button 2 was pressed
 button2State = digitalRead(BUTTON_2);
 if (button2State == LOW && currentTime - lastButtonPress > 50000) {
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

    // Parse the JSON response
    fastestFee = doc["fastestFee"];
    halfHourFee = doc["halfHourFee"];
    hourFee = doc["hourFee"];
    economyFee = doc["economyFee"];
    
    // Calculate the center x-coordinate for the title
   int titleWidth = tft.textWidth("Fees");
   int titleX = (tft.width() - titleWidth) / 2;

   // Calculate the center x-coordinate for the "Fastest" line
   int fastestWidth = tft.textWidth("$" + String(fastestFee, 2) + " USD");
   int fastestX = (tft.width() - fastestWidth) / 2;

   // Calculate the y-coordinate for the Fastest line
   int titleY = 1;
   int fastestY = titleY + tft.fontHeight(TITLE_SIZE) + 45;

   // Define the border coordinates
   int borderX = 10;
   int borderY = titleY + tft.fontHeight(TITLE_SIZE)+30;
   int borderWidth = tft.width() - 20;
   int borderHeight = tft.height() - borderY - 5;

   // Display the fees
   tft.setTextColor(TITLE_COLOR);
   tft.setTextSize(TITLE_SIZE);
   tft.setCursor(titleX, titleY); //centered 
   tft.println("Fees");
   tft.setTextColor(TITLE_COLOR);
   tft.setTextSize(VALUE_SIZE);
   tft.setCursor(fastestX, fastestY); // Draw the "Fastest" line centered on the x-coordinate
   tft.print("Fast: ");
   tft.print(fastestFee);
   tft.println(" sats");
   tft.setCursor(fastestX, fastestY  +20); // Draw the "Fastest" line centered on the x-coordinate
   tft.print("30m: ");
   tft.print(halfHourFee);
   tft.println(" sats");
   tft.setCursor(fastestX, fastestY +40); // Draw the "Fastest" line centered on the x-coordinate
   tft.print("60m: ");
   tft.print(hourFee);
   tft.println(" sats");
   tft.setCursor(fastestX, fastestY +60); // Draw the "Fastest" line centered on the x-coordinate
   tft.print("Econ: ");
   tft.print(economyFee);
   tft.println(" sats");
   
   // Draw the border around the bottom text
   tft.drawRect(borderX, borderY, borderWidth, borderHeight, VALUE_COLOR);
    
   } else {
   // If the request failed, display an error message on the TFT display
   tft.setCursor(0, 0);
   tft.print("Error: Failed to retrieve suggested fees.");
 }

  // End the HTTP connection
  http.end();
}


void displayTransactionVolumeOverTime() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);
 
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

  // Initialize variables for drawing the chart
  int chartX = 0;
  int chartY = 30;
  int chartW = tft.width();
  int chartH = tft.height() - chartY - 10;
  int chartPadding = 1;
  int numPoints = numBlocks;
  float xStep = (float)(chartW - 2*chartPadding) / (float)(numPoints - 1);
  float yScale = (float)(chartH - 2*chartPadding) / (float)maxTxs;

  // Define color gradient
  uint16_t gradient[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN};

  // Calculate the center x-coordinate for the title
  int titleWidth = tft.textWidth("Tx Vol");
  int titleX = (tft.width() - titleWidth) / 2;

  // Draw chart axis and labels
  tft.fillRect(0, chartY, tft.width(), chartH, TFT_BLACK);
  //tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_WHITE);
  //tft.drawFastVLine(chartPadding, chartY + chartH - chartPadding, -chartH + 2*chartPadding, TFT_WHITE);
  tft.setTextSize(TITLE_SIZE);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(titleX, 1); // Draw the title centered on the x-coordinate
  tft.print("Tx Vol");
  tft.drawFastVLine(chartPadding, chartY + chartPadding, chartH - 2*chartPadding, TFT_WHITE);
  tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_WHITE);

  // Draw background grid
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setTextSize(1);

  // Number of grid lines
  int numLines = numBlocks;

  // Draw horizontal grid lines
  for (int i = 0; i <= numLines; i++) {
  int y = chartY + chartH - chartPadding - i*chartH/numLines;
  tft.drawFastHLine(chartPadding, y, chartW - 2*chartPadding, TFT_LIGHTGREY);
  tft.setCursor(chartPadding + 4, y - 4);
  // tft.print(String(i*maxTxs/numLines) + " tx");
  }

  // Draw vertical grid lines
  for (int i = 0; i < numPoints; i++) {
  int x = chartPadding + i*xStep;
  tft.drawFastVLine(x, chartY + chartPadding, chartH - 2*chartPadding, TFT_LIGHTGREY);
  
  // Add tick marks on x-axis
  // tft.drawFastVLine(x, chartY + chartH - chartPadding, 5, TFT_WHITE);
 }

  // Draw chart line with color gradient
  for (int i = 0; i < numPoints-1; i++) {
    float x1 = chartPadding + i*xStep;
    float x2 = chartPadding + (i+1)*xStep;
    float y1 = chartY + chartH - chartPadding - yScale*txsPerBlock[i];
    float y2 = chartY + chartH - chartPadding - yScale*txsPerBlock[i+1];
    float txVolume = (txsPerBlock[i] + txsPerBlock[i+1]) / 2.0;
    uint16_t color = TFT_BLACK;
    if (txVolume <= maxTxs/4) {
      color = gradient[0];
    } else if (txVolume <= maxTxs/2) {
      color = gradient[1];
    } else if (txVolume <= 3*maxTxs/4) {
      color = gradient[2];
    } else {
      color = gradient[3];
    }
    tft.drawLine(x1, y1, x2, y2, color);
    tft.fillCircle(x1, y1, 3, color);
  }
  // Draw last data point
  float x = chartPadding + (numPoints-1)*xStep;
  float y = chartY + chartH - chartPadding - yScale*txsPerBlock[numPoints-1];
  tft.fillCircle(x, y, 3, TFT_ORANGE);

//footer

    // Calculate the center x-coordinate for the footer line
    int footerWidth = tft.textWidth("Last 6 blocks");
    int footerX = (tft.width() - footerWidth) / 2;
    int titleY = 1;
    // Calculate the y-coordinate for the USD line
    int footerY = titleY + tft.fontHeight(TITLE_SIZE) + 80;

    tft.setCursor(footerX, footerY +30); // Draw the footer centered on the x-coordinate +30 padding
    tft.setTextSize(1);
    tft.print("Last 6 blocks");
  }
  }  else {
    // Display an error message on the TFT display
    tft.setCursor(0, 50);
    tft.println("Error: Unable to retrieve bitcoin price");
  }
  
  // Clean up
  http.end();
}


void displayLightningStats() {

  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);


  // Send the API request
  HTTPClient http;
  http.begin(lightning_api_url);
  int httpCode = http.GET();
  
  // If the request was successful, parse the JSON response
  if (httpCode == HTTP_CODE_OK) {
    String jsonStr = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonStr);
      
    // Parse the JSON response
    channel_count = doc["latest"]["channel_count"];
    clearnet_nodes = doc["latest"]["clearnet_nodes"];
    tor_nodes = doc["latest"]["tor_nodes"];
    total_capacity = doc["latest"]["total_capacity"];
    med_fee_rate = doc["latest"]["med_fee_rate"];

    // Calculate the center x-coordinate for the title
    int titleWidth = tft.textWidth("Tx Vol");
    int titleX = (tft.width() - titleWidth) / 2;

    // Define the border coordinates
    int titleY = 1;
    int borderX = 10;
    int borderY = titleY + tft.fontHeight(TITLE_SIZE)+30;
    int borderWidth = tft.width() - 20;
    int borderHeight = tft.height() - borderY - 5;

    // Calculate the center x-coordinate for the USD line
    int clearnetWidth = tft.textWidth("Clearnet nodes: ");
    int clearnetX = (tft.width() - clearnetWidth) / 2;

    // Calculate the y-coordinate for the "clearnet" line
    int clearnetY = titleY + tft.fontHeight(TITLE_SIZE) + 45;

    // Display the current block height and some additional information on the TFT display
    tft.setTextSize(TITLE_SIZE);
    tft.setTextColor(TITLE_COLOR);
    tft.setCursor(titleX -40 , titleY ); // Draw the title centered on the x-coordinate
    tft.println("Lightning");
    tft.println();
    tft.setCursor(clearnetX, clearnetY);
    tft.setTextSize(1);
    tft.setTextColor(TITLE_COLOR);
    tft.print("Channels: ");
    tft.println(channel_count);
    tft.setCursor(clearnetX, clearnetY +10);
    tft.print("Clearnet: ");
    tft.println(clearnet_nodes);
    tft.setCursor(clearnetX, clearnetY +20);
    tft.print("tor: ");
    tft.println(tor_nodes);
    tft.setCursor(clearnetX, clearnetY +30);
    tft.print("Capacity: ");
    tft.print(total_capacity);
    tft.println("sats");
    tft.setCursor(clearnetX, clearnetY +40 );
    tft.print("Median Fee: ");
    tft.print(med_fee_rate);
    tft.println("sats");
    
    // Draw the border around the bottom text
    tft.drawRect(borderX, borderY, borderWidth, borderHeight, VALUE_COLOR);

  } else {
    // If the API request failed, display an error message
    tft.println("Error: Failed to retrieve Lightning Network statistics");
  }

  // Clean up
  http.end();
}

void displayCapacityGrowth() {
  ////broke
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request
  HTTPClient http;
  http.begin(lightning_api_3m_url);
  int httpCode = http.GET();

  // If the request was successful, parse the JSON response
  if (httpCode == HTTP_CODE_OK) {
    String jsonStr = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonStr);

    // Get the array of objects
    JsonArray data = doc.as<JsonArray>();

    // Create an array to store the total capacity values
    int numPoints = data.size();
    int totalCapacity[numPoints];

  // Loop through the array and extract the total_capacity value from each object
  for (int i = 0; i < numPoints; i++) {
    totalCapacity[i] = data[i]["total_capacity"].as<int>();
  }

// Calculate the width of each bar based on the number of data points and screen width
  int barWidth = 240 / numPoints - 1; // subtract 1 for the space between bars

// Find the maximum value of the y-axis data
  int maxVal = 0;
  for (int i = 0; i < numPoints; i++) {
    int yVal = doc[i]["total_capacity"].as<int>() / 1000000000;
    if (yVal > maxVal) {
      maxVal = yVal;
    }
  }

// Loop through each data point and draw a bar
for (int i = 0; i < numPoints; i++) {
  // Get the value for the current data point
  int yVal = doc[i]["total_capacity"].as<int>() / 1000000000;

  // Map the y-value to fit within the display range
  int y = map(yVal, 0, maxVal * 1.2, 120, 10);

  // Calculate the x-coordinate for the current bar based on its position in the loop and the width of each bar
  int x = i * (barWidth + 1); // add 1 for the space between bars
 
  // Draw a rectangle to represent the current bar
  tft.fillRect(x, 120 - y, barWidth, y, TFT_WHITE);
  //tft.fillRect(10, 120, 10, 50, TFT_WHITE); 

  // Add a label for the current bar indicating its position in the loop (1-based index)
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(x + 2, 123);
  tft.print(i+1);

  // Add a label for the y value of the current bar
  tft.setCursor(x + 2, 110 - y);
  tft.print(yVal);

   // Print the JSON output for the current data point to the display
  String jsonOutput = "Data point " + String(i+1) + ": " + String(yVal);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.print(jsonOutput);

  // Add labels and title to the graph
  tft.setTextColor(TFT_ORANGE);
  tft.setTextSize(1); 
  tft.drawString("Capacity Growth", 10, 15, 2);
  tft.drawString("Days", 110, 110);
  tft.drawString("Sats", 50, 50, 2);
}
  } else {
    // If the request failed, display an error message on the TFT display
    tft.print("Error: Failed to retrieve capacity statsistics.");
  }
  // clean up
  http.end();
}


void displayBlockHeight() {
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

    // Get the updated time
    String updated_time = doc["time"]["updatedISO"];
    
    // Get the current bitcoin price from the response
    float bitcoin_usd_price = doc["bpi"]["USD"]["rate_float"];
    float bitcoin_gbp_price = doc["bpi"]["GBP"]["rate_float"];
    float bitcoin_eur_price = doc["bpi"]["EUR"]["rate_float"];

    // Calculate the center x-coordinate for the title
    int titleWidth = tft.textWidth("Price");
    int titleX = (tft.width() - titleWidth) / 2;

    // Calculate the y-coordinate for the title
    int titleY = 1;

    // Calculate the center x-coordinate for the USD line
    int usdWidth = tft.textWidth("$" + String(bitcoin_usd_price, 2) + " USD");
    int usdX = (tft.width() - usdWidth) / 2;

    // Calculate the y-coordinate for the USD line
    int usdY = titleY + tft.fontHeight(TITLE_SIZE) + 45;


    // Define the border coordinates
    int borderX = 10;
    int borderY = titleY + tft.fontHeight(TITLE_SIZE)+30;
    int borderWidth = tft.width() - 20;
    int borderHeight = tft.height() - borderY - 5;

    // Display the current bitcoin price on the TFT display
    tft.setTextSize(TITLE_SIZE);
    tft.setTextColor(TITLE_COLOR);
    tft.setCursor(titleX, titleY); // Draw the title centered on the x-coordinate
    tft.println("Price");
    tft.println();

    tft.setTextSize(VALUE_SIZE);
    tft.setTextColor(TITLE_COLOR);
    tft.setCursor(usdX, usdY); // Draw the USD line centered on the x-coordinate
    tft.print("$");
    tft.print(bitcoin_usd_price, 2);
    tft.println(" USD");
    tft.setCursor(usdX, usdY +20); // Draw the GBP line centered on the x-coordinate +20 padding
    tft.print("£");
    tft.print(bitcoin_gbp_price, 2);
    tft.println(" GBP");
    tft.setCursor(usdX, usdY +40); // Draw the EUR line centered on the x-coordinate +40 padding
    tft.print("€");
    tft.print(bitcoin_eur_price, 2);
    tft.println(" EUR");
    tft.println();

    //Print updated date
    tft.setCursor(usdX, usdY +70);
    tft.setTextSize(1);
    tft.print(updated_time);

    // Draw the border around the bottom text
    tft.drawRect(borderX, borderY, borderWidth, borderHeight, ACCENT_COLOR);
  
  
   } else {
     // If the request failed, display an error message on the TFT display
     tft.setCursor(0, 0);
     tft.print("Error: Failed to retrieve suggested fees.");
   }

  // Clean up
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
  
  } else {
    // Display an error message on the TFT display
    tft.setCursor(0, 50);
    tft.println("Error: Unable to retrieve bitcoin price");
  }

  // End the HTTP connection
  http.end();
}
