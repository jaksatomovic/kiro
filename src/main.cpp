#include <Arduino.h>
#include <AceButton.h>
using namespace ace_button;

constexpr uint8_t BTN_PIN = 10;          // Feather S3 tipka
constexpr bool BTN_ACTIVE_LOW = true;    // tipka spaja na GND
constexpr uint8_t SLIDE_PIN = 9;         // GPIO9 za slide switch

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
  
  Serial.print("[KIRO] AceButton ready - MODE: ");
  Serial.print(CURRENT_MODE);
  if (CURRENT_MODE == 0) {
    Serial.println(" (LOCK - samo very long press 10s)");
  } else {
    Serial.println(" (CLICK/RECORD - single/double/triple + long press 3s)");
  }
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
      }
      // Ako single click nije valjan, jednostavno ga preskoči
    } else if (clickCount == 2) emitEvent("EVENT: DOUBLE_CLICK");
    else if (clickCount >= 3) emitEvent("EVENT: TRIPLE_CLICK");
    clickCount = 0;
  }
}
