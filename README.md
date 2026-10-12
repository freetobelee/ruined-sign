# RUINED Sign

Arduino code for the RUINED letter sign. Each of the six letters is switched by its own relay channel, and a push button (or the serial monitor) cycles through light animations.

## Wiring

| Arduino pin | Connects to |
|---|---|
| D3 | Relay IN for **R** |
| D4 | Relay IN for **U** |
| D5 | Relay IN for **I** |
| D6 | Relay IN for **N** |
| D7 | Relay IN for **E** |
| D8 | Relay IN for **D** |
| D2 | Push button, other side to GND (uses the internal pull-up) |

The relays are driven **active-low**: `LOW` turns a letter on and `HIGH` turns it off, which matches common optocoupled relay boards. All letters start off at power-up. Relay switching is rate-limited to one change per 20 ms per channel to protect the contacts.

## Animations

Press the button to step to the next animation, or send a digit over serial at 9600 baud:

1. Bounce: starts dark, then a ball falls from R to D under gravity, lighting letters as it drops. It rebounds toward R a little lower and quicker each time and settles with the whole sign lit before starting over
2. Flicker (E and D flicker at random like a failing sign)
3. Sequential
4. Sparkle: 1 to 3 random letters blink off, with randomised blink and gap lengths
5. Chase
6. Pulse
7. Spell: lights the letters to spell out "U RUIN. I RUIN. U R RUIND". RUIND stays lit, the E glitches rapidly and then catches, and the whole sign holds for 15 seconds before starting over
8. RIE / UND: flashes between RIE and UND with a short dark beat between them
9. Pendulum: one lit letter swings across the sign like a damped pendulum, lingering at the ends and rushing through the middle, until it settles on IN and gets pushed again

The serial monitor also prints the sign's state as it changes, with lit letters in uppercase (for example `RUInED`).

## Uploading

Open `ruined_sign/ruined_sign.ino` in the Arduino IDE, choose your board and port, and upload.
