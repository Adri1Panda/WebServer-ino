# WebServer-ino
## General Description :
Improving Arduino Ethernet WebServer example to bring navigation and dynamisation of web pages
Based and improved from https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial

## Change status :
2023/01/30 : Initial state
  Starting from this case study : https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/SD-card-web-server/ (all parts are interesting to read). Improving by allowing navigating through several pages, loading images/icon... by parsing HTTP requests

## Needed Hardware :
- Arduino (Uno is fine, made with Uno R2)
- Ethernet + SD shield (made with elecrow W5200 shield that embed both Ethernet and SD card chips)
- PC with wired Ethernet card + Straight Ethernet RJ45 cord
- µSD card (tried with 8Go Class2)

## TO BE IMPROVED :
  - Sending CSS files
  - Using AJAX to provide dynamic datas from/to the arduino to/from the webpages (that is discussed later in the https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial)
