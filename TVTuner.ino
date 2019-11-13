#include <Arduboy2.h>
#include <ArduboyTones.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

static const uint8_t tvWidth = 83;
static const uint8_t tvHeight = 64;
static const uint8_t setDisplay = 80;
static const uint8_t blankDelay = 10;

int8_t power = 1;
int8_t source = 1;
int8_t volume = 0;
int8_t channel = 3;
uint32_t lfsr = 1;
uint8_t displayChannel = setDisplay;
uint8_t displayVolume = setDisplay;
uint8_t powerDown = 0;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.clear();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.pollButtons();
  
  if (power) {
    playSound();
    displayStatic();
  
    if (displayChannel > 0) {
      arduboy.setCursor((WIDTH-tvWidth)/2+tvWidth-36,2);
      if (source) {
        arduboy.print(F("CH "));
        if (channel < 10) {
          arduboy.print(' ');      
        }
        arduboy.print(channel);
      }
      else {
        arduboy.print(F("AUX"));      
      }
      displayChannel--;
    }
  
    if (displayVolume > 0) {
      arduboy.setCursor((WIDTH-tvWidth)/2+2,tvHeight-10);
      arduboy.print(F("VOL |"));
      if (volume == 1) {
        arduboy.print(F("|||"));
      }
      else if (volume == 2) {
        arduboy.print(F("|||||||"));
      }
      displayVolume--;
    }
    
    if(arduboy.justPressed(UP_BUTTON)) {
      volume = min(volume+1,2);
      displayVolume = setDisplay;
    }
    if(arduboy.justPressed(DOWN_BUTTON)) {
      volume = max(volume-1,0);
      displayVolume = setDisplay;
    }
    
    if(arduboy.justPressed(RIGHT_BUTTON)) {
      if (source) {
        if (channel++ >= 13) {
          channel = 2;
        }
      }
      displayChannel = setDisplay;
    }
    
    if(arduboy.justPressed(LEFT_BUTTON)) {
      if (source) {
        if (channel-- <= 2) {
          channel = 13;
        }
      }
      displayChannel = setDisplay;
    }
    
    
    if(arduboy.justPressed(A_BUTTON)) {
      source ^= 1;
      displayChannel = setDisplay;
    }
    if(arduboy.justPressed(B_BUTTON)) {
      power ^= 1;
      powerDown = setDisplay;
    }

  }
  else {
    arduboy.clear();
        
    if (powerDown > 0) {
      if (powerDown > (setDisplay-blankDelay)) {
        uint8_t dist = powerDown-(setDisplay-blankDelay);
        arduboy.fillRect(WIDTH/2-dist*4,HEIGHT/2-dist*3,dist*8,dist*6);
      }
      arduboy.drawPixel(WIDTH/2,HEIGHT/2);
      powerDown--;
    }

    if(arduboy.justPressed(B_BUTTON)) {
      power ^= 1;
      displayChannel = setDisplay;
      displayVolume = setDisplay;
    }

  }
  arduboy.display();
}

uint8_t randomByte() {
  return randomWord();
}

uint8_t randomWord() {
  lfsr = (lfsr >> 1) ^ (uint32_t)(0 - (lfsr & 1u) & 0xd0000001u); 
  return lfsr;
}

void displayStatic() {
  uint8_t interleave = arduboy.frameCount % 2 ? 0xaa : 0x55;
  if (displayChannel > 70) {
    interleave = 0;
  }
  for(uint8_t x = (WIDTH-tvWidth)/2; x < (WIDTH+tvWidth)/2; x++) {
    for(uint8_t y = 0; y < HEIGHT/8; y++) {
      uint8_t data = source ? randomByte() : 0xff;
      arduboy.sBuffer[y*WIDTH+x] = data & interleave;
    }
  }
}

void playSound() {
  if ((volume>0) && source && (displayChannel < 70) && !sound.playing()) {
    uint16_t highVolume = (volume == 2) ? TONE_HIGH_VOLUME : 0;
    sound.tone(randomWord() + TONE_HIGH_VOLUME, 10);
  }
}
