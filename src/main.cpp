#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ChronosESP32.h> 
#include <Org_01.h>
#include <SeaDog4sz.h>


// --- Konfigurasi Display OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Inisialisasi Objek Chronos dengan nama perangkat ---
ChronosESP32 chronos("Smart Watch Ben");

// Variabel Global untuk Menyimpan Data dari HP
String currentTime = "00:00";
String currentDate = "ddd, mmm yy";
String currentCity = "Connecting...";
String currentTemp = "00";
int currentPhoneBattery = 0;      // Baterai HP 0-100
bool isPhoneCharging = false;     // Status charging HP
int currentWatchBattery = 100;     // Baterai jam (ESP32) - bisa diisi dengan pembacaan ADC nanti
bool isBleConnected = false;
bool isDayTime = true;
int unreadNotifCount = 0;
String lastNotification = "";
String lastNotifApp = "";

// Data cuaca dan lokasi
WeatherLocation weatherLocation;
Weather currentWeather;

// BITMAP Data (sama persis seperti punyamu)
static const unsigned char PROGMEM image__100baterai_bits[] = {0x3f,0xff,0x00,0x20,0x00,0x80,0x6d,0xb6,0x80,0xcd,0xb6,0x80,0xcd,0xb6,0x80,0xcd,0xb6,0x80,0x6d,0xb6,0x80,0x20,0x00,0x80,0x3f,0xff,0x00};
static const unsigned char PROGMEM image__75baterai_bits[] = {0x3f,0xff,0x00,0x20,0x00,0x80,0x61,0xb6,0x80,0xc1,0xb6,0x80,0xc1,0xb6,0x80,0xc1,0xb6,0x80,0x61,0xb6,0x80,0x20,0x00,0x80,0x3f,0xff,0x00};
static const unsigned char PROGMEM image__50baterai_bits[] = {0x3f,0xff,0x00,0x20,0x00,0x80,0x60,0x36,0x80,0xc0,0x36,0x80,0xc0,0x36,0x80,0xc0,0x36,0x80,0x60,0x36,0x80,0x20,0x00,0x80,0x3f,0xff,0x00};
static const unsigned char PROGMEM image__25baterai_bits[] = {0x3f,0xff,0x00,0x20,0x00,0x80,0x60,0x06,0x80,0xc0,0x06,0x80,0xc0,0x06,0x80,0xc0,0x06,0x80,0x60,0x06,0x80,0x20,0x00,0x80,0x3f,0xff,0x00};
static const unsigned char PROGMEM image_lokasipoint_bits[] = {0x3e,0x00,0x41,0x00,0x80,0x80,0x88,0x80,0x94,0x80,0x88,0x80,0x80,0x80,0x41,0x00,0x22,0x00,0x14,0x00,0x08,0x00};
static const unsigned char PROGMEM image_BLEconnect_bits[] = {0x18,0x00,0x94,0x00,0x52,0x00,0x31,0x00,0x92,0x40,0x5c,0x80,0x92,0x40,0x31,0x00,0x52,0x00,0x94,0x00,0x18,0x00};
static const unsigned char PROGMEM image_BLEdisconnect_bits[] = {0x18,0x00,0x94,0x00,0x52,0x00,0x31,0x00,0x12,0x00,0x1c,0x00,0x12,0x00,0x31,0x00,0x52,0x80,0x94,0x40,0x18,0x00};
static const unsigned char PROGMEM image_sinyal_bits[] = {0x00,0x60,0x00,0x60,0x03,0x60,0x03,0x60,0x1b,0x60,0x1b,0x60,0xdb,0x60,0xdb,0x60};
static const unsigned char PROGMEM image_moon_bits[] = {0x01,0xf0,0x42,0x08,0xa4,0x38,0x48,0x40,0x08,0x80,0x08,0x80,0x08,0x80,0x08,0x40,0x04,0x38,0x12,0x08,0x29,0xf0,0x10,0x00};
static const unsigned char PROGMEM image_sun_bits[] = {0x02,0x00,0x22,0x20,0x10,0x40,0x87,0x08,0x48,0x90,0x10,0x40,0x10,0x40,0x10,0x40,0x48,0x90,0x87,0x08,0x10,0x40,0x22,0x20,0x02,0x00};
static const unsigned char PROGMEM image_notif_bits[] = {0x3f,0xf0,0x40,0x18,0xa0,0x28,0x90,0x48,0x88,0x88,0x97,0x48,0xa0,0x28,0xc0,0x18,0x7f,0xf0};
static const unsigned char PROGMEM image_notifdot_bits[] = {0x00,0x1c,0x3f,0xf4,0x40,0x1c,0xa0,0x28,0x90,0x48,0x88,0x88,0x97,0x48,0xa0,0x28,0xc0,0x18,0x7f,0xf0};
static const unsigned char PROGMEM image_Clock_bits[] = {0x03,0xf0,0x00,0x0f,0xfc,0x00,0x1f,0xfe,0x00,0x38,0x07,0x00,0x7c,0xaf,0x80,0x7d,0x4f,0x80,0xfc,0xaf,0xc0,0xfe,0x5f,0xc0,0xff,0x3f,0xc0,0xff,0x3f,0xc0,0xfe,0xdf,0xc0,0xfd,0xef,0xc0,0x7d,0xef,0x80,0x7d,0xef,0x80,0x38,0x07,0x00,0x1f,0xfe,0x00,0x0f,0xfc,0x00,0x03,0xf0,0x00};

// ============ CALLBACK SESUAI ChronosESP32.h ============

// 1. Callback Connection (parameter bool: true=connected, false=disconnected)
void onConnectionChange(bool state) {
  isBleConnected = state;
  Serial.print("BLE Connection: ");
  Serial.println(state ? "Connected" : "Disconnected");
  
  if (!isBleConnected) {
    currentCity = "Disconnected";
  }
}

// 2. Callback Notification (parameter Notification struct)
void onNotificationReceived(Notification notif) {
  unreadNotifCount++;
  lastNotifApp = notif.app;
  lastNotification = notif.title + ": " + notif.message;
  
  Serial.println("Notif: " + String(notif.app) + " - " + String(notif.title));
}

// 3. Callback Configuration (parameter: Config type, uint32_t value1, uint32_t value2)
void onConfigurationReceived(Config configType, uint32_t value1, uint32_t value2) {
  switch (configType) {
    case CF_TIME: {
      // Ambil string waktu lengkap
      String fullTime = chronos.getTime(); // Format: "HH:MM:SS"

      // Ekstrak jam dan menit dengan substring
      String hourStr = fullTime.substring(0, 2);   // 2 karakter pertama
      String minuteStr = fullTime.substring(3, 5); // 2 karakter setelah ":"

      // Gabungkan
      currentTime = hourStr + ":" + minuteStr;

      // Konversi jam ke integer untuk logika siang/malam
      int hour = hourStr.toInt();  // 16, bukan 4!
  
      // Tentukan siang/malam
      isDayTime = (hour >= 6 && hour < 18);
      
      // Format tanggal
      char dateStr[16];
      const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
      
      sprintf(dateStr, "%s, %02d %s", 
              dayNames[chronos.getDayofWeek()], 
              chronos.getDay(), 
              monthNames[chronos.getMonth()-1]);
      currentDate = String(dateStr);
      
      Serial.print("Hour from parsing: ");
      Serial.println(hour);
      Serial.print("isDayTime: ");
      Serial.println(isDayTime ? "true (siang)" : "false (malam)");
      Serial.println("Time updated: " + currentTime);
      break;
    }
    
    // Tambahkan ini di onConfigurationReceived untuk CF_PBAT
    case CF_PBAT: {
      // JANGAN update currentPhoneBattery dari sini karena selalu 0
      // Tapi kita bisa ambil charging status jika perlu
      // isPhoneCharging = (value2 == 1); // Opsional
      
      Serial.print("CF_PBAT received - Ignoring battery level: ");
      Serial.print(value1);
      Serial.print(", Charging: ");
      Serial.println(value2 == 1 ? "Yes" : "No");
      
      // JANGAN update currentPhoneBattery = value1; <-- HAPUS BARIS INI
      break;
    }
    
    case CF_WEATHER: {
      // Weather config received - ambil data weather dari method library
      weatherLocation = chronos.getWeatherLocation();
      currentCity = weatherLocation.city;
      
      if (chronos.getWeatherCount() > 0) {
        currentWeather = chronos.getWeatherAt(0);
        char tempStr[4];
        sprintf(tempStr, "%d", currentWeather.temp);
        currentTemp = String(tempStr);
      }
      
      Serial.println("Weather updated: " + currentCity + " " + currentTemp + "°C");
      break;
    }
    
    default:
      // Abaikan config lain
      break;
  }
}

// 4. Callback Data (untuk data custom) - FINAL CORRECT VERSION
void onDataReceived(uint8_t *data, int length) {
  Serial.print("Data received, length: ");
  Serial.println(length);
  
  // Data battery selalu length 8
  if (length == 8) {
    // Tampilkan semua byte untuk referensi
    Serial.print("Battery data bytes: ");
    for (int i = 0; i < length; i++) {
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println();
    
    // BYTE KE-7 (index 7) adalah battery level!
    int batteryLevel = data[7];
    
    // Validasi: level harus antara 0-100
    if (batteryLevel >= 0 && batteryLevel <= 100) {
      currentPhoneBattery = batteryLevel;
      
      // Charging flag mungkin di byte lain, tapi kita lihat dulu
      // isPhoneCharging = (data[1] == 1); // Opsional
      
      Serial.print("===== PHONE BATTERY UPDATED ===== Level: ");
      Serial.print(currentPhoneBattery);
      Serial.println("%");
    } else {
      Serial.print("Invalid battery level: ");
      Serial.println(batteryLevel);
    }
  }
}

// 5. Callback Raw Data
void onRawDataReceived(uint8_t *data, int length) {
  // Untuk data mentah, jarang digunakan
}

// 6. Callback Health Request
void onHealthRequest(HealthRequest request, bool start) {
  Serial.print("Health request: ");
  Serial.println(request);
  // Handle health measurements jika diperlukan
}

// 7. Callback Ringer (untuk find phone feature)
void onRingerAlert(String caller, bool state) {
  Serial.print("Ringer: ");
  Serial.print(caller);
  Serial.print(" - ");
  Serial.println(state ? "ON" : "OFF");
}

// Tambahkan di loop atau setup untuk monitoring
void printAllConfigs() {
  Serial.println("=== Current Data ===");
  Serial.println("Time: " + currentTime);
  Serial.println("Date: " + currentDate);
  Serial.println("City: " + currentCity);
  Serial.println("Temp: " + currentTemp);
  Serial.println("Phone Battery: " + String(currentPhoneBattery) + "%");
  Serial.println("BLE Connected: " + String(isBleConnected));
  Serial.println("===================");
}

// ============ SETUP ============

void setup() {
  Serial.begin(115200);
  Serial.println("Smart Watch Starting...");

  // --- Inisialisasi OLED ---
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  display.setRotation(2);
  
  // Tampilkan splash screen
  display.drawBitmap(55, 23, image_Clock_bits, 18, 18, 1);
  display.display();

  // --- Inisialisasi Chronos ---
  chronos.begin();
  
  // Set battery level ESP32 (contoh: 100%, tidak charging)
  chronos.setBattery(100, false);
  
  // Set 24 hour mode
  chronos.set24Hour(true);
  
  // Daftarkan semua callback sesuai header
  chronos.setConnectionCallback(onConnectionChange);
  chronos.setNotificationCallback(onNotificationReceived);
  chronos.setConfigurationCallback(onConfigurationReceived);
  chronos.setDataCallback(onDataReceived);
  chronos.setRawDataCallback(onRawDataReceived);
  chronos.setHealthRequestCallback(onHealthRequest);
  chronos.setRingerCallback(onRingerAlert);
  
  // Set notifikasi baterai HP aktif
  chronos.setNotifyBattery(true);
  
  // Minta sinkronisasi data dari HP
  chronos.syncRequest();
  
  Serial.println("Setup complete. Waiting for connection...");
}

  // Update display setiap loop
void updateDisplay();


// Tambahkan variabel ini di bagian global
unsigned long lastSyncRequest = 0;
unsigned long lastBatteryCheck = 0;
unsigned long lastSecondUpdate = 0;
unsigned long lastDateUpdate = 0;      // TAMBAHKAN INI
unsigned long lastPrintTime = 0;        // TAMBAHKAN INI

// Modifikasi loop()
void loop() {
  chronos.loop();
  
  unsigned long now = millis();
  
    // Update jam internal setiap detik
  if (now - lastSecondUpdate >= 1000) {
    lastSecondUpdate = now;
    
    // Selalu parsing dari getTime()
    String fullTime = chronos.getTime();
    currentTime = fullTime.substring(0, 5);  // "HH:MM"
    
    // Update tanggal setiap 60 detik
    if (now - lastDateUpdate >= 60000) {
      lastDateUpdate = now;
      
      // Parsing jam untuk siang/malam
      int hour = fullTime.substring(0, 2).toInt();
      isDayTime = (hour >= 6 && hour < 18);
      
      // Format tanggal
      char dateStr[16];
      const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
      
      sprintf(dateStr, "%s, %02d %s", 
              dayNames[chronos.getDayofWeek()], 
              chronos.getDay(), 
              monthNames[chronos.getMonth()-1]);
      currentDate = String(dateStr);
    }
  }
  
  // Minta sync setiap 15 menit
  if (now - lastSyncRequest >= 900000) {
    lastSyncRequest = now;
    chronos.syncRequest();
    Serial.println("Manual sync requested");
  }
  
  // Print debug setiap 10 detik (jangan setiap loop!)
  if (now - lastPrintTime >= 10000) {
    lastPrintTime = now;
    printAllConfigs();
  }
  
  // Tambahkan di loop untuk request data baterai lebih sering
  if (now - lastBatteryCheck >= 10000) { // setiap 10 detik
    lastBatteryCheck = now;
    
    // Request data baterai dengan command khusus
    uint8_t cmd[] = {0x0B, 0x01};
    chronos.sendCommand(cmd, 2);
    
    Serial.println("Requesting battery data...");
  }


  updateDisplay();
  delay(50);
}


// ============ UPDATE DISPLAY ============

void drawCenteredText(String text, int y, int textSize, const GFXfont* font) {
  int16_t x1, y1;
  uint16_t w, h;
  
  display.setTextSize(textSize);
  display.setFont(font);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  
  int x = (SCREEN_WIDTH - w) / 2;
  
  // Tambahkan koreksi manual untuk font tertentu
  if (font == &SeaDog_rgEVB4pt7b) {
    x -= 2;  // Geser 2 pixel ke kiri untuk font SeaDog
  }
  if (font == &Org_01) {
    x += 0;  // Tidak perlu koreksi
  }
  
  display.setCursor(x, y);
  display.print(text);
}

void updateDisplay() {
  display.clearDisplay();

  // Gambar frame (tetap sama)
  display.drawLine(126, 3, 126, 60, 1);
  display.drawLine(1, 3, 1, 59, 1);
  display.drawLine(4, 62, 124, 62, 1);
  display.drawLine(4, 1, 124, 1, 1);
  display.drawLine(1, 60, 3, 62, 1);
  display.drawLine(3, 1, 1, 3, 1);
  display.drawLine(124, 1, 126, 3, 1);
  display.drawLine(126, 60, 124, 62, 1);
  display.drawLine(29, 37, 99, 37, 1);
  display.drawLine(29, 38, 99, 38, 1);

  // Icon-icon (tetap sama)
  // Icon Baterai HP
  if (currentPhoneBattery >= 90) {
    display.drawBitmap(106, 5, image__100baterai_bits, 17, 9, 1);
  } else if (currentPhoneBattery >= 65) {
    display.drawBitmap(106, 5, image__75baterai_bits, 17, 9, 1);
  } else if (currentPhoneBattery >= 40) {
    display.drawBitmap(106, 5, image__50baterai_bits, 17, 9, 1);
  } else {
    display.drawBitmap(106, 5, image__25baterai_bits, 17, 9, 1);
  }

  // Icon BLE
  if (isBleConnected) {
    display.drawBitmap(79, 4, image_BLEconnect_bits, 10, 11, 1);
  } else {
    display.drawBitmap(79, 4, image_BLEdisconnect_bits, 10, 11, 1);
  }

  display.drawBitmap(92, 6, image_sinyal_bits, 11, 8, 1);

  // Icon Matahari/Bulan
  if (isDayTime) {
    display.drawBitmap(5, 4, image_sun_bits, 13, 13, 1);
  } else {
    display.drawBitmap(4, 4, image_moon_bits, 13, 12, 1);
  }

  // Icon Notifikasi
  if (unreadNotifCount > 0) {
    display.drawBitmap(110, 49, image_notifdot_bits, 14, 10, 1);
  } else {
    display.drawBitmap(110, 50, image_notif_bits, 13, 9, 1);
  }

  // Icon Lokasi
  display.drawBitmap(4, 49, image_lokasipoint_bits, 9, 11, 1);

  // --- Teks dengan posisi tengah otomatis ---
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  
  // Lokasi (kiri, tidak perlu di tengah)
  display.setFont(&Org_01);
  display.setTextSize(1);
  display.setCursor(16, 56);
  display.print(currentCity);

  // JAM - selalu di tengah horizontal
  drawCenteredText(currentTime, 34, 2, &SeaDog_rgEVB4pt7b);

  // TANGGAL - selalu di tengah horizontal
  drawCenteredText(currentDate, 46, 1, &Org_01);

  // SUHU - di posisi tetap
  display.setFont(&Org_01);
  display.setTextSize(1);
  display.setCursor(20, 12);
  display.print(currentTemp + "'C");

  display.display();
}