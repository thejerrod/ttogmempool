#include <TFT_eSPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <RTClib.h>

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
const char* ssid     = "";       ///EDIT
const char* password = ""; //EDIT

// API URLs

const char* mempool_api_url = "https://mempool.space/api/v1/fees/recommended";
const char* lightning_api_url = "https://mempool.space/api/v1/lightning/statistics/latest";
const char* block_api_url = "https://mempool.space/api/blocks/";
const char* bitcoin_price_api_url = "https://api.coindesk.com/v1/bpi/currentprice.json"; 
const char* lightning_api_3m_url = "https://mempool.space/api/v1/lightning/statistics/3m"; 
const char* lnplus_api_url = "https://lightningnetwork.plus/api/2/get_node/pubkey=0254bb156ecd0eac318844415a91a377bc6947ea4c9fbe5d248e563c29a1662835"; 

//map for api_url cache (becasue arduino's std::map sucks)
std::vector<std::pair<String, std::pair<DynamicJsonDocument, unsigned long>>> apiCache;

//screen setup
int titleX;
int titleY;
int textX;
int textY;

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
  
  //checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);

  // Display the current page
  switch (currentDisplay) {
    case 0:
      // Display the suggested fees
      displaySuggestedFees();
      break;

    case 1:
      // Display the lightning network statistics 
      displayLightningStats();
      break; 
 
    case 2:
      // Display the current block info 
      displayBlocks();
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
      // Display  node info for lightning pubkey
      nodeBuddy();
      break;
  }
}

void displaySuggestedFees() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  DynamicJsonDocument doc(1024);
  getData(mempool_api_url, doc, apiCache);

  // Parse the JSON response
  int fastestFee = doc["fastestFee"];
  int halfHourFee = doc["halfHourFee"];
  int hourFee = doc["hourFee"];
  int economyFee = doc["economyFee"];
  String title = "Fees";
  String text = "Text";
  uint16_t color = ACCENT_COLOR;
  drawBorderText(0, 0, title, text, color);
    
  tft.setTextColor(TITLE_COLOR);
  tft.setTextSize(TITLE_SIZE);
  tft.println(title);
  tft.setTextColor(TITLE_COLOR);
  tft.setTextSize(VALUE_SIZE);
  tft.setCursor(textX + 20, textY + 40); //  20 padding on the x, 40 on the y
  tft.print("Fast: ");
  tft.print(fastestFee);  
  tft.println(" sats");
  tft.setCursor(textX + 20, textY + 60);
  tft.print("30m: ");
  tft.print(halfHourFee);
  tft.println(" sats");
  tft.setCursor(textX + 20, textY + 80); 
  tft.print("60m: ");
  tft.print(hourFee);
  tft.println(" sats");
  tft.setCursor(textX + 20, textY + 100); 
  tft.print("Econ: ");
  tft.print(economyFee);
  tft.println(" sats");

  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay++;
  
}

void displayLightningStats() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  DynamicJsonDocument doc(1024);
  getData(lightning_api_url, doc, apiCache);

  // Parse the JSON response
  int channel_count = doc["latest"]["channel_count"].as<int>();
  int clearnet_nodes = doc["latest"]["clearnet_nodes"].as<int>();
  int tor_nodes = doc["latest"]["tor_nodes"].as<int>();
  uint64_t total_capacity = doc["latest"]["total_capacity"].as<uint64_t>();
  double total_capacity_btc = total_capacity / 100000000.0;
  int med_fee_rate = doc["latest"]["med_fee_rate"].as<int>();

  // Draw the title border
  String title = "LN Stats";
  String text = "Channels: " + String(channel_count);
  uint16_t color = ACCENT_COLOR;
  drawBorderText(0, 0, title, text, color);

  // Display the data values
  const int textX = 20;
  const int textY = 40;
  const int lineHeight = 18;
  tft.setTextColor(TITLE_COLOR);
  tft.setTextSize(TITLE_SIZE);
  //tft.setCursor(textX, textY - 30);
  tft.println(title);
  tft.setTextSize(VALUE_SIZE);
  tft.setCursor(textX, textY);
  tft.print("Channels: ");
  tft.println(channel_count);

  tft.setCursor(textX, textY + lineHeight);
  tft.print("Clear nodes: ");
  tft.println(clearnet_nodes);

  tft.setCursor(textX, textY + 2 * lineHeight);
  tft.print("Tor nodes: ");
  tft.println(tor_nodes);

  tft.setCursor(textX, textY + 3 * lineHeight);
  tft.print("BTC: ");
  tft.println(total_capacity_btc);

  tft.setCursor(textX, textY + 4 * lineHeight);
  tft.print("Fee rate: ");
  tft.print(med_fee_rate);
  tft.println(" sats");

  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay++;
}

void displayBlocks() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);
  DynamicJsonDocument doc(2048);
  getData(block_api_url, doc, apiCache);
  const size_t num_blocks = doc.size();

  // Calculate blocks and time until halving
  int current_block = doc[0]["height"];
  int tx_count = doc[0]["tx_count"];
  int blocks_since_last_halving = current_block % 210000;
  int blocks_until_halving = 210000 - blocks_since_last_halving;
  int time_until_halving = blocks_until_halving * 10;
  String title = "Blocks";
  String text = "Text";
  uint16_t color = ACCENT_COLOR;

  // Calculate time between blocks
  int64_t block_time = 0;
  // look at last 10 blocks
  int block_count = std::min(doc.size() - 1, static_cast<size_t>(10));
  for (int i = 0; i < block_count; i++) {
    int64_t timestamp1 = doc[i]["timestamp"].as<int64_t>();
    int64_t timestamp2 = doc[i + 1]["timestamp"].as<int64_t>();
    block_time += (timestamp1 - timestamp2);
  }
  block_time /= block_count; // average time between blocks in seconds

  // convert to minutes
  int block_time_in_minutes = block_time / 60;

  // Get block size in megabytes
  float block_size_mb = (float)doc[0]["size"].as<int>() / 1000000;

  // Display the current block height and some additional information on the TFT display
  drawBorderText(0, 0, title, text, color);
  tft.setTextSize(TITLE_SIZE);
  tft.setTextColor(TITLE_COLOR);
  tft.println(title);
  tft.setTextSize(2);
  tft.setCursor(15 , 40);
  tft.print("Current: ");
  tft.println(current_block);
  tft.setCursor(15 , 60);
  tft.print("Block time: ~");
  tft.print(block_time_in_minutes);
  tft.print("m");
  tft.setCursor(40 , 85);
  tft.print(tx_count);
  tft.print("tx/");
  tft.print(block_size_mb);
  tft.print("MB");

  // Calculate progress bar values
  float progress = (float)blocks_since_last_halving / 210000;
  int progressBarWidth = 120;
  int progressBarHeight = 10;
  int progressBarX = (240 - progressBarWidth) / 2;
  int progressBarY = 105;
  int progressFillWidth = (int)(progressBarWidth * progress);

  // Draw progress bar
  tft.drawRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight, TFT_LIGHTGREY);
  tft.fillRect(progressBarX, progressBarY, progressFillWidth, progressBarHeight, ACCENT_COLOR);
  tft.setTextSize(1);
  tft.setCursor(progressBarX - 5, progressBarY + 15);
  tft.print("Halving in ");
  tft.print(blocks_until_halving);
  tft.print(" blocks");

  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay++;
}

void displayBitcoinPrice() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);


  DynamicJsonDocument doc(2048);
  getData(bitcoin_price_api_url, doc, apiCache);
    
  String updated_time = doc["time"]["updatedISO"];
  float bitcoin_usd_price = doc["bpi"]["USD"]["rate_float"];
  float bitcoin_gbp_price = doc["bpi"]["GBP"]["rate_float"];
  float bitcoin_eur_price = doc["bpi"]["EUR"]["rate_float"];

  String title = "Price";
  String text = "Text";
  uint16_t color = 0xFFFF;
  drawBorderText(0, 0, title, text, ACCENT_COLOR);

    
  // Display the current bitcoin price on the TFT display
  tft.setTextSize(TITLE_SIZE);
  tft.setTextColor(TITLE_COLOR);
  tft.println(title);
  tft.setTextSize(VALUE_SIZE);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(textX + 40, textY + 40); 
  tft.print("$");
  tft.print(bitcoin_usd_price, 2);
  tft.println(" USD");
  tft.setCursor(textX + 40, textY + 60); 
  tft.print("£");
  tft.print(bitcoin_gbp_price, 2);
  tft.println(" GBP");
  tft.setCursor(textX + 40, textY + 80); 
  tft.print("€");
  tft.print(bitcoin_eur_price, 2);
  tft.println(" EUR");

  //Print updated date
  tft.setCursor(textX + 40, textY +110);
  tft.setTextSize(1);
  tft.print(updated_time);

  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay++;
}

void displayTransactionVolumeOverTime() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);
   
  DynamicJsonDocument doc(2048);
  getData(block_api_url, doc, apiCache);

  String title = "TX Vol";
  String text = "Text";
  uint16_t color = TFT_BLACK;
  drawBorderText(0, 0, title, text, color);
  tft.setTextSize(TITLE_SIZE);
  tft.print(title);
  
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

  // Draw chart axis and labels
  tft.fillRect(0, chartY, tft.width(), chartH, TFT_BLACK);
  tft.drawFastHLine(chartPadding, chartY + chartH - chartPadding, chartW - 2*chartPadding, TFT_WHITE);
  tft.drawFastVLine(chartPadding, chartY + chartH - chartPadding, -chartH + 2*chartPadding, TFT_WHITE);
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
  }

  // Draw vertical grid lines
  for (int i = 0; i < numPoints; i++) {
  int x = chartPadding + i*xStep;
  tft.drawFastVLine(x, chartY + chartPadding, chartH - 2*chartPadding, TFT_LIGHTGREY);
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

    // Calculate the center x-coordinate for the footer line
    int footerWidth = tft.textWidth("Last 6 blocks");
    int footerX = (tft.width() - footerWidth) / 2;
    int titleY = 1;
    // Calculate the y-coordinate for the USD line
    int footerY = titleY + tft.fontHeight(TITLE_SIZE) + 80;

    tft.setCursor(footerX, footerY +30); // Draw the footer centered on the x-coordinate +30 padding
    tft.setTextSize(1);
    tft.print("Last ");
    tft.print(numBlocks);
    tft.print(" blocks");
  }
    delay(5000);
    checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
    currentDisplay++;
}

void displayCapacityGrowth() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  // Send the API request and parse the response
  DynamicJsonDocument doc(4096);
  getData(lightning_api_3m_url, doc, apiCache);

  // Calculate some variables for chart display
  String title = "LN Growth";
  String text = "Text";
  int chartWidth = 200;
  int chartHeight = 55;
  int chartMargin = 1;
  int borderThickness = 10;
  const float SMOOTHING_FACTOR = .5;
  int numInstances = doc.size();

  // Find the maximum values for capacity and node count
  uint64_t max_capacity = 0;
  uint64_t max_nodes = 0;
  uint64_t max_tor_nodes = 0;
  uint64_t max_clearnet_nodes = 0;
  uint64_t max_unannounced_nodes = 0;
  uint64_t max_clearnet_tor_nodes = 0;
  for (JsonVariant instance : doc.as<JsonArray>()) {
    uint64_t total_capacity = instance["total_capacity"];
    if (total_capacity > max_capacity) {
      max_capacity = total_capacity;
    }
    uint64_t total_nodes = instance["channel_count"];
    if (total_nodes > max_nodes) {
      max_nodes = total_nodes;
    }
    uint64_t tor_nodes_count = instance["tor_nodes"];
    uint64_t clearnet_nodes_count = instance["clearnet_nodes"];
    uint64_t unannounced_nodes_count = instance["unannounced_nodes"];
    uint64_t clearnet_tor_nodes_count = instance["clearnet_tor_nodes"];
    if (tor_nodes_count > max_tor_nodes) {
      max_tor_nodes = tor_nodes_count;
    }
    if (clearnet_nodes_count > max_clearnet_nodes) {
      max_clearnet_nodes = clearnet_nodes_count;
    }
    if (unannounced_nodes_count > max_unannounced_nodes) {
      max_unannounced_nodes = unannounced_nodes_count;
    }
    if (clearnet_tor_nodes_count > max_clearnet_tor_nodes) {
      max_clearnet_tor_nodes = clearnet_tor_nodes_count;
    }
  }

  // Calculate scaling factors for bar chart display
  float capacityScaleFactor = (float)chartHeight / (float)max_capacity;

  drawBorderText(0, 0, title, text, ACCENT_COLOR);
  // Add labels and title to the graph
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(TITLE_SIZE);
  tft.print(title);

  // Draw the capacity bar chart
  int capacityBarWidth = (chartWidth - (numInstances - 1) * chartMargin) / numInstances;
  int capacityChartX = (TFT_HEIGHT - chartWidth) / 2;
  int capacityChartY = (115 - chartHeight) + borderThickness;
  int capacityBarX = capacityChartX + borderThickness;
  for (int i = 0; i < numInstances; i++) {
    uint64_t capacity = doc[i]["total_capacity"].as<uint64_t>();
    int capacityBarHeight = (int)(capacity * capacityScaleFactor);
    int capacityBarY = capacityChartY + chartHeight - capacityBarHeight;
    tft.fillRect(capacityBarX, capacityBarY, capacityBarWidth, capacityBarHeight, TFT_BLUE);
    capacityBarX += capacityBarWidth + chartMargin;
  }

  // Calculate scaling factors for line chart display
  float max_node_count = max(max(max_tor_nodes, max_clearnet_nodes), max(max_unannounced_nodes, max_clearnet_tor_nodes));
  float nodeScaleFactor = (float)(chartHeight - 20) / (float)max_node_count;
  int nodeY = 0; 
  int nodeLineWidth = 3;
  int nodeChartX = (capacityChartX / 2 + 50);
  int nodeChartY = (capacityChartY / 2 +10);
  const uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_YELLOW, TFT_CYAN};
  int colorIndex = 0;
  for (const char* nodeType : {"tor_nodes", "clearnet_nodes",  "clearnet_tor_nodes"}) {  //removed "unannounced_nodes",
    //drop the _nodes from the response and put it in label 
    std::string nodeTypeStr(nodeType);
    std::string label = nodeTypeStr.substr(0, nodeTypeStr.find("_nodes"));
    int prevNodeCount = -1;
    float prevSmoothedNodeCount = -1.0;
    for (int i = 0; i < numInstances; i++) {
      uint64_t nodeCount = doc[i][nodeType].as<uint64_t>();
      float smoothedNodeCount = nodeCount;
      if (prevSmoothedNodeCount != -1.0) {
        smoothedNodeCount = prevSmoothedNodeCount + SMOOTHING_FACTOR * (nodeCount - prevSmoothedNodeCount);
      }
       nodeY = nodeChartY + chartHeight - (int)(smoothedNodeCount * nodeScaleFactor) - 10;
      if (prevNodeCount != -1) {
        int prevNodeY = nodeChartY + chartHeight - (int)(prevSmoothedNodeCount * nodeScaleFactor) - 10;
        uint16_t color = colors[colorIndex % (sizeof(colors) / sizeof(colors[0]))];
        tft.drawLine(nodeChartX + (i-1)*(nodeLineWidth+chartMargin), prevNodeY, nodeChartX + i*(nodeLineWidth+chartMargin), nodeY, color);  
    }
    prevNodeCount = nodeCount;
    prevSmoothedNodeCount = smoothedNodeCount;
   }
   uint16_t color = colors[colorIndex % (sizeof(colors) / sizeof(colors[0]))];
   tft.setTextSize(1);
   tft.setTextDatum(TR_DATUM); // Set text alignment to top-right
   tft.setTextColor(color);
   tft.drawString(label.c_str(), nodeChartX + chartWidth - 35, (nodeY +2), 2);
  
  colorIndex++;
  nodeChartY += 10 + 1;
  }
  tft.setRotation(0);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(borderThickness+2,borderThickness+10);
  tft.print("Capacity");
  tft.setRotation(1);

  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay++;
}

void nodeBuddy(){
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Send the API request
  DynamicJsonDocument doc(2048);
  getData(lnplus_api_url, doc, apiCache);

  // Parse the JSON response
  String alias = doc["alias"].as<String>();
  String node_color = doc["color_hex"];
  String open_channels = doc["open_channels"];
  uint64_t total_capacity = doc["capacity"].as<uint64_t>();
  double total_capacity_btc = total_capacity / 100000000.0;
  String lnp_hubness = doc["weighted_hubness_rank"];
  String lnp_rank = doc["weighted_hubness_rank"];
  String lnp_rank_name = doc["lnp_rank_name"].as<String>();
  String lnp_positive_ratings_received = doc["lnp_positive_ratings_received"].as<String>();
  String title = alias;
  String text = "Text";
  String color = node_color;   //need to takke this from #xxxx to 0xXXXX
  drawBorderText(0, 0, title, text, TFT_MAGENTA);
    
   // Display node info 
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(VALUE_SIZE);
  tft.setCursor(titleX, titleY);
  tft.println(alias);
  tft.setTextColor(TITLE_COLOR);
  tft.setTextSize(VALUE_SIZE);
  tft.setCursor(textX + 20, textY + 35); //  20 padding on the x, 40 on the y
  tft.print("Channels: ");
  tft.println(open_channels);
  tft.setCursor(textX + 20, textY + 50);
  tft.print("Capacity: ");
  tft.print(total_capacity_btc);
  tft.println("B");
  tft.setCursor(textX + 20, textY + 65);
  tft.print("Hubness: ");
  tft.println(lnp_hubness);
  tft.setCursor(textX + 20, textY + 80);
  tft.print("Rank: ");
  tft.println(lnp_rank);
  tft.setCursor(textX + 20, textY + 95);
  tft.print("Ratings: ");
  tft.println(lnp_positive_ratings_received);
  tft.setCursor(textX + 20, textY + 110);
  tft.print("LN+ level:");
  tft.setTextColor(0xffda24);
  tft.println(lnp_rank_name);
 
  delay(5000);
  checkButtonPress(currentDisplay, maxDisplay, lastButtonPress, button1State, button2State);
  currentDisplay = 0;
}

//////////tools
void checkButtonPress(int& currentDisplay, int maxDisplay, unsigned long& lastButtonPress, int& button1State, int& button2State) {
  // Get the current time
  unsigned long currentTime = millis();

  // Check if button 1 was pressed
  button1State = digitalRead(BUTTON_1);
  if (button1State == LOW && currentTime - lastButtonPress > 1) {
    currentDisplay--;
    if (currentDisplay < 0) {
      currentDisplay = maxDisplay;
    }
    lastButtonPress = currentTime;
  }

  // Check if button 2 was pressed
  button2State = digitalRead(BUTTON_2);
  if (button2State == LOW && currentTime - lastButtonPress > 1) {
    currentDisplay++;
    if (currentDisplay > maxDisplay) {
      currentDisplay = 0;
    }
    lastButtonPress = currentTime;
  }
}

void getData(String api_url, DynamicJsonDocument& doc, std::vector<std::pair<String, std::pair<DynamicJsonDocument, unsigned long>>>& apiCache) {
  unsigned long currentTime = millis();

  // Define the cache duration in seconds
  const int CACHE_DURATION = 60;

  // Check if the cache is still valid
  bool cacheHit = false;
  for (auto& cacheEntry : apiCache) {
    if (cacheEntry.first == api_url && currentTime - cacheEntry.second.second < CACHE_DURATION * 1000) {
      // Use the cached data
      doc = cacheEntry.second.first;
      cacheHit = true;
      break;
    }
  }

  if (!cacheHit) {
    // Make a new API request
    HTTPClient http;
    http.begin(api_url);
    int httpCode = http.GET();

    // Check if the request was successful
    if (httpCode == HTTP_CODE_OK) {
      // Parse the JSON response
      String response = http.getString();
      deserializeJson(doc, response);

      // Update the cache
      apiCache.push_back({api_url, {doc, currentTime}});
    }

    // End the HTTP connection
    http.end();
  }
}

void drawBorderText(int x, int y, String title, String text, uint16_t color) {
   
  // Calculate the center x-coordinate for the title
  int titleWidth = tft.textWidth(title);
  int titleX = x + (tft.width() - titleWidth) / 2;

  // Calculate the center x-coordinate for the text
  int textWidth = tft.textWidth(text);
  int textX = x + (tft.width() - textWidth) / 2;

  // Calculate the y-coordinate for the text
  int titleY = y;
  int textY = titleY + tft.fontHeight(TITLE_SIZE) + 45;

  // Define the border coordinates
  int borderX = x + 10;
  int borderY = titleY + tft.fontHeight(TITLE_SIZE)+30;
  int borderWidth = tft.width() - 20;
  int borderHeight = tft.height() - borderY - 5;
    
  // Draw the border around the bottom text
  tft.drawRect(borderX, borderY, borderWidth, borderHeight, color);
  //set the title cursor centroed on x
   tft.setCursor(titleX - 30, titleY);
}
