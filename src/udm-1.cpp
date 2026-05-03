#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <WiFiS3.h>
#include <Wire.h>

#include "../wifi_secrets.h"

// ─── PULL UP BUTTON ─────────────────────────────────────────────────────────
const int music_cycle_pin = 6;

// Button debounce state
bool lastButtonState = HIGH;  // INPUT_PULLUP idles HIGH
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// Cycle state: -1 = stopped, 0/1/2 = melody slot
const int NUM_MELODIES = 3;
int currentSlot = -1;  // start stopped

// ─── UDP ────────────────────────────────────────────────────────────────────
WiFiUDP myUDP;
const int UDP_PORT = 12345;
char myPacket[255];
int dataLength;

// ─── OLED ───────────────────────────────────────────────────────────────────
const int screen_width = 128;
const int screen_height = 64;
const int OLED_reset = -1;
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 myDisplay(screen_width, screen_height, &Wire, OLED_reset);

// ─── BUZZER / TEMPO ─────────────────────────────────────────────────────────
const int buzzer = 3;
const int tempo = 114;
const int wholenote = (60000 * 4) / tempo;

// ─── MELODIES (PROGMEM) ─────────────────────────────────────────────────────
// Each melody is an array of (pitch, duration) pairs.
// Negative duration = dotted note.  REST = 0.

const int melody_cannon_in_d[] PROGMEM = {
	NOTE_FS4, 2, NOTE_E4,  2, NOTE_D4,	2, NOTE_CS4, 2, NOTE_B3,  2, NOTE_A3,  2, NOTE_B3,	2,
	NOTE_CS4, 2, NOTE_FS4, 2, NOTE_E4,	2, NOTE_D4,	 2, NOTE_CS4, 2, NOTE_B3,  2, NOTE_A3,	2,
	NOTE_B3,  2, NOTE_CS4, 2, NOTE_D4,	2, NOTE_CS4, 2, NOTE_B3,  2, NOTE_A3,  2, NOTE_G3,	2,
	NOTE_FS3, 2, NOTE_G3,  2, NOTE_A3,	2,

	NOTE_D4,  4, NOTE_FS4, 8, NOTE_G4,	8, NOTE_A4,	 4, NOTE_FS4, 8, NOTE_G4,  8, NOTE_A4,	4,
	NOTE_B3,  8, NOTE_CS4, 8, NOTE_D4,	8, NOTE_E4,	 8, NOTE_FS4, 8, NOTE_G4,  8, NOTE_FS4, 4,
	NOTE_D4,  8, NOTE_E4,  8, NOTE_FS4, 4, NOTE_FS3, 8, NOTE_G3,  8, NOTE_A3,  8, NOTE_G3,	8,
	NOTE_FS3, 8, NOTE_G3,  8, NOTE_A3,	2, NOTE_G3,	 4, NOTE_B3,  8, NOTE_A3,  8, NOTE_G3,	4,
	NOTE_FS3, 8, NOTE_E3,  8, NOTE_FS3, 4, NOTE_D3,	 8, NOTE_E3,  8, NOTE_FS3, 8, NOTE_G3,	8,
	NOTE_A3,  8, NOTE_B3,  8,

	NOTE_G3,  4, NOTE_B3,  8, NOTE_A3,	8, NOTE_B3,	 4, NOTE_CS4, 8, NOTE_D4,  8, NOTE_A3,	8,
	NOTE_B3,  8, NOTE_CS4, 8, NOTE_D4,	8, NOTE_E4,	 8, NOTE_FS4, 8, NOTE_G4,  8, NOTE_A4,	2,
	NOTE_A4,  4, NOTE_FS4, 8, NOTE_G4,	8, NOTE_A4,	 4, NOTE_FS4, 8, NOTE_G4,  8, NOTE_A4,	8,
	NOTE_A3,  8, NOTE_B3,  8, NOTE_CS4, 8, NOTE_D4,	 8, NOTE_E4,  8, NOTE_FS4, 8, NOTE_G4,	8,
	NOTE_FS4, 4, NOTE_D4,  8, NOTE_E4,	8, NOTE_FS4, 8, NOTE_CS4, 8, NOTE_A3,  8, NOTE_A3,	8,

	NOTE_CS4, 4, NOTE_B3,  4, NOTE_D4,	8, NOTE_CS4, 8, NOTE_B3,  4, NOTE_A3,  8, NOTE_G3,	8,
	NOTE_A3,  4, NOTE_D3,  8, NOTE_E3,	8, NOTE_FS3, 8, NOTE_G3,  8, NOTE_A3,  8, NOTE_B3,	4,
	NOTE_G3,  4, NOTE_B3,  8, NOTE_A3,	8, NOTE_B3,	 4, NOTE_CS4, 8, NOTE_D4,  8, NOTE_A3,	8,
	NOTE_B3,  8, NOTE_CS4, 8, NOTE_D4,	8, NOTE_E4,	 8, NOTE_FS4, 8, NOTE_G4,  8, NOTE_A4,	2,
};

const int melody_never_gonna_give_you_up[] PROGMEM = {
	NOTE_D5,  -4, NOTE_E5,	-4, NOTE_A4,  4,  NOTE_E5,	-4, NOTE_FS5, -4, NOTE_A5,	16,
	NOTE_G5,  16, NOTE_FS5, 8,	NOTE_D5,  -4, NOTE_E5,	-4, NOTE_A4,  2,  NOTE_A4,	16,
	NOTE_A4,  16, NOTE_B4,	16, NOTE_D5,  8,  NOTE_D5,	16, NOTE_D5,  -4, NOTE_E5,	-4,
	NOTE_A4,  4,  NOTE_E5,	-4, NOTE_FS5, -4, NOTE_A5,	16, NOTE_G5,  16, NOTE_FS5, 8,
	NOTE_D5,  -4, NOTE_E5,	-4, NOTE_A4,  2,  NOTE_A4,	16, NOTE_A4,  16, NOTE_B4,	16,
	NOTE_D5,  8,  NOTE_D5,	16, REST,	  4,  NOTE_B4,	8,	NOTE_CS5, 8,  NOTE_D5,	8,
	NOTE_D5,  8,  NOTE_E5,	8,	NOTE_CS5, -8, NOTE_B4,	16, NOTE_A4,  2,  REST,		4,

	REST,	  8,  NOTE_B4,	8,	NOTE_B4,  8,  NOTE_CS5, 8,	NOTE_D5,  8,  NOTE_B4,	4,
	NOTE_A4,  8,  NOTE_A5,	8,	REST,	  8,  NOTE_A5,	8,	NOTE_E5,  -4, REST,		4,
	NOTE_B4,  8,  NOTE_B4,	8,	NOTE_CS5, 8,  NOTE_D5,	8,	NOTE_B4,  8,  NOTE_D5,	8,
	NOTE_E5,  8,  REST,		8,	REST,	  8,  NOTE_CS5, 8,	NOTE_B4,  8,  NOTE_A4,	-4,
	REST,	  4,  REST,		8,	NOTE_B4,  8,  NOTE_B4,	8,	NOTE_CS5, 8,  NOTE_D5,	8,
	NOTE_B4,  8,  NOTE_A4,	4,	NOTE_E5,  8,  NOTE_E5,	8,	NOTE_E5,  8,  NOTE_FS5, 8,
	NOTE_E5,  4,  REST,		4,

	NOTE_D5,  2,  NOTE_E5,	8,	NOTE_FS5, 8,  NOTE_D5,	8,	NOTE_E5,  8,  NOTE_E5,	8,
	NOTE_E5,  8,  NOTE_FS5, 8,	NOTE_E5,  4,  NOTE_A4,	4,	REST,	  2,  NOTE_B4,	8,
	NOTE_CS5, 8,  NOTE_D5,	8,	NOTE_B4,  8,  REST,		8,	NOTE_E5,  8,  NOTE_FS5, 8,
	NOTE_E5,  -4, NOTE_A4,	16, NOTE_B4,  16, NOTE_D5,	16, NOTE_B4,  16, NOTE_FS5, -8,
	NOTE_FS5, -8, NOTE_E5,	-4, NOTE_A4,  16, NOTE_B4,	16, NOTE_D5,  16, NOTE_B4,	16,

	NOTE_E5,  -8, NOTE_E5,	-8, NOTE_D5,  -8, NOTE_CS5, 16, NOTE_B4,  -8, NOTE_A4,	16,
	NOTE_B4,  16, NOTE_D5,	16, NOTE_B4,  16, NOTE_D5,	4,	NOTE_E5,  8,  NOTE_CS5, -8,
	NOTE_B4,  16, NOTE_A4,	8,	NOTE_A4,  8,  NOTE_A4,	8,	NOTE_E5,  4,  NOTE_D5,	2,
	NOTE_A4,  16, NOTE_B4,	16, NOTE_D5,  16, NOTE_B4,	16, NOTE_FS5, -8, NOTE_FS5, -8,
	NOTE_E5,  -4, NOTE_A4,	16, NOTE_B4,  16, NOTE_D5,	16, NOTE_B4,  16, NOTE_A5,	4,
	NOTE_CS5, 8,  NOTE_D5,	-8, NOTE_CS5, 16, NOTE_B4,	8,	NOTE_A4,  16, NOTE_B4,	16,
	NOTE_D5,  16, NOTE_B4,	16,

	NOTE_D5,  4,  NOTE_E5,	8,	NOTE_CS5, -8, NOTE_B4,	16, NOTE_A4,  4,  NOTE_A4,	8,
	NOTE_E5,  4,  NOTE_D5,	2,	REST,	  4,
};

const int melody_fur_elise[] PROGMEM = {
	NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4,  16, NOTE_D5,	 16,
	NOTE_C5,  16, NOTE_A4,	-8, NOTE_C4,  16, NOTE_E4,	16, NOTE_A4, 16, NOTE_B4,  -8, NOTE_E4,	 16,
	NOTE_GS4, 16, NOTE_B4,	16, NOTE_C5,  8,  REST,		16, NOTE_E4, 16, NOTE_E5,  16, NOTE_DS5, 16,
	NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5,  16, NOTE_B4,	16, NOTE_D5, 16, NOTE_C5,  16, NOTE_A4,	 -8,
	NOTE_C4,  16, NOTE_E4,	16, NOTE_A4,  16, NOTE_B4,	-8, NOTE_E4, 16, NOTE_C5,  16, NOTE_B4,	 16,
	NOTE_A4,  4,  REST,		8,	NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5,	 16,
	NOTE_B4,  16, NOTE_D5,	16, NOTE_C5,  16, NOTE_A4,	-8, NOTE_C4, 16, NOTE_E4,  16, NOTE_A4,	 16,
	NOTE_B4,  -8, NOTE_E4,	16, NOTE_GS4, 16, NOTE_B4,	16, NOTE_C5, 8,	 REST,	   16, NOTE_E4,	 16,
	NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5,  16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4,  16, NOTE_D5,	 16,
	NOTE_C5,  16, NOTE_A4,	-8, NOTE_C4,  16, NOTE_E4,	16, NOTE_A4, 16, NOTE_B4,  -8, NOTE_E4,	 16,
	NOTE_C5,  16, NOTE_B4,	16, NOTE_A4,  8,  REST,		16, NOTE_B4, 16, NOTE_C5,  16, NOTE_D5,	 16,
	NOTE_E5,  -8, NOTE_G4,	16, NOTE_F5,  16, NOTE_E5,	16, NOTE_D5, -8, NOTE_F4,  16, NOTE_E5,	 16,
	NOTE_D5,  16, NOTE_C5,	-8, NOTE_E4,  16, NOTE_D5,	16, NOTE_C5, 16, NOTE_B4,  8,  REST,	 16,
	NOTE_E4,  16, NOTE_E5,	16, REST,	  16, REST,		16, NOTE_E5, 16, NOTE_E6,  16, REST,	 16,
	REST,	  16, NOTE_DS5, 16, NOTE_E5,  16, REST,		16, REST,	 16, NOTE_DS5, 16, NOTE_E5,	 16,
	NOTE_DS5, 16, NOTE_E5,	16, NOTE_DS5, 16, NOTE_E5,	16, NOTE_B4, 16, NOTE_D5,  16, NOTE_C5,	 16,
	NOTE_A4,  8,  REST,		16, NOTE_C4,  16, NOTE_E4,	16, NOTE_A4, 16, NOTE_B4,  8,  REST,	 16,
	NOTE_E4,  16, NOTE_GS4, 16, NOTE_B4,  16, NOTE_C5,	8,	REST,	 16, NOTE_E4,  16, NOTE_E5,	 16,
	NOTE_DS5, 16, NOTE_E5,	16, NOTE_DS5, 16, NOTE_E5,	16, NOTE_B4, 16, NOTE_D5,  16, NOTE_C5,	 16,
	NOTE_A4,  8,  REST,		16, NOTE_C4,  16, NOTE_E4,	16, NOTE_A4, 16, NOTE_B4,  8,  REST,	 16,
	NOTE_E4,  16, NOTE_C5,	16, NOTE_B4,  16, NOTE_A4,	-4,
};

// ─── MELODY METADATA ────────────────────────────────────────────────────────
struct MelodyInfo {
	const int* data;
	int length;	 // total number of ints (pairs * 2)
	const char* name;
};

const MelodyInfo melodies[] = {
	{melody_never_gonna_give_you_up, (int)(sizeof(melody_never_gonna_give_you_up) / sizeof(int)),
	 "Never Gonna Give You Up"},
	{melody_fur_elise, (int)(sizeof(melody_fur_elise) / sizeof(int)), "Fur Elise"},
	{melody_cannon_in_d, (int)(sizeof(melody_cannon_in_d) / sizeof(int)), "Cannon in D"},
};

// ─── PLAYBACK STATE ─────────────────────────────────────────────────────────
const int* activeMelody = nullptr;
int activeMelodyLen = 0;
int noteIndex = 0;	// index into the int array (0, 2, 4, …)
bool playing = false;
unsigned long noteDeadline = 0;	 // millis() when the current note ends

// Read one int from PROGMEM (works on both AVR 16-bit int and ARM 32-bit int)
static inline int readMelodyInt(const int* ptr, int idx) {
#if defined(__AVR__)
	return (int)(int16_t)pgm_read_word_near(ptr + idx);
#else
	// On ARM (UNO R4, etc.) PROGMEM is a no-op; direct access is fine.
	return ptr[idx];
#endif
}

void startMelody(int slot) {
	// slot is 0-based
	activeMelody = melodies[slot].data;
	activeMelodyLen = melodies[slot].length;
	noteIndex = 0;
	playing = true;
	noteDeadline = 0;  // trigger immediately

	myDisplay.clearDisplay();
	myDisplay.setCursor(0, 0);
	myDisplay.println("Now playing:");
	myDisplay.println(melodies[slot].name);
	myDisplay.display();
}

void stopPlayback() {
	playing = false;
	activeMelody = nullptr;
	noTone(buzzer);

	myDisplay.clearDisplay();
	myDisplay.setCursor(0, 0);
	myDisplay.println("Stopped.");
	myDisplay.display();
}

// Call this from loop() — advances one note at a time, non-blocking.
void updatePlayback() {
	if (!playing || activeMelody == nullptr)
		return;

	if (millis() < noteDeadline)
		return;	 // still within current note

	// Finished melody?
	if (noteIndex >= activeMelodyLen) {
		stopPlayback();
		return;
	}

	int pitch = readMelodyInt(activeMelody, noteIndex);
	int divider = readMelodyInt(activeMelody, noteIndex + 1);
	noteIndex += 2;

	int noteDuration;
	if (divider > 0) {
		noteDuration = wholenote / divider;
	} else {
		noteDuration = (wholenote / abs(divider)) * 3 / 2;	// dotted note
	}

	if (pitch == REST) {
		noTone(buzzer);
	} else {
		tone(buzzer, pitch, (unsigned long)(noteDuration * 0.9f));
	}

	noteDeadline = millis() + (unsigned long)noteDuration;
}

// ─── SETUP ──────────────────────────────────────────────────────────────────
void setup() {
	Serial.begin(9600);
	pinMode(9, OUTPUT);
	pinMode(music_cycle_pin, INPUT_PULLUP);

	// OLED
	myDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
	myDisplay.clearDisplay();
	myDisplay.display();
	myDisplay.setCursor(0, 0);
	myDisplay.setTextColor(WHITE);
	myDisplay.setTextSize(1);
	myDisplay.println("Connecting...");
	myDisplay.display();

	// Wi-Fi
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	Serial.print("\nConnected.  IP: ");
	Serial.println(WiFi.localIP());

	myDisplay.clearDisplay();
	myDisplay.setCursor(0, 0);
	myDisplay.println("WiFi OK");
	myDisplay.println(WiFi.localIP());
	myDisplay.println("Send 1/2/3 or stop");
	myDisplay.display();

	myUDP.begin(UDP_PORT);
	Serial.print("UDP listening on port ");
	Serial.println(UDP_PORT);
}

void handleButton() {
	bool reading = digitalRead(music_cycle_pin);

	// Any change restarts the debounce timer
	if (reading != lastButtonState) {
		lastDebounce = millis();
		lastButtonState = reading;
	}

	// Wait for the signal to be stable
	if ((millis() - lastDebounce) < DEBOUNCE_MS)
		return;

	// Act only on the falling edge (HIGH → LOW = button pressed with pull-up)
	static bool stableState = HIGH;
	if (reading == LOW && stableState == HIGH) {
		stableState = LOW;

		// Advance the cycle
		currentSlot = (currentSlot + 1) % (NUM_MELODIES + 1);  // +1 for the stop state

		if (currentSlot < NUM_MELODIES) {
			startMelody(currentSlot);
		} else {
			currentSlot = -1;  // wrap to stopped
			stopPlayback();
		}
	} else if (reading == HIGH) {
		stableState = HIGH;	 // button released, ready for next press
	}
}

// ─── LOOP ───────────────────────────────────────────────────────────────────
void loop() {
	handleButton();
	// ── Non-blocking melody tick ──────────────────────────────────────────
	updatePlayback();

	// ── UDP receive ───────────────────────────────────────────────────────
	if (!myUDP.parsePacket())
		return;

	dataLength = myUDP.available();
	myUDP.read(myPacket, sizeof(myPacket) - 1);
	myPacket[dataLength] = '\0';

	String cmd = String(myPacket);
	cmd.trim();
	cmd.toLowerCase();

	Serial.print("CMD: ");
	Serial.println(cmd);

	String response;

	if (cmd == "1") {
		startMelody(0);
		response = "Playing: Never Gonna Give You Up";

	} else if (cmd == "2") {
		startMelody(1);
		response = "Playing: Fur Elise";

	} else if (cmd == "3") {
		startMelody(2);
		response = "Playing: Cannon in D";

	} else if (cmd == "stop") {
		stopPlayback();
		response = "Playback stopped.";

	} else if (cmd == "clear") {
		myDisplay.clearDisplay();
		myDisplay.setCursor(0, 0);
		myDisplay.display();
		response = "Display cleared.";

	} else {
		// Pass-through: show on display (original marble behaviour)
		myDisplay.println(myPacket);
		myDisplay.display();
		response = "Here is your " + String(myPacket) + " marble";
	}

	// Send reply
	myUDP.beginPacket(myUDP.remoteIP(), myUDP.remotePort());
	myUDP.print(response);
	myUDP.endPacket();
}
