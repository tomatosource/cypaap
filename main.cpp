#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"

#define LED_PIN A1     // GPIO pin connected to DIN
#define NUM_LEDS 30    // adjust to your strip length
#define DELAY_MS 1000  // 1 second delay

// Sydney: UTC+10 standard, UTC+11 with DST (Oct → Apr)
#define TZ_SYDNEY "AEST-10AEDT-11,M10.1.0/2,M4.1.0/3"
#define NTP_SERVER "au.pool.ntp.org"

const char* WIFI_SSID = "____slow";
const char* WIFI_PASSWORD = "thewifipassword";

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool toggle = false;

bool connectWiFi(unsigned long timeoutMs = 20000) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(200);
  }
  return WiFi.status() == WL_CONNECTED;
}

static void printTimeOnce(const char* label) {
  struct tm tm;
  if (getLocalTime(&tm, 10)) {
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &tm);
    Serial.printf("%s %s\n", label, buf);
  }
}

// float pct = yearProgressPercent();
float yearProgressPercent() {
  struct tm now;
  if (!getLocalTime(&now)) {
    return -1.0;  // error indicator if time not set
  }

  // Copy "now" and reset to Jan 1 00:00:00 of current year
  struct tm startOfYear = now;
  startOfYear.tm_mon = 0;   // January
  startOfYear.tm_mday = 1;  // 1st
  startOfYear.tm_hour = 0;
  startOfYear.tm_min = 0;
  startOfYear.tm_sec = 0;
  startOfYear.tm_isdst = -1;  // let mktime recalc DST

  // Copy "now" and reset to Jan 1 of next year
  struct tm startOfNextYear = startOfYear;
  startOfNextYear.tm_year += 1;
  startOfNextYear.tm_isdst = -1;  // force recalc DST

  // Also set DST flag for current time
  now.tm_isdst = -1;

  // Convert to time_t (seconds since epoch)
  time_t tNow = mktime(&now);
  time_t tStartOfYear = mktime(&startOfYear);
  time_t tStartOfNext = mktime(&startOfNextYear);

  if (tNow == -1 || tStartOfYear == -1 || tStartOfNext == -1) {
    return -1.0;  // error handling if mktime fails
  }

  double elapsed = difftime(tNow, tStartOfYear);
  double fullYear = difftime(tStartOfNext, tStartOfYear);

  return (elapsed / fullYear) * 100.0;
}


void setup() {
  strip.begin();
  strip.setBrightness(128);
  strip.show();  // Initialize all pixels to 'off'

  Serial.begin(115200);
  delay(100);

  Serial.println("Connecting WiFi…");
  if (!connectWiFi()) {
    Serial.println("WiFi failed. Check credentials or signal.");
  } else {
    Serial.print("WiFi OK: ");
    Serial.println(WiFi.localIP());
  }

  configTime(0, 0, NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", TZ_SYDNEY, 1);    // Set environment variable with your time zone
  tzset();
}



void renderDigit(const int d) {
  const char* digits[10] = {
    //_LLLLBBBBRRRR_rrrrTTTTllllmmmm
    "011111111111101111111111110000",  // 0
    "000000000111101111000000000000",  // 1
    "011111111000001111111100001111",  // 2
    "000001111111101111111100001111",  // 3
    "000000000111101111000011111111",  // 4
    "000001111111100000111111111111",  // 5
    "011111111111100000111111111111",  // 6
    "000000000111101111111100000000",  // 7
    "011111111111101111111111111111",  // 8
    "000001111111101111111111111111",  // 9
  };
  const char* pattern = digits[d];

  for (int i = 0; i < NUM_LEDS; i++) {
    if (pattern[i] == '1') {
      strip.setPixelColor(i, strip.Color(0, 0, 255));  // Blue
    } else {
      strip.setPixelColor(i, 0);  // Off
    }
  }
}

int i = 0;
void loop() {
  renderDigit(i);
  strip.show();
  i = (i + 1) % 10;
  delay(DELAY_MS);
}
