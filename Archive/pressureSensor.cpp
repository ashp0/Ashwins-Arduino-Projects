/*
  ATMO STATION — BMP085 + WiFi Web Dashboard
  ─────────────────────────────────────────────
  Reads temperature, pressure, and altitude from a BMP085 sensor,
  displays them on an SSD1306 OLED, and serves a live web dashboard
  over WiFi.

  Endpoints:
    /        → HTML dashboard (auto-refreshes every 2 s via fetch)
    /data    → JSON snapshot  { t, p, a, h, b }
    /base    → Sets current pressure as the base (relative height resets)

  Physical:
    Pin 6  → Button (INPUT_PULLUP) — sets base pressure on press
    OLED   → Shows same data on device
*/

#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>

#include "../wifi_secrets.h"

// ── WiFi ────────────────────────────────────────────────────────────────────
char ssid[] = "BELL400";
char pass[] = WIFI_PASSWORD;
int wifiStatus = WL_IDLE_STATUS;
WiFiServer server(80);

// ── Button ───────────────────────────────────────────────────────────────────
const int CONFIGURE_BASE_PIN = 6;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long BUTTON_DEBOUNCE_MS = 10;

// ── Sensor state ─────────────────────────────────────────────────────────────
float air_pressure    = 0.f;
float temperature     = 0.f;
float altitude        = 0.f;
float base_pressure   = 0.f;
float height_filtered = 0.f;
bool  base_pressure_set = false;
bool  bmp_connected     = false;

unsigned long lastBMPSample = 0;
const unsigned long BMP_SAMPLE_INTERVAL_MS = 693;

// ── Hypsometric constants ────────────────────────────────────────────────────
constexpr float TEMP_AT_SEA_LEVEL           = 288.15f;   // K
constexpr float MOLAR_MASS_OF_AIR           = 0.0289644f; // kg/mol
constexpr float ACCELERATION_GRAVITY        = 9.80665f;   // m/s²
constexpr float UNIVERSAL_GAS_CONSTANT      = 8.3144598f; // J/(mol·K)
constexpr float TEMPERATURE_CHANGE_BY_ALTITUDE = 0.0065f; // K/m

// ── Peripherals ──────────────────────────────────────────────────────────────
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 myDisplay(128, 64, &Wire, -1);
Adafruit_BMP085  myBMP;

// ────────────────────────────────────────────────────────────────────────────
//  Helpers
// ────────────────────────────────────────────────────────────────────────────

float computeChangeInAltitude(float currentPressure, float basePressure) {
    float left_term = TEMP_AT_SEA_LEVEL / TEMPERATURE_CHANGE_BY_ALTITUDE;
    float power     = (UNIVERSAL_GAS_CONSTANT * TEMPERATURE_CHANGE_BY_ALTITUDE) /
                      (MOLAR_MASS_OF_AIR * ACCELERATION_GRAVITY);
    return left_term * (1.f - pow(currentPressure / basePressure, power));
}

void printWifiStatus() {
    Serial.print(F("SSID: "));       Serial.println(WiFi.SSID());
    Serial.print(F("IP:   "));       Serial.println(WiFi.localIP());
    Serial.print(F("RSSI: "));       Serial.print(WiFi.RSSI());
    Serial.println(F(" dBm"));
    Serial.print(F("Dashboard → http://"));
    Serial.println(WiFi.localIP());
}

// ────────────────────────────────────────────────────────────────────────────
//  HTTP response helpers
// ────────────────────────────────────────────────────────────────────────────

// Serve the full HTML dashboard page
void serveHTML(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-type:text/html"));
    client.println();

    // ── <head> ──────────────────────────────────────────────────────────────
    client.print(F("<!DOCTYPE html><html lang='en'><head>"));
    client.print(F("<meta charset='utf-8'>"));
    client.print(F("<meta name='viewport' content='width=device-width,initial-scale=1'>"));
    client.print(F("<title>ATMO STATION</title>"));

    // Google Fonts — Share Tech Mono (values) + Teko (headings)
    client.print(F("<link rel='preconnect' href='https://fonts.googleapis.com'>"));
    client.print(F("<link href='https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Teko:wght@300;500&display=swap' rel='stylesheet'>"));

    // ── CSS ─────────────────────────────────────────────────────────────────
    client.print(F("<style>"));
    client.print(F(":root{"
                   "--bg:#07090b;"
                   "--card:#0d1117;"
                   "--border:#1a2420;"
                   "--green:#39ff14;"
                   "--cyan:#00e5ff;"
                   "--amber:#ffb300;"
                   "--dim:#2e4032;"
                   "--text:#c8ffc8;"
                   "}"));

    client.print(F("*{margin:0;padding:0;box-sizing:border-box}"));

    client.print(F("body{"
                   "background:var(--bg);"
                   "color:var(--text);"
                   "font-family:'Share Tech Mono',monospace;"
                   "min-height:100vh;"
                   "padding:24px 16px;"
                   "background-image:radial-gradient(ellipse at 50% 0%,rgba(57,255,20,.06) 0%,transparent 70%);"
                   "}"));

    client.print(F("h1{"
                   "font-family:'Teko',sans-serif;"
                   "font-size:3rem;"
                   "font-weight:500;"
                   "letter-spacing:.4em;"
                   "color:var(--cyan);"
                   "text-align:center;"
                   "text-shadow:0 0 24px rgba(0,229,255,.4);"
                   "margin-bottom:2px;"
                   "}"));

    client.print(F(".sub{"
                   "text-align:center;"
                   "font-size:.6rem;"
                   "color:var(--dim);"
                   "letter-spacing:.3em;"
                   "margin-bottom:28px;"
                   "}"));

    client.print(F(".grid{"
                   "display:grid;"
                   "grid-template-columns:1fr 1fr;"
                   "gap:10px;"
                   "max-width:460px;"
                   "margin:0 auto;"
                   "}"));

    client.print(F(".card{"
                   "background:var(--card);"
                   "border:1px solid var(--border);"
                   "border-radius:3px;"
                   "padding:14px 16px 16px;"
                   "position:relative;"
                   "overflow:hidden;"
                   "transition:border-color .3s;"
                   "}"));

    // Top accent bar
    client.print(F(".card::after{"
                   "content:'';"
                   "position:absolute;"
                   "top:0;left:0;right:0;"
                   "height:2px;"
                   "background:var(--green);"
                   "}"));

    client.print(F(".card.amber::after{background:var(--amber)}"));
    client.print(F(".card.cyan::after{background:var(--cyan)}"));
    client.print(F(".card.wide{grid-column:1/-1}"));
    client.print(F(".card.wide::after{background:linear-gradient(90deg,var(--green),var(--cyan))}"));

    client.print(F(".label{"
                   "font-size:.55rem;"
                   "letter-spacing:.25em;"
                   "color:var(--dim);"
                   "margin-bottom:8px;"
                   "}"));

    client.print(F(".value{"
                   "font-family:'Teko',sans-serif;"
                   "font-size:2.6rem;"
                   "font-weight:300;"
                   "line-height:1;"
                   "color:var(--green);"
                   "}"));

    client.print(F(".card.amber .value{color:var(--amber)}"));
    client.print(F(".card.cyan .value{color:var(--cyan)}"));
    client.print(F(".card.wide .value{font-size:2.8rem}"));

    client.print(F(".unit{"
                   "font-size:.55rem;"
                   "color:var(--dim);"
                   "margin-top:4px;"
                   "letter-spacing:.15em;"
                   "}"));

    client.print(F(".btn{"
                   "display:block;"
                   "margin:14px auto 0;"
                   "max-width:460px;"
                   "width:100%;"
                   "background:transparent;"
                   "border:1px solid var(--green);"
                   "color:var(--green);"
                   "font-family:'Share Tech Mono',monospace;"
                   "font-size:.7rem;"
                   "letter-spacing:.25em;"
                   "padding:10px;"
                   "cursor:pointer;"
                   "border-radius:2px;"
                   "transition:background .2s,color .2s;"
                   "}"));

    client.print(F(".btn:hover{background:var(--green);color:var(--bg)}"));
    client.print(F(".btn:active{opacity:.7}"));

    client.print(F(".footer{"
                   "text-align:center;"
                   "font-size:.55rem;"
                   "color:var(--dim);"
                   "margin-top:14px;"
                   "letter-spacing:.15em;"
                   "}"));

    client.print(F("#dot{"
                   "display:inline-block;"
                   "width:6px;height:6px;"
                   "border-radius:50%;"
                   "background:var(--green);"
                   "margin-right:6px;"
                   "vertical-align:middle;"
                   "animation:blink 1.2s ease-in-out infinite;"
                   "}"));

    client.print(F("@keyframes blink{0%,100%{opacity:1}50%{opacity:.15}}"));
    client.print(F("</style></head><body>"));

    // ── HTML body ────────────────────────────────────────────────────────────
    client.print(F("<h1>ATMO STATION</h1>"));
    client.print(F("<p class='sub'>BMP085 &bull; LIVE TELEMETRY &bull; 2 s REFRESH</p>"));

    client.print(F("<div class='grid'>"));

    // Temperature card
    client.print(F("<div class='card'>"));
    client.print(F("<div class='label'>TEMPERATURE</div>"));
    client.print(F("<div class='value' id='temp'>--.-</div>"));
    client.print(F("<div class='unit'>&deg;CELSIUS</div>"));
    client.print(F("</div>"));

    // Pressure card
    client.print(F("<div class='card amber'>"));
    client.print(F("<div class='label'>AIR PRESSURE</div>"));
    client.print(F("<div class='value' id='pres'>------</div>"));
    client.print(F("<div class='unit'>PASCALS</div>"));
    client.print(F("</div>"));

    // Altitude card
    client.print(F("<div class='card cyan'>"));
    client.print(F("<div class='label'>ALTITUDE ASL</div>"));
    client.print(F("<div class='value' id='alt'>---.-</div>"));
    client.print(F("<div class='unit'>METRES</div>"));
    client.print(F("</div>"));

    // Relative height card (wide)
    client.print(F("<div class='card wide'>"));
    client.print(F("<div class='label'>RELATIVE HEIGHT FROM BASE</div>"));
    client.print(F("<div class='value' id='rel'>--</div>"));
    client.print(F("<div class='unit' id='rel-unit'>SET A BASE PRESSURE TO ENABLE</div>"));
    client.print(F("</div>"));

    client.print(F("</div>")); // .grid

    client.print(F("<button class='btn' onclick='setBase()'>[ SET CURRENT PRESSURE AS BASE ]</button>"));

    client.print(F("<p class='footer'><span id='dot'></span><span id='ts'>AWAITING DATA&hellip;</span></p>"));

    // ── JS polling ───────────────────────────────────────────────────────────
    client.print(F("<script>"));
    client.print(F("function fmt(n,d){return isNaN(n)?'--':n.toFixed(d);}"));

    client.print(F("function update(){"
                   "fetch('/data')"
                   ".then(r=>r.json())"
                   ".then(d=>{"
                   "document.getElementById('temp').textContent=fmt(d.t,1);"
                   "document.getElementById('pres').textContent=Math.round(d.p);"
                   "document.getElementById('alt').textContent=fmt(d.a,1);"
                   "if(d.b){"
                   "document.getElementById('rel').textContent=fmt(d.h,2);"
                   "document.getElementById('rel-unit').textContent='METRES';"
                   "}else{"
                   "document.getElementById('rel').textContent='--';"
                   "document.getElementById('rel-unit').textContent='SET A BASE PRESSURE TO ENABLE';"
                   "}"
                   "var t=new Date().toLocaleTimeString();"
                   "document.getElementById('ts').textContent='LAST UPDATE: '+t;"
                   "})"
                   ".catch(function(){});"
                   "}"));

    client.print(F("update();setInterval(update,2000);"));

    client.print(F("function setBase(){"
                   "fetch('/base')"
                   ".then(r=>r.text())"
                   ".then(function(){"
                   "alert('Base pressure set! Relative height will reset.');"
                   "update();"
                   "})"
                   ".catch(function(){alert('Could not reach device.');});"
                   "}"));

    client.print(F("</script></body></html>"));
    client.println();
}

// Serve live JSON sensor data
void serveJSON(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-type:application/json"));
    client.println(F("Access-Control-Allow-Origin:*"));
    client.println();

    client.print(F("{\"t\":"));  client.print(temperature, 2);
    client.print(F(",\"p\":"));  client.print(air_pressure, 0);
    client.print(F(",\"a\":"));  client.print(altitude, 2);
    client.print(F(",\"h\":"));  client.print(height_filtered, 3);
    client.print(F(",\"b\":"));  client.print(base_pressure_set ? "true" : "false");
    client.println(F("}"));
    client.println();
}

// Serve a plain-text 200 OK
void serveOK(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-type:text/plain"));
    client.println();
    client.println(F("OK"));
    client.println();
}

// ────────────────────────────────────────────────────────────────────────────
//  setup()
// ────────────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(9600);
    pinMode(CONFIGURE_BASE_PIN, INPUT_PULLUP);

    // OLED
    myDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    myDisplay.clearDisplay();
    myDisplay.setTextColor(WHITE);
    myDisplay.setTextSize(1);
    myDisplay.setCursor(0, 0);

    // BMP085
    bmp_connected = myBMP.begin();
    myDisplay.println(bmp_connected ? F("BMP085  OK") : F("BMP085  FAIL"));
    myDisplay.display();

    // WiFi shield check
    if (WiFi.status() == WL_NO_SHIELD) {
        myDisplay.println(F("No WiFi shield"));
        myDisplay.display();
        Serial.println(F("WiFi shield not present — halting."));
        while (true);
    }

    if (WiFi.firmwareVersion() != "1.1.0") {
        Serial.println(F("WiFi firmware update recommended."));
    }

    // Connect
    myDisplay.println(F("Connecting..."));
    myDisplay.display();

    while (wifiStatus != WL_CONNECTED) {
        Serial.print(F("Connecting to ")); Serial.println(ssid);
        wifiStatus = WiFi.begin(ssid, pass);
        delay(10000);
    }

    server.begin();
    printWifiStatus();

    // Show IP on OLED
    myDisplay.clearDisplay();
    myDisplay.setCursor(0, 0);
    myDisplay.println(F("WiFi connected"));
    myDisplay.println(WiFi.localIP());
    myDisplay.display();
}

// ────────────────────────────────────────────────────────────────────────────
//  loop()
// ────────────────────────────────────────────────────────────────────────────

void loop() {
    unsigned long now = millis();

    // ── Physical button (set base pressure) ──────────────────────────────────
    if (bmp_connected) {
        bool btnState = digitalRead(CONFIGURE_BASE_PIN);
        if (btnState != lastButtonState) {
            if (now - lastDebounceTime > BUTTON_DEBOUNCE_MS) {
                if (btnState == LOW) {
                    base_pressure     = air_pressure;
                    base_pressure_set = true;
                    height_filtered   = 0.f;
                    Serial.println(F("Base pressure set via button."));
                }
                lastDebounceTime = now;
            }
        }
        lastButtonState = btnState;
    }

    // ── Sensor sampling ───────────────────────────────────────────────────────
    if (bmp_connected && (now - lastBMPSample > BMP_SAMPLE_INTERVAL_MS)) {
        temperature  = myBMP.readTemperature();
        air_pressure = myBMP.readPressure();
        altitude     = myBMP.readAltitude();

        if (base_pressure_set) {
            float h       = computeChangeInAltitude(air_pressure, base_pressure);
            height_filtered = 0.98f * height_filtered + 0.02f * h;
        }

        // Update OLED
        myDisplay.clearDisplay();
        myDisplay.setCursor(0, 0);
        myDisplay.print(temperature);  myDisplay.println(F(" C"));
        myDisplay.print(altitude);     myDisplay.println(F(" m"));
        myDisplay.print(air_pressure); myDisplay.println(F(" Pa"));
        if (base_pressure_set) {
            myDisplay.println(F("---"));
            myDisplay.print(F("h: "));
            myDisplay.print(height_filtered);
            myDisplay.println(F(" m"));
        }
        myDisplay.display();
        lastBMPSample = now;
    }

    // ── Web server ────────────────────────────────────────────────────────────
    WiFiClient client = server.available();
    if (!client) return;

    Serial.println(F("Client connected"));
    String currentLine = "";

    while (client.connected()) {
        if (!client.available()) continue;

        char c = client.read();

        if (c == '\n') {
            if (currentLine.length() == 0) {
                // End of HTTP headers with no route matched — serve dashboard
                serveHTML(client);
                break;
            }
            currentLine = "";
        } else if (c != '\r') {
            currentLine += c;

            // Route: dashboard
            if (currentLine.endsWith(F("GET / ")) ||
                currentLine.endsWith(F("GET /index.html "))) {
                serveHTML(client);
                break;
            }
            // Route: JSON data feed
            if (currentLine.endsWith(F("GET /data "))) {
                serveJSON(client);
                break;
            }
            // Route: set base pressure via web UI
            if (currentLine.endsWith(F("GET /base "))) {
                base_pressure     = air_pressure;
                base_pressure_set = true;
                height_filtered   = 0.f;
                Serial.println(F("Base pressure set via web."));
                serveOK(client);
                break;
            }
        }
    }

    client.stop();
    Serial.println(F("Client disconnected"));
}
