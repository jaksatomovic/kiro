#include "font_simple.h"
#include "epd.h"

uint8_t getCharIndex(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A';
  } else if (c >= 'a' && c <= 'z') {
    return c - 'a';  // mala slova koriste iste indekse
  } else if (c >= '0' && c <= '9') {
    return 26 + (c - '0');  // brojevi su nakon slova (26-35)
  } else if (c == ' ') {
    return 36;  // SPACE
  } else if (c == '!') {
    return 37;
  } else if (c == ':') {
    return 38;
  } else if (c == '<') {
    return 39;
  } else if (c == '>') {
    return 40;
  } else if (c == ',') {
    return 41;
  } else if (c == '-') {
    return 42;
  } else if (c == '.') {
    return 43;
  } else {
    return 44;  // unknown character
  }
}

void drawChar(uint8_t* buffer, int x, int y, char c, bool white) {
  if (x < 0 || x >= EPD_WIDTH - FONT_WIDTH || y < 0 || y >= EPD_HEIGHT - FONT_HEIGHT) {
    return;  // out of bounds
  }
  
  uint8_t charIndex = getCharIndex(c);
  if (charIndex >= sizeof(font6x8) / sizeof(font6x8[0])) {
    return;  // invalid character
  }
  
  // Crtaj karakter pixel po pixel - POPRAVLJEN SMIJER
  for (int col = 0; col < FONT_WIDTH; col++) {
    uint8_t line = pgm_read_byte(&font6x8[charIndex][col]);
    for (int row = 0; row < FONT_HEIGHT; row++) {
      if (line & (1 << (FONT_HEIGHT - 1 - row))) {
        // Set pixel at (x + col, y + row) - NORMALNI SMIJER
        int pixelX = x + col;
        int pixelY = y + row;
        
        if (pixelX >= 0 && pixelX < EPD_WIDTH && pixelY >= 0 && pixelY < EPD_HEIGHT) {
          uint16_t byteIndex = (pixelX + pixelY * EPD_WIDTH) / 8;
          uint8_t bit = 7 - (pixelX % 8);  // MSB-first
          
          if (white) {
            buffer[byteIndex] |= (1 << bit);
          } else {
            buffer[byteIndex] &= ~(1 << bit);
          }
        }
      }
    }
  }
}

void drawText(uint8_t* buffer, int x, int y, const char* text, bool white) {
  int currentX = x;
  while (*text) {
    if (*text == '\n') {
      currentX = x;  // reset to start
      y += FONT_HEIGHT + 1;  // move to next line
    } else {
      drawChar(buffer, currentX, y, *text, white);
      currentX += FONT_WIDTH + FONT_SPACING;
    }
    text++;
  }
}

int getTextWidth(const char* text) {
  int width = 0;
  while (*text) {
    if (*text == '\n') {
      break;  // don't count newlines in width
    }
    width += FONT_WIDTH + FONT_SPACING;
    text++;
  }
  if (width > 0) {
    width -= FONT_SPACING;  // remove last spacing
  }
  return width;
}
