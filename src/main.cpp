#include <Arduino.h>
#include <SPI.h>
#include <AceButton.h>
using namespace ace_button;

// E-ink display includes
#include "epd.h"
#include "Ap_29demo.h"
#include "clock.h"

// Pin definicije za Adafruit Feather ESP32-S3
constexpr uint8_t BTN_PIN = 10;          // Feather S3 tipka
constexpr bool BTN_ACTIVE_LOW = true;    // tipka spaja na GND
constexpr uint8_t SLIDE_PIN = 9;         // GPIO9 za slide switch

// E-ink display pinovi su definirani u epd.h

// Pragovi
const unsigned long MULTI_CLICK_WINDOW = 300;  // ms za multi-click
const unsigned long LONG_PRESS_MS = 3000;      // 3s za mode 1
const unsigned long VERY_LONG_PRESS_MS = 10000; // 10s za mode 0
const unsigned long SINGLE_CLICK_MAX_MS = 500; // 2s max za single click

// Globalni state mode (0, 1)
uint8_t CURRENT_MODE = 0;  // Početni mode

AceButton button(BTN_PIN, BTN_ACTIVE_LOW ? LOW : HIGH);

// Interno stanje za multi-click i very-long
uint8_t clickCount = 0;
unsigned long lastClickTime = 0;
unsigned long pressStart = 0;
bool longFired = false;
bool veryLongFired = false;
bool singleClickValid = true; // Da li je single click valjan

// E-ink display state
bool displayInitialized = false;
unsigned long lastDisplayUpdate = 0;
uint8_t currentScreen = 0; // 0=blank, 1=text, 2=image, 3=clock, 4=simple_font, 5=small_font, 6=small_numbers, 7=clock_icon

// Funkcija za ispis eventa s trenutnim modom
void emitEvent(const char* event) {
  Serial.print(event);
  Serial.print(" [MODE: ");
  Serial.print(CURRENT_MODE);
  Serial.println("]");
}

void handleEvent(AceButton* btn, uint8_t eventType, uint8_t buttonState) {
  unsigned long now = millis();

  switch (eventType) {
    case AceButton::kEventPressed:
      pressStart = now;
      longFired = false;
      veryLongFired = false;
      singleClickValid = true; // Reset single click valjanost
      break;

     case AceButton::kEventReleased: {
       if (veryLongFired) {
         emitEvent("EVENT: VERY_LONG_RELEASE");
         // Reset states after very long press
         veryLongFired = false;
         longFired = false;
         pressStart = 0;
       } else if (longFired) {
         emitEvent("EVENT: LONG_RELEASE");
         // Reset states after long press
         longFired = false;
         pressStart = 0;
       } else if (CURRENT_MODE == 1 && singleClickValid) {
         // normal click - samo za mode 1 i ako je single click valjan
         clickCount++;
         lastClickTime = now;
         pressStart = 0;
       } else {
         // Mode 0 - ignoriraj clickove ili single click nije valjan
         pressStart = 0;
       }
     } break;

    case AceButton::kEventLongPressed:
      // Long press samo za mode 1
      if (CURRENT_MODE == 1 && !longFired) {
        longFired = true;
        emitEvent("EVENT: LONG_PRESS");
        Serial.print("DEBUG: Long press detektiran na ");
        Serial.print(now - pressStart);
        Serial.println("ms");
      }
      break;
  }
}

// E-ink display funkcije
void initDisplay() {
  if (displayInitialized) return;
  
  // Init display
  epd_init();
  epd_clear_screen();
  
  displayInitialized = true;
  Serial.println("E-ink display initialized");
}

void updateDisplay() {
  if (!displayInitialized) return;
  
  switch (currentScreen) {
    case 0: // Blank screen
      epd_display_white_screen();
      Serial.println("Display: Blank white screen");
      break;
      
    case 1: // Text screen
      epd_display_image(gImage_1);
      Serial.println("Display: Text screen (gImage_1)");
      break;
      
    case 2: // Image screen
      epd_display_image(gImage_2);
      Serial.println("Display: Image screen (gImage_2)");
      break;
      
    case 3: // Clock screen
      epd_4gray_display(gImage_4G1);
      Serial.println("Display: Clock screen (4-gray gImage_4G1)");
      break;
      
    case 4: // image font test (48x48 font)
      epd_display_image_centered(epd_bitmap_clock_2, 48, 48);
      Serial.println("Display: Clock icon centered (48x48px)");
      break;
      
    case 5: // Large clock icon centered (64x64px)
      epd_display_image_centered(epd_bitmap_clock_3, 64, 64);
      Serial.println("Display: Large clock icon centered (48x48px)");
      break;
      
    case 6: // Small font numbers test - poboljšani brojevi
      epd_display_small_text("0123456789", 10, 80, false);
      Serial.println("Display: Small font numbers test (improved 0-9)");
      break;
      
    case 7: // Small clock icon centered (32x32px)
      epd_display_image_centered(epd_bitmap_clock, 32, 32);
      Serial.println("Display: Small clock icon centered (32x32px)");
      break;
  }
  
  lastDisplayUpdate = millis();
}

void cycleDisplay() {
  currentScreen = (currentScreen + 1) % 8;  // 0-7 screens
  updateDisplay();
  Serial.print("Display screen changed to: ");
  Serial.println(currentScreen);
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(SLIDE_PIN, INPUT_PULLUP);  // Slide switch s pullup
  button.setEventHandler(handleEvent);
  
   // Configure button timing
   auto* cfg = button.getButtonConfig();
   cfg->setDebounceDelay(10); // 10ms debounce
   cfg->setLongPressDelay(LONG_PRESS_MS); // 3s za mode 1
   cfg->setRepeatPressDelay(0); // Disable repeat
   cfg->setRepeatPressInterval(0); // Disable repeat
   cfg->setClickDelay(MULTI_CLICK_WINDOW); // 300ms za multi-click
   cfg->setDoubleClickDelay(MULTI_CLICK_WINDOW); // 300ms za double click
   
   // Enable features
   cfg->setFeature(ButtonConfig::kFeatureClick);
   cfg->setFeature(ButtonConfig::kFeatureDoubleClick);
   cfg->setFeature(ButtonConfig::kFeatureLongPress);
   cfg->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
  
  Serial.begin(115200);
  
  // Čitaj slide switch i postavi mode
  CURRENT_MODE = digitalRead(SLIDE_PIN) ? 1 : 0;
  
  Serial.println("=== KIRO E-INK DISPLAY CONTROLLER ===");
  Serial.print("MODE: ");
  Serial.print(CURRENT_MODE);
  if (CURRENT_MODE == 0) {
    Serial.println(" (LOCK MODE)");
    Serial.println("  - Samo VERY LONG PRESS (10s) → Clear display");
    Serial.println("  - Svi ostali clickovi se ignoriraju");
  } else {
    Serial.println(" (CLICK/RECORD MODE)");
    Serial.println("  - SINGLE CLICK → Cycle display screens (0-7)");
    Serial.println("  - DOUBLE CLICK → Show text screen");
    Serial.println("  - TRIPLE CLICK → Show clock screen");
    Serial.println("  - LONG PRESS (3s) → Show image screen");
    Serial.println("  Screens: 0=blank, 1=text, 2=image, 3=clock, 4=large_clock, 5=large_clock, 6=small_numbers, 7=small_clock");
  }
  Serial.println("=====================================");
  
  // Initialize display
  initDisplay();
  updateDisplay();
}

void loop() {
  button.check();

  unsigned long now = millis();
  
  // Čitaj slide switch i ažuriraj mode
  uint8_t newMode = digitalRead(SLIDE_PIN) ? 1 : 0;
  if (newMode != CURRENT_MODE) {
    CURRENT_MODE = newMode;
    Serial.print("DEBUG: Mode promijenjen na ");
    Serial.print(CURRENT_MODE);
    if (CURRENT_MODE == 0) {
      Serial.println(" (LOCK)");
    } else {
      Serial.println(" (CLICK/RECORD)");
    }
  }

   // Provjera single click valjanosti (samo za mode 1)
   if (CURRENT_MODE == 1 && singleClickValid && button.isPressedRaw() && pressStart > 0) {
     if ((now - pressStart) >= SINGLE_CLICK_MAX_MS) {
       singleClickValid = false;
     }
   }

   // Very long press detekcija (samo za mode 0)
   if (CURRENT_MODE == 0 && !veryLongFired && button.isPressedRaw() && pressStart > 0) {
     if ((now - pressStart) >= VERY_LONG_PRESS_MS) {
       veryLongFired = true;
       emitEvent("EVENT: VERY_LONG_PRESS");
       Serial.print("DEBUG: Very long press detektiran na ");
       Serial.print(now - pressStart);
       Serial.println("ms");
     }
   }

  // Multi-click obrada (single/double/triple) - samo za mode 1
  if (CURRENT_MODE == 1 && clickCount > 0 && (now - lastClickTime) > MULTI_CLICK_WINDOW) {
    if (clickCount == 1) {
      if (singleClickValid) {
        emitEvent("EVENT: SINGLE_CLICK");
        Serial.println("  → Action: Cycle display screens (0→1→2→3→4→5→6→7→0...)");
        cycleDisplay(); // Change display screen
      } else {
        Serial.println("  → Single click invalid (too long press)");
      }
    } else if (clickCount == 2) {
      emitEvent("EVENT: DOUBLE_CLICK");
      Serial.println("  → Action: Show text screen (gImage_1)");
      currentScreen = 1;
      updateDisplay();
    } else if (clickCount >= 3) {
      emitEvent("EVENT: TRIPLE_CLICK");
      Serial.println("  → Action: Show clock screen (4-gray)");
      currentScreen = 3;
      updateDisplay();
    }
    clickCount = 0;
  }
  
  // Long press action (samo za mode 1)
  if (CURRENT_MODE == 1 && longFired) {
    Serial.println("  → Action: Show image screen (gImage_2)");
    currentScreen = 2;
    updateDisplay();
  }
  
  // Very long press action (samo za mode 0)
  if (CURRENT_MODE == 0 && veryLongFired) {
    Serial.println("  → Action: Clear display (blank white screen)");
    currentScreen = 0;
    updateDisplay();
  }
}