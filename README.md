# CC3200-Board-To-Board-Messaging
Serial Interfacing between two CC3200 LaunchPads. Uses SPI to interface to a color OLED display and then implements two-way messaging via a UART serial connection.

Uses an Adrafruit OLED Breakout Board - 16-bit Color 1.5" (product id: 1431) and Texas Instruments CC3200 LaunchPad (CC3200-LAUNCHXL).

How To Setup Equipment/Wiring
------------------------------
Connect MOSI (pin 7 on cc3200) to SI on the OLED display.
Connect SCLK (pin 5 on cc3200) to CL on the OLED display.
Connect GPIO (pin 62 on cc3200) to DC on the OLED display.
Connect GPIO (pin 61 on cc3200) to R on the OLED display.
Connect CS (pin 8 on cc3200) to OC on the OLED display.
Connect 3.3V to Vin(+) on the OLED display.
Connect GND to GND(G) on the OLED display.

For part 2, Pin 1 is the TX and Pin 2 is the RX.


How To Use Program
-------------------
To use the demo routine for the OLED display uncomment
"DemoRoutine();" (line 349 in main.c) and comment out
"ComunicateRoutine();" (line 350 in main.c). Then just
run the program and it'll automatically cycle through the
various tests.

To use the board-to-board communication routine comment
out "DemoRoutine();" (line 349 in main.c) and uncomment
"ComunicateRoutine();" (line 350 in main.c). Then just 
run the program and follow the onscreen prompts in the 
terminal/on your OLED and when you begin to type your 
message will be displayed on the reciever's OLED display.
