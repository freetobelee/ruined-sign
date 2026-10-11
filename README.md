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

1. Bounce
2. Flicker (E and D flicker at random like a failing sign)
3. Sequential
4. Sparkle
5. Chase
6. Pulse

The serial monitor also prints the sign's state as it changes, with lit letters in uppercase (for example `RUInED`).

## Uploading

Open `ruined_sign/ruined_sign.ino` in the Arduino IDE, choose your board and port, and upload.
