# FrequencyGenerator_Arduino_Si5351

This project help you to build a Frequency Generator using a Si5351 Module on Arduino (UNO R3, Nano, etc...)
The schematic attached use a LCD 20x2 compatible Hitachi HD44780 with 74HC595 shift register integrated circuit (see ["how drive a HD44780 LCD using a 74HC595 shift register"](https://www.google.com/search?q=how+drive+a+HD44780+LCD+using+a+74HC595+shift+register))

### Software Requirements
- si5351 Etherkit Library (found here: https://github.com/etherkit/Si5351Arduino)
- LiquidCrystal 74HC595 Library (found here: https://github.com/matmunk/LiquidCrystal_74HC595)
- IU8NQI Utils Library (included in the IU8NQI_Utils folder)

### Hardware Requirements
[See the schematic](https://github.com/StarNiell/FrequencyGenerator_Arduino_Si5351/blob/main/FG_Schematics.pdf)
- Arduino (UNO R3, Nano, etc..)
- Si5351 module
- LCD 20x2 compatible Hitachi HD44780
- 1 Rotary Encoder With Push Button
- 1 Transistor BC547
- 3 Polyester Capacitor 100 nf

### Notes
The output signal is locate on CLK0 output (pin 1) of si5351 module<br>
Long press button for swap between Frequency and other functions menu<br>
Short press button for navigate through the functions<br>
- in the Frequency function you can navigate between every digits 
- in the other functions you can navigate between: ON/OFF signal, Brightness and Power output



