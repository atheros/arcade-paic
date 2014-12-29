Programmable Arcade Input Controller
====================================


About
-----

This is code for Arduino Pro Micro based boards with ATmega32u4 microcontroller.

It emulates an USB keyboard and converts pin changes to key strokes.

It also supports reprogramming keys it emulates.


Status
------

This code should be treated as **alpha** quality. My arcade machine is using 3 of those and works, however there are still few issues:

* I was only able to program it through the serial monitor of ardunio IDE.
* only characters you can enter in the serial terminal can be used
* because Arduino Keyboard class lacks direct control of the HID report, you cannot set button priorities (yet)

