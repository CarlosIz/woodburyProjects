var serial;
// set serial baud rate options:
// (and make sure your arduino has a matching baud rate!)
var serialOptions = {
  baudRate: 115200
};

var aioData = "";
var inData = "";
var outData = "";

var pix = [];

var buttonVal = 0; // button input value
var sensorVal = 0; // light sensor input valueXX

var millisLast; // timer variable

var doSerial = false; // enable serial functionality
var doAnimation = true; // enable animation
var doPreview = false; // disable led preview

var rotAngle = 0;
var rotAngle2 = 0;
var rotSpeed = 0.05;

var fadeVal = 0;
var fadeDir = 1;

var fsm = new StateMachine({
  init: 'running',
  transitions: [{
      name: 'stop',
      from: '*',
      to: 'off'
    },
    {
      name: 'run',
      from: '*',
      to: 'running'
    },
    {
      name: 'pulse',
      from: '*',
      to: 'pulsing'
    },
    {
      name: 'aiodata',
      from: '*',
      to: 'color'
    },
    {
      name: 'pulseR',
      from: '*',
      to: 'pulsingR'
    }
  ],
  methods: {
    onStop: function() {
      print('stop..');
    },
    onRun: function() {
      print('run..');
    },
    onPulse: function() {
      print('pulse..');
    },
    onAiodata: function() {
      print('aio data..');
      // use Feed Info AIP string for your Adafruit IO feed:
      var checkURL = "https://io.adafruit.com/api/v2/carbito/feeds/testfeed";
      loadJSON(checkURL, aioGetData);
    },
     onPulseR: function() {
      print('pulseR..');
    }
  }
});


function setup() {
  createCanvas(640, 480);
  frameRate(30);

  if(doSerial) {
    serial = new p5.SerialPort();
    serial.on('list', printList);
    serial.on('data', serialEvent);
    serial.open('COM6', serialOptions); // change to your serial port
    serial.clear();
  }
  else {
    print("simulate with mouse..");
  }
  // fill pix[] array with led coordinates arranged in a circle:
  for (var i = 0; i < 6; i++) {
    pix.push(new Led());
    pix[i].x = int(width / 2 + 200 * cos(TWO_PI / 6.0 * i));
    pix[i].y = int(height / 2 + 200 * sin(TWO_PI / 6.0 * i));
  }

  millisLast = millis();
  print("current state: " + fsm.state);
}

// receive serial data (button and sensor value separate by comma):
function serialEvent() {
  var inString = serial.readLine();
  var splitString = split(inString, ',');

  if (splitString.length == 2) { // received 2 values
    buttonVal = Number(splitString[0]);
    sensorVal = Number(splitString[1]);
    inData = "button: " + buttonVal + ", sensor: " + sensorVal;
  }
  //print(inData);
}

// print available serial ports:
function printList(portList) {
  for (var i = 0; i < portList.length; i++) {
    print(i + " " + portList[i]);
  }
}

// key presses to simulate state machine transitions:
function keyTyped() {
  if (key == ' ') {
    doAnimation = !doAnimation;
  } else if (key == 'p') {
    doPreview = !doPreview;
  } else if (key == '0') { 
    fsm.stop();
  } else if (key == '1') {
    fsm.pulse();
  } else if (key == '2') {
    fsm.run();
  } else if (key == 'a') {
    fsm.aiodata();
  } else if (key == '3') {
    fsm.pulseR();
  }
}

function aioGetData(aioJsonData) {
  // print(aioJsonData);
  print("last value: " + aioJsonData.last_value);
  aioData = aioJsonData.last_value;
}

function draw() {
  background(0);

  // sensor input simulation with mouse movement and press:
  if (!doSerial) {
    sensorVal = int(map(mouseX, 0, width, 0, 255));
    rotSpeed = (PI / 15000.0) * sensorVal;

    if (mouseIsPressed)
      buttonVal = 0;
    else
      buttonVal = 1;
    inData = "button: " + buttonVal + ", sensor: " + sensorVal;
  }

  // animations for various program states:
  if (fsm.state == 'running') {
    // example of rotating animation
    if (doAnimation)
      rotAngle += rotSpeed;

    fill(255 , buttonVal * 100, 0, sensorVal);
    push();
    translate(width / 2, height / 2);
    rotate(rotAngle);
    ellipse(200, 0, 50, 50);
    pop();
  } else if (fsm.state == 'pulsing') {
    
    // example of fade up/down animation:
    if (doAnimation) {
      if (fadeDir == 1) {
        if (fadeVal < 255)
          fadeVal += 1 + sensorVal/4;
        else
          fadeDir = -1;
      } else if (fadeDir == -1) {
        if (fadeVal > 0)
          fadeVal -= 1 + sensorVal/4;
        else
          fadeDir = 1;
      }
    }
    for (var i = 0; i < pix.length; i++) {
      // set HSB (hue, saturation, brightness) color mode:
      colorMode(HSB);
      // create color with a hue evenly spaced along the rainbow: 
      var c = color(i * 256 / pix.length, 255, 255);
      // set RGB (red, green, blue) color mode:
      colorMode(RGB);
      // create new color with alpha value:
      var cAlpha = color(red(c), green(c), blue(c), fadeVal);
      noStroke();
      fill(cAlpha);
      rect(pix[i].x - 40, pix[i].y - 40, 80, 80);
    }
  } else if (fsm.state == 'pulsingR') {
    
    // example of fade up/down animation:
    if (doAnimation) {
      
      rotAngle2 += rotSpeed;
      if (fadeDir == 1) {
        if (fadeVal < 255)
          fadeVal += 1 + sensorVal/4;
        else
          fadeDir = -1;
      } else if (fadeDir == -1) {
        if (fadeVal > 0)
          fadeVal -= 1 + sensorVal/4;
        else
          fadeDir = 1;
      }
    }
    for (var t = 0; t < pix.length; t++) {
      push();
      // set HSB (hue, saturation, brightness) color mode:
      colorMode(HSB);
      // create color with a hue evenly spaced along the rainbow: 
      var c = color(t * 256 / pix.length, 255, 255);
      // set RGB (red, green, blue) color mode:
      colorMode(RGB);
      // create new color with alpha value:
      var cAlpha = color(red(c), green(c), blue(c), fadeVal);
      noStroke();
      fill(cAlpha);
      push();
      translate(0, 0);
      rotate(rotAngle2);
      rect(pix[t].x - 40, pix[t].y - 40, 80, 80);
      pop();
      pop();
    }
  }
  else if (fsm.state == 'color') {
    // convert aioData to string and then to color:
    var aioColor = (str(aioData));
    print("aioColor = " + aioColor);
    print("aioData = " + aioData);
    fill(aioColor, 255, 255);
    rect(0, 0, width, height);
  }

  // animate 
  if (doAnimation && (millis() - millisLast > 50)) {
    // built-in p5.js function to load screen pixels into the
    // one-dimensional pixels[] array:
    loadPixels(); 
    var d = pixelDensity(); // pixel density multiplier

    outData = ""; // re-initialize output data string

    // output screen pixel values at pix[] array coodinates:
    for (var i = 0; i < 6; i++) {
      // calculate pixels[] array index based on x, y coordinates:
      var n = 4 * (pix[i].y * d * width * d + pix[i].x * d);
      pix[i].r = pixels[n];
      pix[i].g = pixels[n + 1];
      pix[i].b = pixels[n + 2];
      outData += hexColor(pix[i].r, pix[i].g, pix[i].b);
    }

    // send output string to serial port:
    if(doSerial) 
      serial.write(outData + "\r\n");
    //print(outData);
    millisLast = millis();
  }

  // draw button and sensor values:
  noStroke();
  fill(255);
  text(inData, width / 2 - 50, height / 2 - 50);
  var c = map(sensorVal, 0, 1023, 0, 255);
  fill(c);
  ellipse(width / 2, height / 2, 50, 50);
  noFill();
  strokeWeight(2);
  if (buttonVal == 1)
    stroke(255);
  else
    stroke(0, 255, 0);
  ellipse(width / 2, height / 2, 50, 50);
  noStroke();

  // draw preview of led behavior on screen:
  if (doPreview) {
    noStroke();
    fill(0);
    rect(0, 0, width, height); // hide animation graphics

    for (var i = 0; i < pix.length; i++) {
      fill(pix[i].r, pix[i].g, pix[i].b);
      ellipse(pix[i].x, pix[i].y, 50, 50);
    }
    fill(255);
    text("PREVIEW MODE", width / 2 - 40, height / 2);
  }

  // draw outlines of leds on screen:
  for (var i = 0; i < pix.length; i++) {
    noFill();
    stroke(100);
    ellipse(pix[i].x, pix[i].y, 50, 50);
  }
}

// convert rgb to hexadecimal format #000000 - #ffffff
function hexColor(r, g, b) {
  return "#" + ((r << 16) | (g << 8) | b).toString(16).padStart(6, "0");
}

// led object class definition:
class Led {
  constructor() {
    this.x = this.y = 0; // x, y coordinates
    this.r = this.g = this.b = 0; // r, g, b colors
  }
}