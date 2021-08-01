## SimpleWiFi

This is an Arduino library for interfacing with WiFi modules that using AT command interface, such as ESP8266/ESP32. This library is based on the works of TinyGSM which try to be all-things-AT-commands. This library stripped out all the unrelated non-WiFi modules and focus only on WiFi modules that using AT command interface, to make the library easier to setup and focus on the development of new features and functionalities related to WiFi modules.

> **This library has not reach to a mature stage yet, therefore do not download as it may not work at this stage. Check back in the next a couple of weeks, we will remove this sentence when it is ready for use.**

If you like **SimpleWiFi** - give it a star, or fork it and contribute!

- [Supported WiFi modules](#supported-wifi-modules)
- [Features](#features)
- [Getting Started](#getting-started)
    - [First Steps](#first-steps)
    - [Writing your own code](#writing-your-own-code)
    - [If you have any issues](#if-you-have-any-issues)
- [How does it work?](#how-does-it-work)
- [API Reference](#api-reference)
- [Troubleshooting](#troubleshooting)
  - [Ensure stable data & power connection](#ensure-stable-data--power-connection)
  - [Baud rates](#baud-rates)
  - [Broken initial configuration](#broken-initial-configuration)
  - [Failed connection or no data received](#failed-connection-or-no-data-received)
  - [Diagnostics sketch](#diagnostics-sketch)
  - [Web request formatting problems - "but it works with PostMan"](#web-request-formatting-problems---but-it-works-with-postman)
  - [SoftwareSerial problems](#softwareserial-problems)
  - [ESP32 Notes](#esp32-notes)
    - [HardwareSerial](#hardwareserial)
    - [HttpClient](#httpclient)
- [License](#license)


## Supported WiFi modules

- ESP8266/ESP32 (with AT commands interface)
- Digi XBee WiFi (using XBee command mode)


## Features

**Data connections**
- TCP (HTTP, MQTT, Blynk, ...)
    - ALL modules support TCP connections
    - Multiple simultaneous connections support:
        - ESP8266 - 5
        - Digi XBee - _only 1 connection supported!_
- UDP
    - (To be supported in near future)
- SSL/TLS (HTTPS)
    - Supported on:
        - ESP8266
    - Not possible on:
        - XBee _WiFi_


## Getting Started


#### First Steps

  1. Ensure that you have a stable power supply capable to supply up to **600mA**.
  2. Check if serial connection is connected correctly with cross connection (i.e. Tx -> Rx, Rx -> Tx) (Hardware Serial is recommended).
     Send an `AT` command using [this sketch](tools/AT_Debug/AT_Debug.ino) or via Serial Monitor/TeraTerm.
  6. Try out the [http_get](https://github.com/e-tinkers/SimpleWiFi/blob/master/examples/http_get/http_get.ino) example


#### Writing your own code

The general flow of your code should be:
- Define the module that you are using (choose one and only one)
    - ie, `#define SIMPLE_WIFI_MODEM_SIM800`
- Included SimpleWiFi
    - `#include <SimpleWiFiClient.h>`
- Create a SimpleWiFi modem instance
    - `SimpleWiFi modem(SerialAT);`
- Create one or more SimpleWiFi client instances
    - For a single connection, use
        - `SimpleWiFiClient client(modem);`
        or
        `SimpleWiFiClientSecure client(modem);` (on supported modules)
    - For multiple connections (on supported modules) use:
        - `SimpleWiFiClient client0(modem, 0);`, `SimpleWiFiClient client1(modem, 1);`, etc
          or
        - `SimpleWiFiClientSecure client0(modem, 0);`, `SimpleWiFiClientSecure client1(modem, 1);`, etc
    - Secure and insecure clients can usually be mixed when using multiple connections.
    - The total number of connections possible varies by module
- Begin your serial communication and set all your pins as required to power your module and bring it to full functionality.
    - The examples attempt to guess the module's baud rate.  In working code, you should use a set baud.
- Wait for the module to be ready (could be as much as 6s, depending on the module)
- Initialize the modem
    - `modem.init()` or `modem.restart()`
    - restart generally takes longer than init but ensures the module doesn't have lingering connections
- Specify your SSID information:
    - `modem.networkConnect(wifiSSID, wifiPass)`
- Wait for network registration to be successful
    - `modem.waitForNetwork(600000L)`
- Connect the TCP or SSL client
    `client.connect(server, port)`
- Send out your data.


#### If you have any issues

  1. Read the whole README (you're looking at it!), particularly the troubleshooting section below.
  2. Try running the Diagnostics sketch


## How does it work?

Many WiFi modules can be controlled by sending AT commands over Serial.
SimpleWiFi wrapped those AT commands with an easy-to-use Arduino client API to to send and to handle AT responses.

This library is "blocking" in all of its communication.
Depending on the function, your code may be blocked for a long time waiting for the module responses.
Apart from the obvious (ie, `waitForNetwork()`) several other functions may block your code for up to several *minutes*.
The `client.connect()` functions commonly block the longest, especially in poorer service regions.
The module shutdown and restart may also be quite slow.

This libary *does not* support any sort of "hardware" or pin level controls for the modules.
If you need to turn your module on or reset it using some sort of High/Low/High pin sequence, you must write those functions yourself.

## API Reference

Please refer to [this example sketch](examples/AllFunctions/AllFunctions.ino) where it demonstrates the usage of most of the API.

## Troubleshooting

### Ensure stable data & power connection

Most modules require _**at least 500mA**_ to properly connect to the network, many of the Arduino boards may not able to power those WiFi modules.
Improving the power supply actually solves stability problems in **many** cases!
- Keep your wires as short as possible
- Do not put your wires next to noisy signal sources (buck converters, antennas, oscillators etc.)
- If everything else seems to be working but you are unable to connect to the network, check your power supply!
- Make sure you connect the Serial connection correctly between the host MCU and the WiFi module (i.e. MCU Tx -> WiFi Rx, MCU Rx -> WiFi Tx).

### Baud rates

Most modules capable of communicating at high speed up to MHz, therefore Hardware Serial is recommended on your MCU in order to achieve high performance. If the MCU lack of Hardware Serial port, then Software Serial can be used. See [SoftwareSerial problems](#softwareserial-problems) for further information on using Software Serial.

### Broken initial configuration

Sometimes (especially if you played with AT commands), your module configuration may become invalid.
This may result in problems such as:

 * Can't connect to the network
 * Can't connect to the server
 * Sent/received data contains invalid bytes
 * etc.

To return module to **Factory Defaults**, use this sketch:
  File -> Examples -> SimpleWiFi -> tools -> [FactoryReset](https://github.com/e-tinkers/SimpleWiFi/blob/master/tools/FactoryReset/FactoryReset.ino)


### Diagnostics sketch

Use this sketch to help diagnose connection issues:
  File -> Examples -> SimpleWiFi -> tools -> [Diagnostics](https://github.com/e-tinkers/SimpleWiFi/blob/master/tools/Diagnostics/Diagnostics.ino)

If the diagnostics fail, uncomment this line to output some debugging comments from the library:
```cpp
#define SIMPLE_WIFI_DEBUG SerialMon
```
In any custom code, `SIMPLE_WIFI_DEBUG` must be defined before including the SimpleWiFi library.

If you are unable to see any obvious errors in the library debugging, use [StreamDebugger](https://github.com/vshymanskyy/StreamDebugger) to copy the entire AT command sequence to the main serial port.
In the diagnostics example, simply uncomment the line:
```cpp
#define DUMP_AT_COMMANDS
```
In custom code, you can add this snippit:
```cpp
#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  SimpleWiFi modem(debugger);
#else
  SimpleWiFi modem(SerialAT);
#endif
```

### Web request formatting problems - "but it works with PostMan"

This library opens a TCP (or SSL) connection to a server.
In the [OSI model](https://en.wikipedia.org/wiki/OSI_model), that's [layer 4](http://www.tcpipguide.com/free/t_TransportLayerLayer4.htm) (or 5 for SSL).
HTTP (GET/POST), MQTT, and most of the other functions you probably want to use live up at [layer 7](http://www.tcpipguide.com/free/t_ApplicationLayerLayer7.htm).
This means that you need to either manually code the top layer or use another library (like [HTTPClient](https://github.com/arduino-libraries/ArduinoHttpClient) or [PubSubClient](https://pubsubclient.knolleary.net/)) to do it for you.
Tools like [PostMan](https://www.postman.com/) also show layer 7, not layer 4/5 like SimpleWiFi.
If you are successfully connecting to a server, but getting responses of "bad request" (or no response), the issue is probably your formatting.
Here are some tips for writing layer 7 (particularly HTTP request) manually:
- Look at the "http_get" example
- Make sure you are including all required headers.
    - If you are testing with PostMan, make sure you un-hide and look at the "auto-generated" headers; you'll probably be surprised by how many of them there are.
- Use `client.print("...")`, or `client.write(buf, #)`, or even `client.write(String("..."))`, not `client.write("...")` to help prevent text being sent out one character at a time (typewriter style)
- Enclose the entirety of each header or line within a single string or print statement
    - use
    ```cpp
    client.print(String("GET ") + resource + " HTTP/1.1\r\n");
    ```
    instead of
    ```cpp
    client.print("GET ");
    client.print(resource);
    client.println(" HTTP/1.1")
    ```
- Make sure there is one entirely blank line between the last header and the content of any POST request.
    - Add two lines to the last header `client.print("....\r\n\r\n")` or put in an extra `client.println()`
    - This is an HTTP requirement and is really easy to miss.

### SoftwareSerial problems

When using `SoftwareSerial` (on Uno, Nano, etc), the speed **115200** may not work.
Try selecting **57600**, **38400**, or even lower - the one that works best for you.
In some cases **9600** is unstable, but using **38400** helps, etc.
Be sure to set correct TX/RX pins in the sketch. Please note that not every Arduino pin can serve as TX or RX pin.
**Read more about SoftwareSerial options and configuration [here](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) and [here](https://www.arduino.cc/en/Reference/SoftwareSerial).**

### ESP32 Notes

#### HardwareSerial

When using ESP32 `HardwareSerial`, you may need to specify additional parameters to the `.begin()` call.
```
SerialAT.begin(115200,SERIAL_8N1,16,17,false);  // For Serial1
```

#### HttpClient
You will not be able to compile the HttpClient or HttpsClient examples with ESP32 core 1.0.2.  Upgrade to 1.0.3, downgrade to version 1.0.1 or use the WebClient example.

### SAMD21

When using SAMD21-based boards, you may need to use a sercom uart port instead of `Serial1`.
```
#define PIN_SERIAL2_RX       (34ul)               // Pin description number for PIO_SERCOM on D12
#define PIN_SERIAL2_TX       (36ul)               // Pin description number for PIO_SERCOM on D10
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3

Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
```

__________

## License
This project is released under
The GNU Lesser General Public License (LGPL-3.0)
