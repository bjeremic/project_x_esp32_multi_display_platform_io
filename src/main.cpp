#include <Arduino.h>
#include <stdio.h>


#define SCREEN_TOGGLE_MILLISEC  250   // milliseconds

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/OpenSans_VariableFont_wdth_wght20pt7b.h>
#include <Fonts/OpenSans_Regular6pt7b.h>
#include <Fonts/OpenSans_Regular10pt7b.h>
#include <Fonts/OpenSans_Regular4pt7b.h>
#include <Fonts/OpenSans_Regular5pt7b.h>
#include <Fonts/OpenSans_Regular8pt7b.h>

// https://rop.nl/truetype2gfx/      font ttf to free font format converter

//  https://github.com/KrisKasprzak/DualDisplay/blob/main/Dual.ino  reference to DUAL display

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// Declaration for SSD1306 display connected using software SPI (default case):
//#define OLED_MOSI       23    // OLED: SDA    WROOM: D23   SPI: VSPI_MOSI    I2C: SCL
//#define OLED_CLK        18    // OLED: SCL    WROOM: D18   SPI: VSPI_CLK     I2C: NONE
//#define OLED_DC         21    // OLED: DC     WROOM: D21   SPI: NONE         I2C: SDA 
//#define OLED_CS_ONE      5    // OLED: CS     WROOM: D5    SPI: VSPI_CS0     I2C: NONE
//#define OLED_CS_TWO      7    // OLED: CS     WROOM: D5    SPI: NONE         I2C: NONE
//#define OLED_RESET_ONE   4    // OLED: RES    WROOM: D4    SPI: NONE         I2C: NONE          
//#define OLED_RESET_TWO   6    // OLED: RES    WROOM: D6    SPI: NONE         I2C: NONE
//                                       WROOM: D15   SPI: HSPI_CS0     I2C: NONE
//                                       WROOM: D19   SPI: VSPI_MISO    I2C: NONE
//SPI INIT
//#define OLED_CS_ONE      5 
//Adafruit_SSD1306 screen_one(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET_ONE, OLED_CS_ONE);
//#define OLED_CS_TWO    6
//Adafruit_SSD1306 screen_two(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET_TWO, OLED_CS_TWO);

// I2C INIT
// PHYSICAL WIRE:
// OLED: SDA --- WROOM: D21   I2C: SDA     SPI: NONE
// OLED: SCL --- WROOM: D22   I2C: NONE    SPI: NONE
// OLED: RES --- WROOM: D4    I2C: NONE    SPI: NONE
// OLED: DC  --- NC
// OLED: CS  --- NC
#define OLED_RESET_ONE   2
#define OLED_RESET_TWO   4
#define SCREEN_ADDRESS_ONE 0x3C   // when DC pin pulled up to VCC then address is 0x3D
Adafruit_SSD1306 screen_one(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_ONE);
#define SCREEN_ADDRESS_TWO 0x3D   // when DC pin pulled up to VCC then address is 0x3D
Adafruit_SSD1306 screen_two(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_TWO);




/* Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
define OLED_RESET   4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
*/





// #define NUMFLAKES     10 // Number of snowflakes in the animation example

// #define LOGO_HEIGHT   16
// #define LOGO_WIDTH    16
// static const unsigned char PROGMEM logo_bmp[] =
// { 0b00000000, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000011, 0b11100000,
//   0b11110011, 0b11100000,
//   0b11111110, 0b11111000,
//   0b01111110, 0b11111111,
//   0b00110011, 0b10011111,
//   0b00011111, 0b11111100,
//   0b00001101, 0b01110000,
//   0b00011011, 0b10100000,
//   0b00111111, 0b11100000,
//   0b00111111, 0b11110000,
//   0b01111100, 0b11110000,
//   0b01110000, 0b01110000,
//   0b00000000, 0b00110000 };

// #################################
int OFFSET_THREE = 3; 
int OFFSET_FOUR = 4; 
int OFFSET_FIVE = 5; 
int LINE_HOR_TOP_THREE = (screen_two.height()/3) - OFFSET_FOUR;
int LINE_HOR_BOTT_THREE = (2*screen_two.height()/3) + OFFSET_FOUR;
int long_line_y = 14;
int line_bottom_y = 50;


struct splitData {
    uint8_t splitMinutes;
    uint8_t splitSeconds;
    uint8_t splitSecondTens;
};

struct dataErg {
    uint16_t meters;
    uint8_t metersHundreds;
    splitData split;
    splitData splitAv;
    uint8_t spm;
    uint8_t spmTens;
};


dataErg myDataErg;

enum displayer {ONEONE, ONETWO, PM5ONE};

struct layoutSelection {
  displayer screenOne;
  displayer screenTwo;
  u_int8_t multiple;
};

struct layoutSelectionStack {
  layoutSelection one;
  layoutSelection two;
  layoutSelection three;
  layoutSelection four;
  layoutSelection five;
};

layoutSelectionStack  myLSS;
// #####################
// #################################

void drawTimeLayoutOne(Adafruit_SSD1306 &screen, String time) {
  screen.setFont(&OpenSans_Regular6pt7b);
  screen.setCursor(85,10);
  screen.setTextSize(1);
  screen.println(time);
}

void drawOneOneMarginSplitAverage(Adafruit_SSD1306 &screen, String prefix, splitData mySplitData) {
  screen.setCursor(5,10);
  screen.setFont(&OpenSans_Regular5pt7b);
  screen.println(prefix);
  screen.setFont(&OpenSans_Regular6pt7b);
  screen.setCursor(17,10);
  screen.print(String(mySplitData.splitMinutes));
  screen.setCursor(24,10);
  screen.print(F(":"));
  screen.setCursor(28,10);
  screen.print(String(mySplitData.splitSeconds));
}

void drawMetersLayoutOne(Adafruit_SSD1306 &screen) {
  String meters=String(myDataErg.meters);
  uint8_t prefixPosition = 0;
  if (meters.length() > 4) {
     prefixPosition += 6; 
  };
  screen.setFont(&OpenSans_Regular6pt7b);
  screen.setCursor(4,62);
  screen.print("m"); 
  screen.setCursor(17,62);
  screen.print(meters);
  screen.setCursor(47+prefixPosition,62);
  screen.print("."); 
  screen.setCursor(49+prefixPosition,62);
  screen.print(String(myDataErg.metersHundreds));
}

void drawSpmLayoutOne(Adafruit_SSD1306 &screen) { 
  screen.setFont(&OpenSans_Regular6pt7b);
  screen.setCursor(77,62);
  screen.print(F("spm"));
  screen.setCursor(102,62);
  screen.print(String(myDataErg.spm));
  screen.setCursor(116,62);
  screen.print(F("."));
  screen.setCursor(119,62);
  screen.print(String(myDataErg.spmTens));
}

// void drawOneOneMainSplit(String prefix, splitData mySplitData) {
//   display.setTextColor(SSD1306_WHITE);        // Draw white text
//   display.setFont(&OpenSans_Regular6pt7b);
//   display.setCursor(2,25);
//   display.print(prefix);
//   display.setFont(&OpenSans_Regular4pt7b);
//   display.setCursor(2,45);
//   display.print(F("/500"));
//   display.setFont(&OpenSans_VariableFont_wdth_wght20pt7b);
//   display.setCursor(23,45);
//   display.print(mySplitData.splitMinutes);
//   display.setCursor(43,45);
//   display.print(F(":"));
//   display.setCursor(52,45);
//   display.print(mySplitData.splitSeconds);
//   display.setFont(&OpenSans_Regular10pt7b);
//   display.setCursor(98,45);
//   display.print(F("."));
//   display.setCursor(103,45);
//   display.print(mySplitData.splitSeconds);
// }

void drawSplitMainLayoutOneOne(Adafruit_SSD1306 &screen, String prefix, splitData mySplitData, int8_t y_offset) {
  screen.setTextColor(SSD1306_WHITE);        // Draw white text
  screen.setFont(&OpenSans_Regular6pt7b);
  screen.setCursor(2,y_offset);
  screen.print(prefix);
  screen.setFont(&OpenSans_Regular4pt7b);
  screen.setCursor(2,20+y_offset);
  screen.print(F("/500"));
  screen.setFont(&OpenSans_VariableFont_wdth_wght20pt7b);
  screen.setCursor(23,20+y_offset);
  screen.print(mySplitData.splitMinutes);
  screen.setCursor(43,20+y_offset);
  screen.print(F(":"));
  screen.setCursor(52,20+y_offset);
  screen.print(mySplitData.splitSeconds);
  screen.setFont(&OpenSans_Regular10pt7b);
  screen.setCursor(98,20+y_offset);
  screen.print(F("."));
  screen.setCursor(103,20+y_offset);
  screen.print(mySplitData.splitSeconds);
}


void drawLinesLayoutOne(Adafruit_SSD1306 &screen) {
  screen.drawLine(0, long_line_y, screen.width()-1, long_line_y, SSD1306_WHITE);
  screen.drawLine(0, line_bottom_y, screen.width()-1, line_bottom_y, SSD1306_WHITE);  
}


void drawLayoutOneOne(Adafruit_SSD1306 &screen) {
  //screen.clearDisplay();
  drawLinesLayoutOne(screen);
  drawSplitMainLayoutOneOne(screen, F("av"), myDataErg.splitAv, 25);
  drawOneOneMarginSplitAverage(screen, F("cr"), myDataErg.split);
  drawTimeLayoutOne(screen, F("1:05.04"));
  drawMetersLayoutOne(screen);
  drawSpmLayoutOne(screen);
  screen.display();
  screen.clearDisplay();
}

void drawLayoutOneTwo(Adafruit_SSD1306 &screen) {
  drawLinesLayoutOne(screen);
  drawSplitMainLayoutOneOne(screen, F("cr"), myDataErg.split, 25);
  drawOneOneMarginSplitAverage(screen, F("av"), myDataErg.splitAv);
  drawTimeLayoutOne(screen, F("1:05.04"));
  drawMetersLayoutOne(screen);
  drawSpmLayoutOne(screen);
  screen.display();
  screen.clearDisplay();  // this clears the mem buffer, but it does not clear the screen
}

void drawLinesLayoutPM5One(Adafruit_SSD1306 &screen) {
  screen.drawLine(1, LINE_HOR_TOP_THREE, screen.width()-1, LINE_HOR_TOP_THREE, SSD1306_WHITE);
  screen.drawLine(1, LINE_HOR_BOTT_THREE, screen.width()-1, LINE_HOR_BOTT_THREE, SSD1306_WHITE);  
  screen.drawLine(80, 1, 80, LINE_HOR_TOP_THREE, SSD1306_WHITE);
  screen.drawLine(80, LINE_HOR_BOTT_THREE, 80, screen.height()-1, SSD1306_WHITE);
}


void drawLayoutPM5One(Adafruit_SSD1306 &screen) {
  drawLinesLayoutPM5One(screen);
  //drawOneOneMeters();
  //drawOneOneSpm();
  screen.display();
  screen.clearDisplay();
}


void initData() {
  myDataErg.split.splitMinutes = 1;
  myDataErg.split.splitSeconds = 43;
  myDataErg.split.splitSecondTens = 10;
  myDataErg.splitAv.splitMinutes = 1;
  myDataErg.splitAv.splitSeconds = 46;
  myDataErg.splitAv.splitSecondTens = 20;
  myDataErg.meters = 2409;
  myDataErg.metersHundreds = 10;
  myDataErg.spm = 26;
  myDataErg.spmTens = 4;
}

// void setUpUdp() {
//   // Connect to Wifi network.
//   WiFi.begin(ssid, ssidPassword);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500); 
//     Serial.print(F("."));
//   };
//   udp.begin(localUdpPort);
//   Serial.printf("UDP server : %s:%i \n", WiFi.localIP().toString().c_str(), localUdpPort);
// }

void initializeDisplayScreenInfo() {
  myLSS.one.screenOne = ONEONE;
  myLSS.one.screenTwo = ONETWO;
  myLSS.one.multiple = 1;
  myLSS.two.screenOne = PM5ONE;
  myLSS.two.screenTwo = PM5ONE;
  myLSS.two.multiple = 1;
  myLSS.three.screenOne = ONEONE;
  myLSS.three.screenTwo = ONETWO;
  myLSS.three.multiple = 1;
  myLSS.four.screenOne = ONETWO;
  myLSS.four.screenTwo = ONEONE;
  myLSS.four.multiple = 1;
  myLSS.five.screenOne = ONEONE;
  myLSS.five.screenTwo = PM5ONE;
  myLSS.five.multiple = 1;
}


void setup() {
  Serial.begin(115200);
  //################
  // wifi
  //etUpUdp();  
  //###############
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // ######################################################
  // if(!display.begin(SSD1306_SWITCHCAPVCC)) {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for(;;); // Don't proceed, loop forever
  // }
  screen_one.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS_ONE);
  screen_two.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS_TWO);  // THERE IS A PROBLEM TURNING ON THE SECOND DISPLAY
  // 
  initData();
  initializeDisplayScreenInfo();
}


void crank(uint8_t addition) {
  myDataErg.meters += addition;
}

void delayScreenToggleMultiplier(uint8_t multiple) {
  delay(SCREEN_TOGGLE_MILLISEC * multiple);
}

void displayLayoutOnSingleScreen(displayer layoutChoice, Adafruit_SSD1306 &screen) {
  if(layoutChoice == ONEONE)       {drawLayoutOneOne(screen);}
  else if(layoutChoice == ONETWO)  {drawLayoutOneTwo(screen);}
  else if(layoutChoice == PM5ONE)  {drawLayoutPM5One(screen);}
}

void displayLayoutsOnScreens(layoutSelection &ls) {
  displayLayoutOnSingleScreen(ls.screenOne, screen_one);  
  displayLayoutOnSingleScreen(ls.screenTwo, screen_two);
  delayScreenToggleMultiplier(ls.multiple);
}

void displayLayoutSequence() {
  displayLayoutsOnScreens(myLSS.one);
  displayLayoutsOnScreens(myLSS.two);
  displayLayoutsOnScreens(myLSS.three);
  displayLayoutsOnScreens(myLSS.four);
  displayLayoutsOnScreens(myLSS.five);
}


void loop() {
  displayLayoutSequence();
}

