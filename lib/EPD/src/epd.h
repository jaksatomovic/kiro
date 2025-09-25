#ifndef EPD_H
#define EPD_H

#include <Arduino.h>
#include <SPI.h>

// Pin definicije za Adafruit Feather ESP32-S3
#define EPD_BUSY_PIN   6    // BUSY  (input)
#define EPD_RST_PIN    11   // RST   (output)
#define EPD_DC_PIN     12   // DC    (output)
#define EPD_CS_PIN     13   // CS    (output)

// Display konstante
#define EPD_WIDTH      88
#define EPD_HEIGHT     184
#define EPD_FB_SIZE    (EPD_WIDTH * EPD_HEIGHT / 8)
#define EPD_ARRAY      (EPD_WIDTH * EPD_HEIGHT / 8)

// SPI pin definicije za Feather ESP32-S3
#define EPD_MOSI_PIN   35   // MOSI
#define EPD_SCK_PIN    36   // SCK
#define EPD_MISO_PIN   37   // MISO (ne koristi se)

// Display funkcije
void epd_init(void);
void epd_clear_screen(void);
void epd_display_image(const uint8_t* image);
void epd_display_white_screen(void);
void epd_display_black_screen(void);
void epd_fast_update_init(void);
void epd_fast_update_display(const uint8_t* image);
void epd_4gray_init(void);
void epd_4gray_display(const uint8_t* image);
void epd_partial_update_time(uint8_t x1, uint8_t y1, const uint8_t* data1,
                            uint8_t x2, uint8_t y2, const uint8_t* data2,
                            uint8_t x3, uint8_t y3, const uint8_t* data3,
                            uint8_t x4, uint8_t y4, const uint8_t* data4,
                            uint8_t x5, uint8_t y5, const uint8_t* data5,
                            uint8_t width, uint8_t height);
void epd_partial_update_full(const uint8_t* image);
void epd_set_basemap(const uint8_t* basemap);
void epd_sleep(void);

// Text funkcije
void epd_draw_text(int x, int y, const char* text, bool white);
void epd_draw_char(int x, int y, char c, bool white);
int epd_get_text_width(const char* text);
void epd_create_text_buffer(uint8_t* buffer);
void epd_display_text(const char* text, int x, int y, bool white);

// Small font funkcije (na temelju ESP32_ePaper_example SmallFont.c)
void epd_draw_small_text(int x, int y, const char* text, bool white);
void epd_draw_small_char(int x, int y, char c, bool white);
int epd_get_small_text_width(const char* text);
void epd_display_small_text(const char* text, int x, int y, bool white);


// Image funkcije
void epd_display_image_at_position(const uint8_t* image, int x, int y, int width, int height);
void epd_display_image_centered(const uint8_t* image, int width, int height);

// Utility funkcije
void epd_setup_spi(void);
void epd_setup_pins(void);
bool epd_is_busy(void);
void epd_wait_for_busy(void);

// Interni driver funkcije (potrebne za epd.cpp)
void EPD_HW_Init(void);
void EPD_HW_Init_Fast(void);
void EPD_HW_Init_4Gray(void);
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_ALL_4Gray(const unsigned char *datas);
void EPD_Dis_Part_Time(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                       unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                       unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                       unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                       unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                       unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_PartAll(const unsigned char * datas);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_DeepSleep(void);

#endif // EPD_H
