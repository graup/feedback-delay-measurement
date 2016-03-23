# How to use

1. Assemble hardware
  - You need the printed circuit board and a few parts. You can also put everything together on a breadboard if you don't like soldering.
  - Parts needed: Arduino Micro, LTC1050, BPW45, capacitor 10nF and 15pF, resistor 380 kOhm.

2. Connect Arduino via USB. Open the program code in the Arduino IDE. Have a look at the file, it has some additional explanations and settings.

3. Place the board onto the screen you want to measure. Connect the trigger pin to the touch screen if you want to use it.

4. Upload the sketch to the Arduino. Follow the program instructions.

5. When everything is set up and tested, start the measurement. Use the Arduino Serial Monitor to get the results. The Arduino will print one measurement per line via Serial.

6. Put the numbers into your favorite statistics software.

# Any questions?

Everything is explained in detail in my thesis, but please don't hesitate to contact me with any further questions!

[Paul Grau](http://graycoding.com), March 2016
