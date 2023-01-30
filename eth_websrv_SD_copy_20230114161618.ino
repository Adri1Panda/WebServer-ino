/*--------------------------------------------------------------
  Program:      eth_websrv_SD

  Description:  Arduino web server that serves up a basic web
                site. The web site is stored on the SD card.
  
  Hardware:     Arduino Uno and Ethernet + SD shield (used Elecrow W5200 shield combining ETH + SD chips on SPI interface)
                2Gb micro SD card formatted FAT16 (tried with 8Go Class2 )
                
  Software:     Developed using Arduino 2.0.3 software
                SD card contains web site starting with page /index.htm
  
  References:   - 2013/01/10 : [W.A. Smith],
                  https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/SD-card-web-server/
                  modified by Tom Igoe
                - 2023/01/23 : [Adrien Levasseur],
                  Analyse TCP request and loads target html/jpg/png/ico files
  
--------------------------------------------------------------*/

#include <SPI.h>
#include <EthernetV2_0.h>   //#include <Ethernet.h> //For the Elecrow W5200 Ethernet shield : https://www.elecrow.com/w5200-ethernet-shield-p-367.html
#include <SD.h>
#include <string.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 1, 41, 11);
EthernetServer server(80);  //Creates a server at standard HTTP port 80

File webFile;
char clientHttpRequest[100] = "";  //The complete HTTP request from the web browser
char clientHttpGetFilePath[100];    //The complete filepath from the presumed GET instruction
char clientHttpGetFileExt[4];       //The file extension from the presumed GET instruction
unsigned int charIndex = 0;

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(115200);       // for debugging
    
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
}

void loop()
{
    EthernetClient client = server.available();  // try to get client
    
    if (client) {  // got client?
        boolean currentLineIsBlank = true;   //Detects if the line is blank so that it means the HTTP request is entirely received
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char clientHttpChar = client.read(); // read 1 byte (character) from client
                
                //Buffering the http client request
                if(charIndex < sizeof(clientHttpRequest)){
                  clientHttpRequest[charIndex] = clientHttpChar;
                }
                charIndex++;

//*DEBUG                Serial.print(clientHttpChar); //*DEBUG
                
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (clientHttpChar == '\n' && currentLineIsBlank) {
//*DEBUG                   Serial.println(); //*DEBUG
                    //Analysing the client request before generating a response
                    //Let's assume a GET instruction, parsing the filename
                    char* strchrSlash, *strchrDot, *strchrSpace;    //Get the pointer of each of important character to construct the HTTP answer
                    strchrSlash = strchr(clientHttpRequest, '/');
//*DEBUG                    Serial.print("strchrSlash = ");Serial.println(strchrSlash); //*DEBUG
                    strchrDot = strchr(strchrSlash, '.');
//*DEBUG                   Serial.print("strchrDot = ");Serial.println(strchrDot); //*DEBUG
                    strchrSpace = strchr(strchrDot, ' ');
//*DEBUG                    Serial.print("strchrSpace = ");Serial.println(strchrSpace); //*DEBUG

                    memcpy(clientHttpGetFilePath, strchrSlash, strchrSpace - strchrSlash);
//*DEBUG                    Serial.print("clientHttpGetFilePath = ");Serial.println(clientHttpGetFilePath); //*DEBUG

                    memcpy(clientHttpGetFileExt, strchrDot + 1, strchrSpace - strchrDot);
//*DEBUG                   Serial.print("clientHttpGetFileExt = ");Serial.println(clientHttpGetFileExt); //*DEBUG

                    //**Retrieving the complete Filename
                    if(clientHttpGetFilePath == "/"){
                      strcpy(clientHttpGetFilePath, "/index.htm");
                    }
//*DEBUG                    Serial.print("File path : ");Serial.println(clientHttpGetFilePath); //*DEBUG
                    
                    //Finally send a standard http response header
                    webFile = SD.open(clientHttpGetFilePath);        // open web page file

                    //**Generating the answer
                    if(webFile){
                      client.println("HTTP/1.1 200 OK");
                    }
                    else{
                      client.println("HTTP/1.1 404 Not Found");
                    }
                    
                    //**Generating the Content-type answer
                    if(clientHttpGetFileExt == "jpg"){
                      client.println("Content-Type: image/jpg");
                    }
                    else if(clientHttpGetFileExt == "ico"){
                      client.println("Content-Type: image/ico");
                    }
                    else if(clientHttpGetFileExt == "png"){
                      client.println("Content-Type: image/png");
                    }
                    else if(clientHttpGetFileExt == "css"){
                      client.println("Content-type: text/css, true");
                    }
                    else{
                      client.println("Content-Type: text/html; charset=utf-8");
                    }

                    //**Closing the connection
                    client.println("Connection: close");
                    client.println();
                    
                    //**Send web page
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    else{
                      Serial.print("Fail to open ");Serial.print(clientHttpGetFilePath);Serial.println(" file");
                    }

                    //Erase data from the Http Request and get ready to receive next one
                    charIndex = 0;
                    memset(clientHttpRequest, '\0', sizeof(clientHttpRequest));
                    memset(clientHttpGetFilePath, '\0', sizeof(clientHttpGetFilePath));
                    
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (clientHttpChar == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (clientHttpChar != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
