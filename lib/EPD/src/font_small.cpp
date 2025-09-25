#include "font_small.h"
#include "epd.h"

uint8_t getSmallCharIndex(char c) {
  // ASCII karakteri prema SmallFont.c strukturi
  if (c >= 'A' && c <= 'Z') {
    return c - 'A';  // A-Z = 0-25
  } else if (c >= 'a' && c <= 'z') {
    return 26 + (c - 'a');  // a-z = 26-51
  } else if (c >= '0' && c <= '9') {
    return 52 + (c - '0');  // 0-9 = 52-61 (eksplicitno definirani brojevi)
  } else if (c == ' ') {
    return 62;  // SPACE
  } else if (c == '!') {
    return 63;
  } else if (c == '"') {
    return 64;
  } else if (c == '#') {
    return 65;
  } else if (c == '$') {
    return 66;
  } else if (c == '%') {
    return 67;
  } else if (c == '&') {
    return 68;
  } else if (c == '\'') {
    return 69;
  } else if (c == '(') {
    return 70;
  } else if (c == ')') {
    return 71;
  } else if (c == '*') {
    return 72;
  } else if (c == '+') {
    return 73;
  } else if (c == ',') {
    return 74;
  } else if (c == '-') {
    return 75;
  } else if (c == '.') {
    return 76;
  } else if (c == '/') {
    return 77;
  } else {
    return 62;  // SPACE za nepoznate karaktere
  }
}

void drawSmallChar(uint8_t* buffer, int x, int y, char c, bool white) {
  if (x < 0 || x >= EPD_WIDTH - SMALL_FONT_WIDTH || y < 0 || y >= EPD_HEIGHT - SMALL_FONT_HEIGHT) {
    return;  // out of bounds
  }
  
  uint8_t charIndex = getSmallCharIndex(c);
  if (charIndex >= sizeof(smallFont) / sizeof(smallFont[0])) {
    return;  // invalid character
  }
  
  // Crtaj karakter pixel po pixel - SmallFont.c pristup
  for (int col = 0; col < SMALL_FONT_WIDTH; col++) {
    uint8_t line = pgm_read_byte(&smallFont[charIndex][col]);
    for (int row = 0; row < SMALL_FONT_HEIGHT; row++) {
      if (line & (1 << (SMALL_FONT_HEIGHT - 1 - row))) {
        // Set pixel at (x + col, y + row)
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

void drawSmallText(uint8_t* buffer, int x, int y, const char* text, bool white) {
  int currentX = x;
  while (*text) {
    if (*text == '\n') {
      currentX = x;  // reset to start
      y += SMALL_FONT_HEIGHT + 1;  // move to next line
    } else {
      drawSmallChar(buffer, currentX, y, *text, white);
      currentX += SMALL_FONT_WIDTH + SMALL_FONT_SPACING;
    }
    text++;
  }
}

int getSmallTextWidth(const char* text) {
  int width = 0;
  while (*text) {
    if (*text == '\n') {
      break;  // don't count newlines in width
    }
    width += SMALL_FONT_WIDTH + SMALL_FONT_SPACING;
    text++;
  }
  if (width > 0) {
    width -= SMALL_FONT_SPACING;  // remove last spacing
  }
  return width;
}
