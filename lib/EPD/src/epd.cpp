#include "epd.h"
#include "epd_spi.h"

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
