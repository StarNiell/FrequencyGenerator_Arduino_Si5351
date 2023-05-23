# FrequencyGenerator_Arduino_Si5351

This project help you to build a Frequency Generator using a Si5351 Module on Arduino (UNO R3, Nano, etc...)
The schematic attacched use a LCD 20x2 compatible Hitachi HD44780 with 74HC595 shift register integrated circuit (see ["how drive a HD44780 LCD using a 74HC595 shift register"](https://www.google.com/search?q=how+drive+a+HD44780+LCD+using+a+74HC595+shift+register))

### Software Requirements
si5351 Etherkit Library (found here: https://github.com/etherkit/Si5351Arduino)<br>
LiquidCrystal 74HC595 Library (found here: https://github.com/matmunk/LiquidCrystal_74HC595)<br>
IU8NQI Utils Library (included in the IU8NQI_Utils folder)<br>

### Hardware Requirements
Arduino (UNO R3, Nano, etc..)<br>
Si5351 module<br>
LCD 20x2 compatible Hitachi HD44780<br>
1 Rotary Encoder With Push Button<br>
1 Transistor BC547<br>
3 Polyester Capacitor 100 nf<br>

### Notes
The ouput signal is locate on CLK0 output pin of si5351 module<br>
Long press button for swap between Frequency and other functions menu<br>
Short press buttom for navigate through the functions<br>
- in the Frquency function you can navigate between every digits 
- in the other functions you can navigate between: ON/OFF signal, Brightness and Power output



