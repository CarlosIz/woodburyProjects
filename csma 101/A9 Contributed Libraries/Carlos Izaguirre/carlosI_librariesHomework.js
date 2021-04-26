//p5,play : http://molleindustria.github.io/p5.play/
//clickable : https://github.com/Lartu/p5.clickable

//buttons
var myButton = new Clickable(),
    myButton2 = new Clickable(),
    myButton3 = new Clickable();

//state checks
var checkA = false,
    checkB = false,
    checkC = false;

//camera
var SCENE_W = innerWidth * 2;
var SCENE_H = innerHeight * 2;

//character
var character1;
var box1, box2, box3;
var bg;

//images for locks
var img1, img2, img3;

function preload() {
    img1 = loadImage("data/Glock.png");
    img2 = loadImage("data/Rlock.png");
    img3 = loadImage("data/Block.png");

}

function setup() {
    createCanvas(innerWidth, innerHeight);

    //.5 zoom is zooming out (50% of the normal size)
    if (keyIsPressed == true && keyCode === SHIFT) {
        camera.zoom = 0.5;
    } else {
        camera.zoom = 1;
    }
    //set the camera position to the ghost position
    camera.position.x = width / 2;
    camera.position.y = height / 2;

    //character stuff
    character1 = createSprite(100, 200, 50, 100);
    box1 = createSprite(-100, -300, 50, 100);
    box1.addImage(img1);
    box2 = createSprite(900, 1000, 50, 100);
    box2.addImage(img2);
    box3 = createSprite(-300, -700, 50, 100);
    box3.addImage(img3);

    var myAnimation = character1.addAnimation('moving', 'data/ghost_walk0001.png', 'data/ghost_walk0002.png', 'data/ghost_walk0003.png', 'data/ghost_walk0004.png');
    myAnimation.offY = 18;

    bg = new Group();

    //background rocks
    for (var i = 0; i < 100; i++) {
        var rock = createSprite(random(-width, SCENE_W + width), random(-height, SCENE_H + height));
        //cycles through rocks 0 1 2
        rock.addAnimation('normal', 'data/rocks' + i % 3 + '.png');
        bg.add(rock);
    }

    //clickable button size and location
    myButton.locate(-100, -200);
    myButton.resize(30, 30);
    myButton.cornerRadius = 30;
    //button 2
    myButton2.locate(900, 900);
    myButton2.resize(30, 30);
    myButton2.cornerRadius = 30;
    //button 3
    myButton3.locate(-300, -600);
    myButton3.resize(30, 30);
    myButton3.cornerRadius = 30;

}

function draw() {
    background(200);

    //camera to char movement
    camera.position.x = character1.position.x;
    camera.position.y = character1.position.y;

    //char overlap
    if (character1.overlap(box1)) {
        checkA = true;

    }
    if (character1.overlap(box2)) {
        checkB = true;

    }
    if (character1.overlap(box3)) {
        checkC = true;

    }

    if (checkA == true && checkB == true && checkC == true) {
        textSize(50);
        text('YOU WIN!', 100, 100);

    }

    //camera move wih mouse
    if (keyIsDown(LEFT_ARROW)) {
        character1.velocity.x -= 1

    }
    if (keyIsDown(RIGHT_ARROW)) {
        character1.velocity.x += 1

    }
    if (keyIsDown(UP_ARROW)) {
        character1.velocity.y -= 1

    }
    if (keyIsDown(DOWN_ARROW)) {
        character1.velocity.y += 1

    }
    // character1.velocity.x = (camera.mouseX - character1.position.x) / 20;
    // character1.velocity.y = (camera.mouseY - character1.position.y) / 20;

    //draw rock
    drawSprites(bg);

    //shadow
    noStroke();
    fill(0, 0, 0, 20);
    ellipse(character1.position.x, character1.position.y + 90, 80, 30);

    //draw char
    drawSprite(character1);

    //clickable button
    myButton.draw();
    myButton2.draw();
    myButton3.draw();
    buttons();
    camStuff();
    // locksA();

    camera.off();

}

// function locksA() {
//     image(img1, -100, -300);
//     image(img2, 900, 1000);
//     image(img3, -300, -700);

// }

function camStuff() {
    //.5 zoom is zooming out (50% of the normal size)
    if (keyIsPressed == true && keyCode === SHIFT) {
        camera.zoom = 0.5;
    } else {
        camera.zoom = 1;
    }
}

function buttons() {
    //button 1
    myButton.onOutside = function () {
        myButton.text = ""
        if (checkA == false) {
            myButton.color = "#FFFFFF"
        } else {
            myButton.color = "#3FEB85"
        }

        myButton.textSize = 20;
    }
    myButton.onHover = function () {
        if (checkA == false) {
            myButton.color = "#E5E5E5";

        } else {
            myButton.color = "#3FEB85";

        }

        myButton.text = "";
        myButton.textSize = 20;

    }
    myButton.onPress = function () {
        myButton.color = "#3FEB85";
        myButton.text = "";
        myButton.textSize = 20;
        checkA = true;

    }

    //bbutton 2
    myButton2.onOutside = function () {
        myButton2.text = ""
        if (checkB == false) {
            myButton2.color = "#FFFFFF"
        } else {
            myButton2.color = "#EB704B"

        }

        myButton2.textSize = 20;

    }
    myButton2.onHover = function () {
        if (checkB == false) {
            myButton2.color = "#E5E5E5";

        } else {
            myButton2.color = "#EB704B";

        }

        myButton2.text = "";
        myButton2.textSize = 20;

    }
    myButton2.onPress = function () {
        myButton2.color = "#EB704B";
        myButton2.text = "";
        myButton2.textSize = 20;
        checkB = true;
    }

    //button 3
    myButton3.onOutside = function () {
        myButton3.text = ""
        if (checkC == false) {
            myButton3.color = "#FFFFFF"

        } else {
            myButton3.color = "#54A2DB ";
        }

        myButton3.textSize = 20;

    }
    myButton3.onHover = function () {
        if (checkC == false) {
            myButton3.color = "#E5E5E5"

        } else {
            myButton3.color = "#54A2DB ";
        }

        myButton3.text = "";
        myButton3.textSize = 20;

    }
    myButton3.onPress = function () {
        myButton3.color = "#54A2DB ";
        myButton3.text = "";
        myButton3.textSize = 20;
        checkC = true;
    }

}