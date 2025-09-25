#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

// Pin map for Feather ESP32-S3
#define EPD_BUSY_PIN   6    // BUSY  (input)
#define EPD_RST_PIN    11   // RST   (output)
#define EPD_DC_PIN     12   // DC    (output)
#define EPD_CS_PIN     13   // CS    (output)

//IO settings (zamjena za originalne A14..A17)
#define isEPD_W21_BUSY digitalRead(EPD_BUSY_PIN)
#define EPD_W21_RST_0  digitalWrite(EPD_RST_PIN, LOW)
#define EPD_W21_RST_1  digitalWrite(EPD_RST_PIN, HIGH)
#define EPD_W21_DC_0   digitalWrite(EPD_DC_PIN, LOW)
#define EPD_W21_DC_1   digitalWrite(EPD_DC_PIN, HIGH)
#define EPD_W21_CS_0   digitalWrite(EPD_CS_PIN, LOW)
#define EPD_W21_CS_1   digitalWrite(EPD_CS_PIN, HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);

#endif