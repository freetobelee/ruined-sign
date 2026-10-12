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
#define NUM_ANIMATIONS 9

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
  Serial.println("7: Spell (U RUIN. I RUIN. U R RUIND)");
  Serial.println("8: RID / UN");
  Serial.println("9: Pendulum");
  
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
    case 8:
      ridUnAnimation();
      break;
    case 9:
      pendulumAnimation();
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

// Wait while still watching the button and serial; returns false if the animation changed
bool effectWait(unsigned long ms, int anim) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    checkButton();
    checkSerial();
    if (currentAnimation != anim) {
      allOff();
      return false;
    }
    delay(10);
  }
  return true;
}

// Light exactly the letters whose bits are set in mask (bit 0 = R ... bit 5 = D).
// Prints only when something actually changed.
void showMask(uint8_t mask) {
  bool changed = false;
  for (int i = 0; i < NUM_LETTERS; i++) {
    bool state = (mask & (1 << i)) ? LOW : HIGH;
    if (currentPinStates[i] != state) {
      setRelayState(i, state);  // May be held back by MIN_RELAY_DELAY; retried next call
      changed |= currentPinStates[i] == state;
    }
  }
  if (changed) printRuinedState();
}

// Bounce timing. Heights are in letters: 6 = whole sign lit, 0 = dark.
#define BOUNCE_GRAVITY 12.0      // letters/s^2, so the first drop takes 1 s
#define BOUNCE_RESTITUTION 0.72  // Fraction of speed kept on each bounce
#define BOUNCE_MIN_PEAK 0.5      // Stop once a bounce would not light a letter
#define BOUNCE_HOLD_TOP 1500     // Full sign before the drop (ms)
#define BOUNCE_HOLD_REST 1200    // Dark pause after it settles (ms)
#define BOUNCE_TICK 10           // Physics step (ms)

// The lit bar is a ball: the number of letters lit from R is its height.
// It drops from the top under gravity and loses speed on every bounce, so
// each hop is lower and quicker, the bar moves fastest near the floor and
// hangs at the top of each arc.
void fallAndBounce() {
  Serial.println("\n=== BOUNCE ANIMATION ===");

  while (true) {
    showMask(0x3F);
    if (!effectWait(BOUNCE_HOLD_TOP, 1)) return;

    // Each arc is h(t) = h0 + v0*t - g*t^2/2, computed from the arc's start
    // time so there is no drift from summing small steps.
    float h0 = NUM_LETTERS;
    float v0 = 0;
    unsigned long arcStart = millis();
    int bounces = 0;

    while (true) {
      float t = (millis() - arcStart) / 1000.0;
      float h = h0 + v0 * t - 0.5 * BOUNCE_GRAVITY * t * t;

      if (h <= 0) {
        float impact = v0 - BOUNCE_GRAVITY * t;  // Negative: moving down
        v0 = -impact * BOUNCE_RESTITUTION;
        h0 = 0;
        arcStart = millis();
        Serial.print("Bounce #");
        Serial.println(++bounces);
        if (v0 * v0 / (2 * BOUNCE_GRAVITY) < BOUNCE_MIN_PEAK) break;
        h = 0;
      }

      int lit = (int)(h + 0.5);
      if (lit > NUM_LETTERS) lit = NUM_LETTERS;
      showMask((1 << lit) - 1);

      if (!effectWait(BOUNCE_TICK, 1)) return;
    }

    showMask(0);
    if (!effectWait(BOUNCE_HOLD_REST, 1)) return;
  }
}

// Sparkle timing (ms). Each blink and each gap gets a random length.
#define SPARKLE_OFF_MIN 40   // Letters dark for 40-70 ms (avg 55)
#define SPARKLE_OFF_MAX 70
#define SPARKLE_ON_MIN 90    // Then all lit for 90-350 ms (avg 220)
#define SPARKLE_ON_MAX 350

void sparkleAnimation() {
  Serial.println("\n=== SPARKLE ANIMATION ===");

  showMask(0x3F);  // Start with all letters on

  while (true) {
    // Blink 1-3 random letters off
    uint8_t mask = 0x3F;
    int numSparkles = random(1, 4);
    for (int i = 0; i < numSparkles; i++) {
      mask &= ~(1 << random(0, NUM_LETTERS));
    }
    showMask(mask);
    if (!effectWait(random(SPARKLE_OFF_MIN, SPARKLE_OFF_MAX + 1), 4)) return;

    showMask(0x3F);
    if (!effectWait(random(SPARKLE_ON_MIN, SPARKLE_ON_MAX + 1), 4)) return;
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
#define SPELL_WORD_ON 900       // How long each word stays lit
#define SPELL_WORD_GAP 300      // Dark gap between words
#define SPELL_SENTENCE_GAP 1000 // Dark pause at each full stop
#define SPELL_E_DELAY 1000      // RUIND alone before the E glitches
#define SPELL_HOLD 15000        // All on this long before starting over

// Words to spell, as letters of RUINED. An empty string marks a full stop.
// The last word stays lit while the E glitches.
const char* spellWords[] = {
  "U", "RUIN", "",
  "I", "RUIN", "",
  "U", "R", "RUIND"
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
      if (w == NUM_SPELL_WORDS - 1) break;  // Last word stays lit
      allOff();
      if (!spellWait(SPELL_WORD_GAP)) return;
    }

    if (!spellWait(SPELL_E_DELAY - SPELL_WORD_ON)) return;

    // Quick, nervous E glitch, kept above the relay's 20 ms minimum
    int glitches = random(10, 16);
    for (int i = 0; i < glitches; i++) {
      setRelayState(4, LOW);  // E on
      printRuinedState();
      if (!spellWait(random(25, 50))) return;
      setRelayState(4, HIGH);  // E off
      printRuinedState();
      if (!spellWait(random(25, 70))) return;
    }

    // E catches and the whole sign stays lit until the cycle restarts
    setRelayState(4, LOW);
    printRuinedState();
    if (!spellWait(SPELL_HOLD)) return;
  }
}

// RID / UN flash timing (ms)
#define RIDUN_ON 600   // How long each word stays lit
#define RIDUN_GAP 120  // Dark beat between words

#define MASK_RID 0x25  // R, I, D
#define MASK_UN  0x0A  // U, N

// Alternates between RID and UN with a short dark beat between them
void ridUnAnimation() {
  Serial.println("\n=== RID / UN ANIMATION ===");

  while (true) {
    showMask(MASK_RID);
    if (!effectWait(RIDUN_ON, 8)) return;
    showMask(0);
    if (!effectWait(RIDUN_GAP, 8)) return;
    showMask(MASK_UN);
    if (!effectWait(RIDUN_ON, 8)) return;
    showMask(0);
    if (!effectWait(RIDUN_GAP, 8)) return;
  }
}

// Pendulum timing
#define PENDULUM_PERIOD 2.4      // Seconds for one full swing there and back
#define PENDULUM_DAMPING 0.08    // Per second; higher dies out faster
#define PENDULUM_MIN_SWING 0.6   // Letters; below this it comes to rest
#define PENDULUM_REST 1500       // Rest at the bottom before the next push (ms)
#define PENDULUM_TICK 10         // Physics step (ms)

// A single lit letter swings across the sign like a damped pendulum:
// x(t) = A * cos(wt) * e^(-damping*t), centred between I and N. It lingers
// at each end, rushes through the middle and swings a little less each time.
// Once it settles, IN stays lit for a moment and it gets pushed again from R.
void pendulumAnimation() {
  Serial.println("\n=== PENDULUM ANIMATION ===");

  const float center = (NUM_LETTERS - 1) / 2.0;  // 2.5, between I and N
  const float omega = 2 * PI / PENDULUM_PERIOD;

  while (true) {
    unsigned long start = millis();

    while (true) {
      float t = (millis() - start) / 1000.0;
      float amplitude = center * exp(-PENDULUM_DAMPING * t);
      if (amplitude < PENDULUM_MIN_SWING) break;

      // Starts at R (x = 0) and swings toward D
      float x = center - amplitude * cos(omega * t);
      int pos = (int)(x + 0.5);
      showMask(1 << pos);

      if (!effectWait(PENDULUM_TICK, 9)) return;
    }

    showMask((1 << 2) | (1 << 3));  // At rest: I and N
    if (!effectWait(PENDULUM_REST, 9)) return;
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
    if (command >= '1' && command <= '9') {
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
    case 8:
      Serial.println("Now playing: RID / UN ANIMATION");
      break;
    case 9:
      Serial.println("Now playing: PENDULUM ANIMATION");
      break;
  }
  Serial.println("=====================\n");
}

