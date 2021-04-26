#include "centroidtracker.h" // has to be above <M5StickC.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

#include <M5StickC.h>

#include "Queue.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

int WIFI_ENABLE = 1;      // set this variable to 1 to enable WiFi server
int TEMP_THRESHOLD = 25;  // temperature threshold value (NOTE: you may need to adjust this)

// Replace with your network credentials
const char* ssid = "MySpectrumWiFid8-2G";
const char* password = "brightsnail985";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <h1>Thermal Cam</h1>
  <p>
  <img src="thermal" id="thermalimage" style="width: 100%">
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    //if (this.readyState == 4 && this.status == 200) {
      var image = document.getElementById("thermalimage");
      //image.src = this.response;
      image.src = "thermal";
      //document.getElementById("thermal") = this.response;
    //}
  };
  xhttp.open("GET", "/thermal", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>
)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
html {
  font-family: Helvetica;
  display: inline-block;
  margin: 0px auto;
  text-align: center;
}
h1{
  color: #0F3376;
  padding: 2vh;
}
p{
  font-size: 1.5rem;
}
)rawliteral";


CentroidTracker *centroidTracker; // pointer to centroidTracker object

//low range of the sensor (this will be blue on the screen)
#define MINTEMP 22

//high range of the sensor (this will be red on the screen)
#define MAXTEMP 34

//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};

Adafruit_AMG88xx amg;
unsigned long delayTime;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
uint16_t displayPixelWidth, displayPixelHeight;

float avgVal;  // average of all pixels

// blob counting code
// http://tech-queries.blogspot.com/2011/07/count-number-of-blobs.html

#define MAX 8
#define USED 0x80

struct point
{
    int x, y;
};

struct blob
{
  int xmin, xmax;
  int ymin, ymax;
};

DataQueue<struct point> *q;
char m[8][8];
int numBlobsTotal = 0;

void push_in_queue(int x, int y, DataQueue<struct point> *q, char mat[MAX][MAX], int nrow, int ncol) 
{
    if(x < 0 || x >= nrow || y < 0 || y >= ncol)
      return;
    struct point pnt;
    if (mat[x][y] == 'X' && !(USED & mat[x][y]))
    {        
        mat[x][y] = USED | mat[x][y];
        pnt.x = x;
        pnt.y = y;
        q->enqueue(pnt);
    }
}

int count_blob(char mat[MAX][MAX], int nrow, int ncol)
{   
    DataQueue<struct point> que;
    DataQueue<struct blob> blobQue;
    std::vector<std::vector<int>> boxes;
    
    int cnt = 0;
    int x, y;
    point pnt, tmp;
    if (nrow == 0 && ncol == 0)
        return 0;
        
    for(x = 0; x < nrow; x++)
    {
        for(y = 0; y < ncol; y++)            
        {
            if (USED & mat[x][y])
                continue;
                
            if (mat[x][y] == 'X')
            {
                mat[x][y] = USED | mat[x][y];
                pnt.x = x;
                pnt.y = y;
                que.enqueue(pnt);

                blob b;
                b.xmin = b.xmax = x;
                b.ymin = b.ymax = y;
                blobQue.enqueue(b); // add new blob to blobQue

                while( !que.isEmpty() )
                {
                    tmp = que.front();  // get the first point from the que

                    if(tmp.x < b.xmin) {
                      b.xmin = tmp.x;
                    }
                    else if(tmp.x > b.xmax) {
                      b.xmax = tmp.x;
                    }
                    if(tmp.y < b.ymin) {
                      b.ymin = tmp.y;
                    }
                    else if(tmp.y > b.ymax) {
                      b.ymax = tmp.y;
                    }
                      
                    que.dequeue();                    
                    push_in_queue(tmp.x - 1, tmp.y,     &que, mat, nrow, ncol);
                    push_in_queue(tmp.x - 1, tmp.y - 1, &que, mat, nrow, ncol);
                    push_in_queue(tmp.x - 1, tmp.y + 1, &que, mat, nrow, ncol);
                    push_in_queue(tmp.x + 1, tmp.y,     &que, mat, nrow, ncol);
                    push_in_queue(tmp.x + 1, tmp.y - 1, &que, mat, nrow, ncol);
                    push_in_queue(tmp.x + 1, tmp.y + 1, &que, mat, nrow, ncol);
                    push_in_queue(tmp.x,     tmp.y - 1, &que, mat, nrow, ncol);
                    push_in_queue(tmp.x,     tmp.y + 1, &que, mat, nrow, ncol);                   
                }

                // screen coordinates of the blob bounding box:
                int bx = b.xmin * 10;
                int by = b.ymin * 10;
                int bw = (1 + b.xmax - b.xmin) * 10;
                int bh = (1 + b.ymax - b.ymin) * 10;
                int cx = bx + bw/2;
                int cy = by + bh/2;

                // increment blob count and insert blob coordinates:
                if(bw > 10 && bh > 10) {
                  
                  cnt += 1; // increment blob count
                  boxes.insert(boxes.end(), {by, bx, by+bh, bx+bw});
                
                  M5.Lcd.drawRect(by, bx, bh, bw, WHITE);
                  // draw cross-hair in the middle of the bounding box:
                  M5.Lcd.drawLine(cy-3, cx, cy+3, cx, WHITE);
                  M5.Lcd.drawLine(cy, cx-3, cy, cx+3, WHITE);
                }
                else {
                  M5.Lcd.drawRect(by, bx, bh, bw, BLACK);
                }
            }                    
        }
    }
    auto objects = centroidTracker->update(boxes);
    if(!objects.empty()) {
      //Serial.print("objects.size() = ");
      //Serial.println(objects.size());
      for (auto obj: objects) { 
        int objID = obj.first;
        if(objID >= numBlobsTotal) {
          numBlobsTotal = objID + 1;
        }
        int objx = obj.second.first;
        int objy = obj.second.second;
        M5.Lcd.setCursor(objx-2, objy-3);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextColor(BLACK, WHITE);  // black on white background
        M5.Lcd.print(objID);
      }
    }
    return cnt;
}


void PixelsToBmp()
{
  uint16_t color = 0;
  unsigned int headers[13];
  int extrabytes;
  int paddedsize;
  int x = 0; 
  int y = 0; 
  int n = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  
  int WIDTH = 8;  //32;
  int HEIGHT = 8; //24;

  extrabytes = 4 - ((WIDTH * 3) % 4);               // How many bytes of padding to add to each
                                                    // horizontal line - the size of which must
                                                    // be a multiple of 4 bytes.
  if (extrabytes == 4)
    extrabytes = 0;

  paddedsize = ((WIDTH * 3) + extrabytes) * HEIGHT;

// Note that the "BM" identifier in bytes 0 and 1 is NOT included in "headers".

  headers[0]  = paddedsize + 54;      // bfSize (whole file size)
  headers[1]  = 0;                    // bfReserved (both)
  headers[2]  = 54;                   // bfOffbits
  headers[3]  = 40;                   // biSize
  headers[4]  = WIDTH;                // biWidth
  headers[5]  = HEIGHT;               // biHeight

// Would have biPlanes and biBitCount in position 6, but they're shorts.
// It's easier to write them out separately (see below) than pretend
// they're a single int, especially with endian issues...

  headers[7]  = 0;                    // biCompression
  headers[8]  = paddedsize;           // biSizeImage
  headers[9]  = 0;                    // biXPelsPerMeter
  headers[10] = 0;                    // biYPelsPerMeter
  headers[11] = 0;                    // biClrUsed
  headers[12] = 0;                    // biClrImportant

  File file = SPIFFS.open("/thermal.bmp", "wb");
  if (!file) {
    Serial.println("There was an error opening the file for writing");
  }
  else {

  // Headers:
  file.print("BM");
  for (n = 0; n <= 5; n++)
  { 
    file.printf("%c", headers[n] & 0x000000FF);
    file.printf("%c", (headers[n] & 0x0000FF00) >> 8);
    file.printf("%c", (headers[n] & 0x00FF0000) >> 16);
    file.printf("%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
  }

  // biPlanes and biBitCount fields:
  file.printf("%c", 1);
  file.printf("%c", 0);
  file.printf("%c", 24);
  file.printf("%c", 0);

  for (n = 7; n <= 12; n++)
  {
    file.printf("%c", headers[n] & 0x000000FF);
    file.printf("%c", (headers[n] & 0x0000FF00) >> 8);
    file.printf("%c", (headers[n] & 0x00FF0000) >> 16);
    file.printf("%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
  }

  // Headers done, now write the data:
  for (y = HEIGHT - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
  {
    for (x = 0; x <= WIDTH - 1; x++)
    {
      // read ColorIndex corresponding to pixel temperature:
      int colorIndex = map(pixels[x+(8*y)], MINTEMP, MAXTEMP, 0, 255);
      colorIndex = constrain(colorIndex, 0, 255);
      color = camColors[colorIndex];
      
      // Convert 4 Digits HEX to RGB565 -> RGB888:
      red = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
      green = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
      blue = (((color & 0x1F) * 527) + 23) >> 6;

      // Constrain RGB range from 0 to 255:
      if (red > 255) red = 255; if (red < 0) red = 0;
      if (green > 255) green = 255; if (green < 0) green = 0;
      if (blue > 255) blue = 255; if (blue < 0) blue = 0;

      // Write data in (b,g,r) format:
      file.printf("%c", blue);
      file.printf("%c", green);
      file.printf("%c", red);
    }
    if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
    {
      for (n = 1; n <= extrabytes; n++)
      {
         file.printf("%c", 0);
      }
    }
  }

  file.close();
  Serial.println("Saved BMP to SPIFFS");
  }         
}

void setup() {
  Serial.begin(115200);
  Serial.println("AMG88xx thermal camera!");

  M5.begin();

/////////////////////////////////////////////////////////////////////////////////////////////////
  
//  tft.createSprite(80, 160); 
//  
//  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
//  TJpgDec.setJpgScale(1);
//
//  // The byte order can be swapped
//  TJpgDec.setSwapBytes(false);
//
//  // The decoder must be given the name of the rendering function above
//  TJpgDec.setCallback(tft_output);
  
/////////////////////////////////////////////////////////////////////////////////////////////////
   
  pinMode(M5_LED, OUTPUT);
  M5.Lcd.fillScreen(BLACK);

  displayPixelWidth = 10;   //tft.width() / 8;
  displayPixelHeight = 10;    //tft.height() / 8;

  // default settings
  bool status = amg.begin();
  if (!status) {
      Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
      while (1);
  }
  
  Serial.println("-- Thermal Camera Test --");
  delay(100); // let sensor boot up

  // initialize centroid tracker with persistence over 5 frames:
  centroidTracker = new CentroidTracker(5); 

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  else {
    Serial.println("SPIFFS mounted..");
  }

  // if home butotn is pressed during startup, enable WiFi Server
  if(digitalRead(M5_BUTTON_HOME) == LOW) {
    WIFI_ENABLE = 1;
  }
  if(WIFI_ENABLE) {
    Serial.println("Starting WiFi Server..");
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
  
    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
  
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html);
    });
  
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/css", style_css);
    });
  
    server.on("/thermal", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/thermal.bmp", "image/bmp", false);
    });
  
    server.begin();   // start server
  }
  else {
    Serial.println("WiFi Server disabled (hold M5 home button during startup to enable).");
  }
}

void loop() {
  // read AMG8833 sensor data into pixels array
  amg.readPixels(pixels);

  /*
  // print pixels array values to the Serial port
  Serial.print("[");
  for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
    Serial.print(pixels[i-1]);
    Serial.print(", ");
    if( i%8 == 0 ) Serial.println();
  }
  Serial.println("]");
  Serial.println();
  */
  
  avgVal = 0; // initialize average value
  
  for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
    int colorIndex = map(pixels[i], MINTEMP, MAXTEMP, 0, 255);
    colorIndex = constrain(colorIndex, 0, 255);

    avgVal += pixels[i];
    
    M5.Lcd.fillRect(displayPixelHeight * floor(i / 8), displayPixelWidth * (i % 8),
        displayPixelHeight, displayPixelWidth, camColors[colorIndex]);
  }

  // write BMP image to memory if M5 home button is pressed
  if(digitalRead(M5_BUTTON_HOME) == LOW) {
    PixelsToBmp();
  }

  avgVal /= 64.0; // divide by number of pixels to obtain average
  //Serial.print("avgVal: ");
  //Serial.println(avgVal);

  // store 'X' or 'O' in array m depending based on pixels (temperature) values
  int n = 0;  // pixels array index
  for(int j=0; j<8; j++) {
    for(int i=0; i<8; i++) {
      if(pixels[n] > TEMP_THRESHOLD) { 
        m[i][j] = 'X';
      }
      else {
        m[i][j] = 'O';
      }
      n++;
    }
  }

  // get the number of blobs detected in the sensor's field of view
  // count_blob function will also update the global variable numBlobTotal 
  int numBlobs = count_blob(m, 8, 8);
  //Serial.print("numBlobs: ");
  //Serial.println(numBlobs);
  //Serial.print("numBlobsTotal: ");
  //Serial.println(numBlobsTotal);

  if(numBlobs > 1) {
    digitalWrite(M5_LED, LOW);
    delay(1000);
    digitalWrite(M5_LED, HIGH);
    delay(1000);
  }
  else {
    digitalWrite(M5_LED, HIGH);
  }
  // turn on built-in LED if a blob (~person) is detected
  if(numBlobs > 0) {
    M5.Lcd.setCursor(2, 85);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(WHITE, BLACK);  // white on black background
    M5.Lcd.printf("%d", numBlobs);
    M5.Lcd.drawRect(0, 85, 8, 8, WHITE);
    
  }
  else {
    M5.Lcd.setCursor(2, 85);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(WHITE, BLACK);  // white on black background
    M5.Lcd.printf("%d", numBlobs);
    M5.Lcd.fillRect(0, 85, 8, 8, BLACK);
    M5.Lcd.drawRect(0, 85, 8, 8, BLACK);
  }

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10, 90);
  M5.Lcd.print("AVG TEMP");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 102);
  M5.Lcd.print(avgVal);

  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10, 126);
  M5.Lcd.print("BLOBS/TOTAL");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 138);
  M5.Lcd.printf("%d/%d", numBlobs, numBlobsTotal);
 
  delay(100);
}
