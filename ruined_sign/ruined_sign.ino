// RUINED SIGN

#define R_PIN 3
#define U_PIN 4
#define I_PIN 5
#define N_PIN 6
#define E_PIN 7
#define D_PIN 8
#define BUTTON_PIN 2

#define NUM_LETTERS 6
#define MIN_RELAY_DELAY 20  // Minimum time (ms) between relay state changes
#define NUM_ANIMATIONS 7

int letterPins[NUM_LETTERS] = {R_PIN, U_PIN, I_PIN, N_PIN, E_PIN, D_PIN};
const char* letters[NUM_LETTERS] = {"R", "U", "I", "N", "E", "D"};

// Track last state change time for each relay
unsigned long lastStateChange[NUM_LETTERS] = {0, 0, 0, 0, 0, 0};
bool currentPinStates[NUM_LETTERS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

int currentAnimation = 1;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
float gravity = 9.8;

void setup() {
  Serial.begin(9600);
  Serial.println("\n=== RUINED Animation System ===");
  Serial.println("Commands:");
  Serial.println("1: Bounce");
  Serial.println("2: Flicker");
  Serial.println("3: Sequential");
  Serial.println("4: Sparkle");
  Serial.println("5: Chase");
  Serial.println("6: Pulse");
  Serial.println("7: Spell (U RUIN. I RUIN. U R RUIN D)");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < NUM_LETTERS; i++) {
    pinMode(letterPins[i], OUTPUT);
    digitalWrite(letterPins[i], HIGH);
  }
  
  printCurrentAnimation();
}

void loop() {
  checkButton();
  checkSerial();
  
  switch(currentAnimation) {
    case 1:
      fallAndBounce();
      break;
    case 2:
      flickerAnimation();
      break;
    case 3:
      sequentialFlash();
      break;
    case 4:
      sparkleAnimation();
      break;
    case 5:
      chaseAnimation();
      break;
    case 6:
      pulseAnimation();
      break;
    case 7:
      spellAnimation();
      break;
  }
}

// Safe relay control function
void setRelayState(int pinIndex, bool state) {
  unsigned long currentTime = millis();
  
  // Check if enough time has passed since last state change
  if (currentTime - lastStateChange[pinIndex] >= MIN_RELAY_DELAY) {
    if (currentPinStates[pinIndex] != state) {  // Only change if state is different
      digitalWrite(letterPins[pinIndex], state);
      lastStateChange[pinIndex] = currentTime;
      currentPinStates[pinIndex] = state;
    }
  }
}

// Utility functions for all letters on/off
void allOff() {
  for (int i = 0; i < NUM_LETTERS; i++) {
    setRelayState(i, HIGH);
  }
  printRuinedState();
}

void allOn() {
  for (int i = 0; i < NUM_LETTERS; i++) {
    setRelayState(i, LOW);
  }
  printRuinedState();
}

void printRuinedState() {
  char word[7] = "ruined";  // Initialize all lowercase
  word[6] = '\0';  // Null terminator
  
  // Update case based on pin states
  for (int i = 0; i < NUM_LETTERS; i++) {
    if (currentPinStates[i] == LOW) {  // If LED is ON
      word[i] = toupper(word[i]);
    }
  }
  
  Serial.println(word);
}

void flickerAnimation() {
  Serial.println("\n=== FLICKER ANIMATION ===");
  
  // Turn all letters on initially
  allOn();
  
  unsigned long lastEFlicker = millis();
  unsigned long lastDFlicker = millis();
  unsigned long nextEFlickerDelay = random(5000, 12000);  // 5-12 seconds
  unsigned long nextDFlickerDelay = random(12000, 18000);  // 12-18 seconds initial delay
  
  while (true) {
    unsigned long currentTime = millis();
    
    // Check E flicker
    if (currentTime - lastEFlicker > nextEFlickerDelay) {
      for (int i = 0; i < random(4, 8); i++) {
        setRelayState(4, HIGH);  // E_PIN
        printRuinedState();
        delay(random(80, 280));
        setRelayState(4, LOW);
        printRuinedState();
        delay(random(80, 280));
      }
      lastEFlicker = millis();  // Use current time, not the old currentTime
      nextEFlickerDelay = random(5000, 12000);
    }
    
    // Check D flicker
    if (currentTime - lastDFlicker > nextDFlickerDelay) {
      setRelayState(5, HIGH);  // D_PIN
      printRuinedState();
      delay(random(100, 200));
      setRelayState(5, LOW);
      printRuinedState();
      lastDFlicker = millis();  // Use current time, not the old currentTime
      nextDFlickerDelay = random(12000, 18000);  // Consistent timing
    }
    
    delay(10);
    
    checkButton();
    checkSerial();
    if (currentAnimation != 2) {
      allOff();
      return;
    }
  }
}

void sequentialFlash() {
  Serial.println("\n=== SEQUENTIAL FLASH ANIMATION ===");
  
  while (true) {
    for (int i = 0; i < NUM_LETTERS; i++) {
      setRelayState(i, LOW);
      printRuinedState();
      delay(800);
      setRelayState(i, HIGH);
      printRuinedState();
      delay(200);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 3) return;
    }
  }
}

void fallAndBounce() {
  Serial.println("\n=== BOUNCE ANIMATION ===");
  int delayTime = 50;
  int height = 100;
  
  for (int i = 0; i < NUM_LETTERS; i++) {
    setRelayState(i, LOW);
    printRuinedState();
    delay(delayTime);
    
    checkButton();
    checkSerial();
    if (currentAnimation != 1) return;
  }
  
  int bounceCount = 1;
  for (int bounce = height; bounce > 0; bounce -= gravity) {
    Serial.print("Bounce #");
    Serial.println(bounceCount++);
    
    for (int i = NUM_LETTERS - 1; i >= 0; i--) {
      setRelayState(i, HIGH);
      printRuinedState();
      delay(delayTime);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 1) return;
    }
    
    delayTime = int(delayTime * 1.5);
    
    for (int i = 0; i < NUM_LETTERS; i++) {
      setRelayState(i, LOW);
      printRuinedState();
      delay(delayTime);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 1) return;
    }
  }
  
  for (int i = NUM_LETTERS - 1; i >= 0; i--) {
    setRelayState(i, HIGH);
    printRuinedState();
    delay(delayTime);
  }
}

void sparkleAnimation() {
  Serial.println("\n=== SPARKLE ANIMATION ===");
  
  allOn();  // Start with all letters on
  
  while (true) {
    // Pick 1-3 random letters to sparkle
    int numSparkles = random(1, 4);
    for (int i = 0; i < numSparkles; i++) {
      int letterIndex = random(0, NUM_LETTERS);
      setRelayState(letterIndex, HIGH);
    }
    printRuinedState();
    delay(50);
    
    allOn();
    printRuinedState();
    delay(random(100, 300));
    
    checkButton();
    checkSerial();
    if (currentAnimation != 4) {
      allOff();
      return;
    }
  }
}

void chaseAnimation() {
  Serial.println("\n=== CHASE ANIMATION ===");
  
  while (true) {
    // Forward chase
    for (int i = 0; i < NUM_LETTERS; i++) {
      setRelayState(i, LOW);  // Turn on current
      if (i > 0) setRelayState(i-1, HIGH);  // Turn off previous
      printRuinedState();
      delay(200);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 5) {
        allOff();
        return;
      }
    }
    
    // Backward chase
    for (int i = NUM_LETTERS-1; i >= 0; i--) {
      setRelayState(i, LOW);  // Turn on current
      if (i < NUM_LETTERS-1) setRelayState(i+1, HIGH);  // Turn off previous
      printRuinedState();
      delay(200);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 5) {
        allOff();
        return;
      }
    }
  }
}

void pulseAnimation() {
  Serial.println("\n=== PULSE ANIMATION ===");
  
  while (true) {
    // Pulse in from edges
    for (int i = 0; i < NUM_LETTERS/2; i++) {
      setRelayState(i, LOW);  // Left side on
      setRelayState(NUM_LETTERS-1-i, LOW);  // Right side on
      printRuinedState();
      delay(300);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 6) {
        allOff();
        return;
      }
    }
    
    delay(500);  // Hold at center
    
    // Pulse out to edges
    for (int i = (NUM_LETTERS/2)-1; i >= 0; i--) {
      setRelayState(i, HIGH);  // Left side off
      setRelayState(NUM_LETTERS-1-i, HIGH);  // Right side off
      printRuinedState();
      delay(300);
      
      checkButton();
      checkSerial();
      if (currentAnimation != 6) {
        allOff();
        return;
      }
    }
    
    delay(500);  // Hold at edges
  }
}

// Spell timing (ms)
#define SPELL_WORD_ON 900      // How long each word stays lit
#define SPELL_WORD_GAP 300     // Dark gap between words
#define SPELL_SENTENCE_GAP 1000 // Dark pause at each full stop
#define SPELL_E_DELAY 1000     // Pause after "D" before the E glitches
#define SPELL_HOLD 15000       // All on before starting over

// Words to spell, as letters of RUINED. An empty string marks a full stop.
const char* spellWords[] = {
  "U", "RUIN", "",
  "I", "RUIN", "",
  "U", "R", "RUIN", "D"
};
#define NUM_SPELL_WORDS (sizeof(spellWords) / sizeof(spellWords[0]))

// Wait while still watching the button and serial; returns false if the animation changed
bool spellWait(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    checkButton();
    checkSerial();
    if (currentAnimation != 7) {
      allOff();
      return false;
    }
    delay(10);
  }
  return true;
}

// Light only the letters in word (e.g. "RUIN")
void showWord(const char* word) {
  for (int i = 0; i < NUM_LETTERS; i++) {
    bool lit = strchr(word, letters[i][0]) != NULL;
    setRelayState(i, lit ? LOW : HIGH);
  }
  printRuinedState();
}

void spellAnimation() {
  Serial.println("\n=== SPELL ANIMATION ===");

  while (true) {
    allOff();
    if (!spellWait(SPELL_SENTENCE_GAP)) return;

    for (unsigned int w = 0; w < NUM_SPELL_WORDS; w++) {
      if (spellWords[w][0] == '\0') {  // Full stop
        if (!spellWait(SPELL_SENTENCE_GAP - SPELL_WORD_GAP)) return;
        continue;
      }
      Serial.println(spellWords[w]);
      showWord(spellWords[w]);
      if (!spellWait(SPELL_WORD_ON)) return;
      allOff();
      if (!spellWait(SPELL_WORD_GAP)) return;
    }

    if (!spellWait(SPELL_E_DELAY)) return;

    // Light everything but E, then glitch the E like a failing tube
    showWord("RUIND");
    if (!spellWait(150)) return;
    int glitches = random(6, 11);
    for (int i = 0; i < glitches; i++) {
      setRelayState(4, LOW);  // E on
      printRuinedState();
      if (!spellWait(random(30, 120))) return;
      setRelayState(4, HIGH);  // E off
      printRuinedState();
      if (!spellWait(random(40, 250))) return;
    }

    // E catches and the whole sign stays on
    allOn();
    if (!spellWait(SPELL_HOLD)) return;
  }
}

void checkButton() {
  int reading = digitalRead(BUTTON_PIN);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {  // Button pressed
        currentAnimation = (currentAnimation % NUM_ANIMATIONS) + 1;
        printCurrentAnimation();
      }
    }
  }
  lastButtonState = reading;
}

void checkSerial() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command >= '1' && command <= '7') {
      currentAnimation = command - '0';
      printCurrentAnimation();
    }
  }
}

void printCurrentAnimation() {
  Serial.println("\n=== Animation Change ===");
  switch(currentAnimation) {
    case 1:
      Serial.println("Now playing: BOUNCE ANIMATION");
      break;
    case 2:
      Serial.println("Now playing: FLICKER ANIMATION");
      break;
    case 3:
      Serial.println("Now playing: SEQUENTIAL FLASH ANIMATION");
      break;
    case 4:
      Serial.println("Now playing: SPARKLE ANIMATION");
      break;
    case 5:
      Serial.println("Now playing: CHASE ANIMATION");
      break;
    case 6:
      Serial.println("Now playing: PULSE ANIMATION");
      break;
    case 7:
      Serial.println("Now playing: SPELL ANIMATION");
      break;
  }
  Serial.println("=====================\n");
}

