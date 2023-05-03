void displayBitcoinPrice() {
  // Clear the screen
  tft.fillScreen(BACKGROUND_COLOR);

  DynamicJsonDocument doc(1024);
  getData(bitcoin_price_api_url, doc);
    
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
