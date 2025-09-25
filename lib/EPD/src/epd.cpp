#include "epd.h"
#include "epd_spi.h"
#include "font_simple.h"
#include "font_small.h"

// Glavne display funkcije
void epd_init(void) {
  epd_setup_spi();
  epd_setup_pins();
  EPD_HW_Init();
}

void epd_clear_screen(void) {
  epd_display_white_screen();
}

void epd_display_image(const uint8_t* image) {
  EPD_HW_Init();
  EPD_WhiteScreen_ALL(image);
  epd_sleep();
}

void epd_display_white_screen(void) {
  EPD_HW_Init();
  EPD_WhiteScreen_White();
  epd_sleep();
}

void epd_display_black_screen(void) {
  EPD_HW_Init();
  EPD_WhiteScreen_Black();
  epd_sleep();
}

void epd_fast_update_init(void) {
  EPD_HW_Init_Fast();
}

void epd_fast_update_display(const uint8_t* image) {
  EPD_HW_Init_Fast();
  EPD_WhiteScreen_ALL_Fast(image);
  epd_sleep();
}

void epd_4gray_init(void) {
  EPD_HW_Init_4Gray();
}

void epd_4gray_display(const uint8_t* image) {
  EPD_HW_Init();
  EPD_WhiteScreen_White();
  epd_sleep();
  
  EPD_HW_Init_4Gray();
  EPD_WhiteScreen_ALL_4Gray(image);
  epd_sleep();
}

void epd_partial_update_time(uint8_t x1, uint8_t y1, const uint8_t* data1,
                            uint8_t x2, uint8_t y2, const uint8_t* data2,
                            uint8_t x3, uint8_t y3, const uint8_t* data3,
                            uint8_t x4, uint8_t y4, const uint8_t* data4,
                            uint8_t x5, uint8_t y5, const uint8_t* data5,
                            uint8_t width, uint8_t height) {
  EPD_HW_Init();
  EPD_Dis_Part_Time(x1, y1, data1, x2, y2, data2, x3, y3, data3, x4, y4, data4, x5, y5, data5, width, height);
  epd_sleep();
}

void epd_partial_update_full(const uint8_t* image) {
  EPD_HW_Init();
  EPD_Dis_PartAll(image);
  epd_sleep();
}

void epd_set_basemap(const uint8_t* basemap) {
  EPD_SetRAMValue_BaseMap(basemap);
}

void epd_sleep(void) {
  EPD_DeepSleep();
}

// Utility funkcije
void epd_setup_spi(void) {
  SPI.begin(EPD_SCK_PIN, EPD_MISO_PIN, EPD_MOSI_PIN);
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
}

void epd_setup_pins(void) {
  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN, OUTPUT);
  pinMode(EPD_CS_PIN, OUTPUT);
}

bool epd_is_busy(void) {
  return digitalRead(EPD_BUSY_PIN) == LOW;
}

void epd_wait_for_busy(void) {
  while (epd_is_busy()) {
    delay(10);
  }
}

// Text funkcije
void epd_create_text_buffer(uint8_t* buffer) {
  // Inicijaliziraj buffer kao bijeli (sve 1)
  for (uint16_t i = 0; i < EPD_ARRAY; i++) {
    buffer[i] = 0xFF;
  }
}

void epd_draw_text(int x, int y, const char* text, bool white) {
  // Ova funkcija radi direktno na display buffer-u
  // Koristi se interno
}

void epd_draw_char(int x, int y, char c, bool white) {
  // Ova funkcija radi direktno na display buffer-u
  // Koristi se interno
}

int epd_get_text_width(const char* text) {
  return getTextWidth(text);
}

void epd_display_text(const char* text, int x, int y, bool white) {
  // Kreiraj buffer za tekst
  static uint8_t textBuffer[EPD_ARRAY];
  
  // Inicijaliziraj buffer kao bijeli
  epd_create_text_buffer(textBuffer);
  
  // Crtaj tekst na buffer
  drawText(textBuffer, x, y, text, white);
  
  // Prikaži buffer na display
  epd_display_image(textBuffer);
}

// Small font funkcije (na temelju ESP32_ePaper_example SmallFont.c)
void epd_draw_small_text(int x, int y, const char* text, bool white) {
  // Ova funkcija radi direktno na display buffer-u
  // Koristi se interno
}

void epd_draw_small_char(int x, int y, char c, bool white) {
  // Ova funkcija radi direktno na display buffer-u
  // Koristi se interno
}

int epd_get_small_text_width(const char* text) {
  return getSmallTextWidth(text);
}

void epd_display_small_text(const char* text, int x, int y, bool white) {
  // Kreiraj buffer za tekst
  static uint8_t textBuffer[EPD_ARRAY];
  
  // Inicijaliziraj buffer kao bijeli
  epd_create_text_buffer(textBuffer);
  
  // Crtaj tekst na buffer pomoću small fonta
  drawSmallText(textBuffer, x, y, text, white);
  
  // Prikaži buffer na display
  epd_display_image(textBuffer);
}


// Image funkcije
void epd_display_image_at_position(const uint8_t* image, int x, int y, int width, int height) {
  // Kreiraj buffer za sliku
  static uint8_t imageBuffer[EPD_ARRAY];
  
  // Inicijaliziraj buffer kao bijeli
  epd_create_text_buffer(imageBuffer);
  
  // Izračunaj poziciju u buffer-u
  int imageBytes = (width * height) / 8;
  int startX = x;
  int startY = y;
  
  // Kopiraj sliku u buffer na određenoj poziciji
  for (int i = 0; i < imageBytes && i < EPD_ARRAY; i++) {
    int bufferX = startX + (i * 8) % width;
    int bufferY = startY + (i * 8) / width;
    
    if (bufferY < EPD_HEIGHT && bufferX < EPD_WIDTH) {
      uint16_t bufferIndex = (bufferX + bufferY * EPD_WIDTH) / 8;
      if (bufferIndex < EPD_ARRAY) {
        imageBuffer[bufferIndex] = pgm_read_byte(&image[i]);
      }
    }
  }
  
  // Prikaži buffer na display
  epd_display_image(imageBuffer);
}

void epd_display_image_centered(const uint8_t* image, int width, int height) {
  int x = (EPD_WIDTH - width) / 2;
  int y = (EPD_HEIGHT - height) / 2;
  epd_display_image_at_position(image, x, y, width, height);
}
