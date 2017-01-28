# Snach Arduino
DIY Smartwatch built with Arduino.

This is the Arduino code for the project demonstrated at https://www.youtube.com/watch?v=0DXkcHel6NA <br>
I am not working actively on this project anymore, but I wanted to share the code with you. I wrote all of this right before I started to study computer sciences, so I was gaining experience and developping my coding skills through the project.

# The Concept
Most of the concept is shown in the video. The watch does hardly any computiation on the arduino but streams all data (button events, sensor data, ...) to the connected Android smartphone. It communicates with the Android Companion App shown in the Snach-Android repository. The Snach should provide the hardware to enable gesture control of your phone and extension of your phone screen. The App should provide software support to reach these goals.  <br>
The vision for this project is that smart watches should not try to resemble but try to extend a smartphone. As such they show dedicated "to the point" information, provide access to a small set of (important) applications right from your wrist and add sensors to your phone.

# The Software
Imagine you have a widget for the Apps you use most. Now imagine these widgets are not on your home screen, but on your wrist watch. Each widget is formatted following a template which is provided by the Snach/Android App. You can select the widgets you want to have on your Snach with the Android App. The App then takes care of showing them on the smartwatch screen when you navigate to the widget. <br>
Navigation is realized like this: Imagine a double linked list with the watch face at the head followed by the widgets. When you tap the button to reach the next app, you will see the next widget in line, when you tap the button to reach the previous App you see the prevous widget in line. Tapping the "home" button brings you to the head of the list (the watch face).

3rd party Apps can use the Android API to configure and offer widgets. Navigating to an App triggers the App to start requesing the layout configuration and contents of the widget from the 3rd party App. If you press a button while the App is shown these button events are also transmitted to the 3rd party Android App. This way it can respond to button clicks and update the screen content as needed.

You need to include the following libraries to your Arduino IDE which are used by the Snach:
https://github.com/adafruit/Adafruit_SHARP_Memory_Display <br>
https://github.com/adafruit/Adafruit-GFX-Library <br>
https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050 <br>
https://www.arduino.cc/en/Reference/Wire <br>
https://github.com/adafruit/RTClib <br>
https://github.com/Cheong2K/ble-sdk-arduino <br>

# The Hardware
To get the Snach running you will need to hook up all DIY boards as shown in the schematics to an Arduino Mega and upload the code to the Arduino. I'm not much of an electronical engineer, so I took the schematics from the open source schematics and put them into one file. I annotated the name of the parts, I think this way it is easier to see how should connected the components.

Used Components:
<ul>
<li>Adruino Mega 2560
<li>BLEShield by RedBearLabs; To create a new BLE Profile the nRF Studio software provided by nRF Semiconductors was pretty helpful
<li>MPU6050 Acceleration Sensor for gesture recognition (experimental; I wanted to control my phone with gestures and the feature works partially but has some issues during gesture recording and medium recognition success rate)
<li>DS1307 I2C Real Time Clock
<li>BSS138 Logic Level Converter by Adafruit
<li>Sharp Memory Display 1.3" Silver Monochrome by Adafruit 
<li>7 Buttons, I used 4 and 3 self made pseudo touch buttons (just 3 wires on pull up resistors, "button presses" were triggered by connecting GND temporarily to one of the wires; I had run out of buttons...). 4 Buttons are designed to be placed right below the display so that you have to press the diplay to click up, left, down, right. These buttons are used for interaction with Apps. The other 3 buttons are used for navigation though the smartwatch system. So one for reaching to the next App, one for reaching to the last App and one to return to the watch face.
<li>Vibration Motor for notification
</ul>

# Licenses
See the licenses.txt for licenses of the schematics used in this project. Code and creative contents created by me are signed as such and licensed under the Apache-2.0. See LICENSE.txt for details. Code not created by me is signed as such and includes a copyright notice on the top.
