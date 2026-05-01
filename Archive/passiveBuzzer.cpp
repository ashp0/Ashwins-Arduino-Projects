#include <Arduino.h>

// ── Note definitions (you already had these) ─────────────────
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// ── Buzzer pin ─────────────────────────────────────────────
const int BUZZ = 6;

// ── Tempo (130 BPM, like the original song) ────────────────
const int tempo = 130;                         // beats per minute
const int wholeNote = 240000 / tempo;          // duration of a whole note in ms

// ── Melody: "Baby" by Justin Bieber ───────────────────────
// Notes are taken exactly from the transcription you supplied.
int melody[] = {
  // ── Intro: Oh woah (first block) ──
  NOTE_DS4, NOTE_F4,        // Oh     (D#-F)
  NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_D4, NOTE_C4,   // woah ~ (G-F-D#-D-C)

  NOTE_DS4, NOTE_F4,        // Oh     (D#-F)
  NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_C4,   // woah ~ (G-F-D#-F-C)

  NOTE_DS4, NOTE_F4,        // Oh     (D#-F)
  NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4,            // woah ~ (G-F-D#-F)

  NOTE_F4, NOTE_DS4, NOTE_D4,                     // Oh    ~ (F-D#-D)
  NOTE_D4, NOTE_C4, NOTE_AS3,                     // oh    ~ (D-C-.Bb)
  NOTE_C4, NOTE_AS3, NOTE_C4, NOTE_AS3,           // yeah  ~ (C-.Bb-C-.Bb)

  // ── Verse 1 ──
  NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_G4,   // You know you love me
  NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_F4,            // I know you care
  NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_G4,   // Just shout whenever
  NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_DS4,  // and I'll be there

  // ── Pre-Chorus ──
  NOTE_AS4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_DS4,  // You are my love
  NOTE_AS4, NOTE_AS4, NOTE_G4, NOTE_F4,            // you were my heart
  NOTE_DS4, NOTE_DS4, NOTE_F4,                     // And we will nev-
  NOTE_F4, NOTE_G4, NOTE_F4, NOTE_G4,              // -er, ev-
  NOTE_F4, NOTE_G4, NOTE_F4,                       // -er be a-
  NOTE_G4, NOTE_F4,                                // -part

  NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_G4,   // Are we an item?
  NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_DS4,            // Girl quit playing
  NOTE_AS4, NOTE_F4, NOTE_G4,                      // We're just friends?
  NOTE_DS4, NOTE_DS4, NOTE_DS4, NOTE_F4, NOTE_DS4, // What are you saying

  // ── Pre-Chorus 2 ──
  NOTE_DS4, NOTE_DS4, NOTE_F4, NOTE_GS4, NOTE_G4,  // Said there's another
  NOTE_DS4, NOTE_DS4, NOTE_DS4, NOTE_DS4, NOTE_F4, NOTE_DS4, // looked right in my eyes
  NOTE_DS4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,     // My first love broke my heart
  NOTE_F4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_DS4, NOTE_F4, NOTE_DS4, // first time, and I was like...

  // ── Chorus ──
  NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_AS4,   // Baby, baby, baby oh...
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_C5, // Like baby, baby, baby no...
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_AS4, // Like baby, baby, baby oh...
  NOTE_G4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_DS4 // Thought you'd always be mine, mine...
};

// ── Note durations (4 = quarter, 8 = eighth, 2 = half, etc.) ─
int noteDurations[] = {
  // Intro: Oh woah
  8, 8,    4, 8, 8, 8, 2,       // Oh woah ~ (last note held)
  8, 8,    4, 8, 8, 8, 2,
  8, 8,    4, 8, 8, 4,
  4, 8, 8,                       // Oh ~
  4, 8, 8,                       // oh ~
  8, 8, 8, 4,                    // yeah ~

  // Verse 1
  8, 8, 8, 8, 4,                 // You know you love me
  8, 8, 8, 2,                    // I know you care
  8, 8, 8, 8, 4,                 // Just shout whenever
  8, 8, 8, 8, 2,                 // and I'll be there

  // Pre-Chorus
  8, 8, 8, 8, 3,                 // You are my love
  8, 8, 8, 3,                    // you were my heart
  8, 8, 8,                       // And we will nev-
  8, 8, 8, 8,                    // -er, ev-
  8, 8, 8,                       // -er be a-
  8, 2,                          // -part

  8, 8, 8, 8, 3,                 // Are we an item?
  8, 8, 8, 4,                    // Girl quit playing
  8, 8, 4,                       // We're just friends?
  8, 8, 8, 8, 3,                 // What are you saying

  // Pre-Chorus 2
  8, 8, 8, 8, 5,                 // Said there's another
  8, 8, 8, 8, 8, 2,              // looked right in my eyes
  8, 8, 8, 8, 8, 8,              // My first love broke my heart
  8, 8, 8, 8, 8, 8, 8, 4,        // first time, and I was like...

  // Chorus
  8, 8, 8, 8, 8, 8, 4,           // Baby, baby, baby oh...
  8, 8, 8, 8, 8, 8, 8, 2,        // Like baby, baby, baby no...
  8, 8, 8, 8, 8, 8, 8, 2,        // Like baby, baby, baby oh...
  8, 8, 8, 8, 8, 8, 8, 8, 7, 2   // Thought you'd always be mine, mine...
};

void setup() {
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  // number of notes = size of the melody array
  int numberOfNotes = sizeof(melody) / sizeof(melody[0]);

  for (int thisNote = 0; thisNote < numberOfNotes; thisNote++) {
    // duration of the current note (in ms)
    int noteDuration = wholeNote / noteDurations[thisNote];

    tone(BUZZ, melody[thisNote], noteDuration);

    // pause between notes (makes them distinct)
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    noTone(BUZZ);
  }

  // wait before the whole song repeats
  delay(3000);
}