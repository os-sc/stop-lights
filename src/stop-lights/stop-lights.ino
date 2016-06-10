#define NONE            0x00

#define LIGHT_NORTH     0x80
#define LIGHT_EAST      0x40
#define LIGHT_SOUTH     0x20
#define LIGHT_WEST      0x10
#define LIGHT_PED_NORTH 0x08
#define LIGHT_PED_SOUTH 0x04

#define DIR_STRAIGHT    0x80
#define DIR_LEFT        0x40
#define DIR_RIGHT       0x20

// Note that the Sensors/Buttons have the
// same values as the corresponding Lights
#define SENS_NORTH      0x80
#define SENS_EAST       0x40
#define SENS_SOUTH      0x20
#define SENS_WEST       0x10
#define BUTT_NORTH      0x08
#define BUTT_SOUTH      0x04

#define COL_GREEN       0x80
#define COL_YELLOW      0x40
#define COL_RED         0x20

// Times in ms
#define YELLOW_TIME     3000
#define GREEN_TIME      8000

// Lights:
//  10000000 North
//  01000000 East
//  00100000 South
//  00010000 West
//  00001000 Ped. North
//  00000100 Ped. South

// Directions:
//  10000000 Straight
//  01000000 Left
//  00100000 Right

// Sensors & Buttons:
//  10000000 Sensor North
//  01000000 Sensor East
//  00100000 Sensor South
//  00010000 Sensor West
//  00001000 Button North
//  00000100 Button South

// Colors:
//  10000000 Green
//  01000000 Yellow
//  00100000 Red


char pedLocations = 0x00;
char carLocations = 0x00;

char lights[6]={
    LIGHT_NORTH,
    LIGHT_EAST,
    LIGHT_SOUTH,
    LIGHT_WEST,
    LIGHT_PED_NORTH,
    LIGHT_PED_SOUTH
};

char directions[3]={
    DIR_STRAIGHT,
    DIR_LEFT,
    DIR_RIGHT
};

char sensors[4]={
    SENS_NORTH,
    SENS_EAST,
    SENS_SOUTH,
    SENS_WEST
};

char addLight(char collection, char newLight) {
    return collection | newLight;
}

// Sets all lights first to yellow and then to red
// If skipYellow is supplied, the yellow phase will be skipped
void closeAllLights(boolean skipYellow) {
    // This assumes there are only 6 traffic lights
    for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
            char pin = getPinNumber(lights[i], directions[j], COL_GREEN);
            if (pin != -1)
                digitalWrite(pin, LOW);
    
            if (!skipYellow) {
                pin = getPinNumber(lights[i], directions[j], COL_YELLOW);
                if (pin != -1)
                    digitalWrite(pin, HIGH);
            }
        }
    }

    delay(YELLOW_TIME);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            char pin = getPinNumber(lights[i], directions[j], COL_YELLOW);
            if (pin != -1)
                digitalWrite(pin, LOW);
    
            pin = getPinNumber(lights[i], directions[j], COL_RED);
            if (pin != -1)
                digitalWrite(pin, HIGH);
        }
    }
}

// Saves the Pedestrians location without overwriting the old one
void savePedLocation(char location) {
    pedLocations = pedLocations | location;
}

// Returns the Light the pedestrian is waiting at
// or 0 if there is no pedestrian waiting
// and resets the variable
char getAndClearPedLocations() {
    char location = pedLocations;
    pedLocations = 0x00;
    return location;
}

// Returns the Lights a car is waiting at
// or 0 if there is no car waiting
char checkForCars() {
    carLocations=0x00;
    for (int i = 0; i < 6; i++) {
        char sensor = getPinNumber(sensors[i],NONE, NONE);
        if ( digitalRead(sensor) == HIGH )
            carLocations = carLocations | sensor;
    }
    return carLocations;
}

// Takes a byte of lights which to change
void changeLights(int lights, char dir, char col = COL_GREEN, boolean skipYellow = false) {
    lights = lights & 0xff;
    
    char light = 0x00;
    while (lights) {
        switch ( __builtin_clz(lights) -24 ) {
            case 0: light = LIGHT_NORTH; break;
            case 1: light = LIGHT_EAST;  break;
            case 2: light = LIGHT_SOUTH; break;
            case 3: light = LIGHT_WEST;  break;
            case 4: light = LIGHT_PED_NORTH; break;
            case 5: light = LIGHT_PED_SOUTH; break;
        }

        if ( col == COL_GREEN )
            openLight(light, dir, skipYellow);
        else
            closeLight(light, dir, skipYellow);

        // clear this light from the lights to continue with the others
        lights = lights ^ light;
    }
}

void openLight(char light, char dir, boolean skipYellow) {
    char green  = getPinNumber(light, dir, COL_GREEN);
    char yellow = getPinNumber(light, dir, COL_YELLOW);
    char red    = getPinNumber(light, dir, COL_RED);

    digitalWrite(red, LOW);
    if (!skipYellow) {
        digitalWrite(yellow, HIGH);
        delay(YELLOW_TIME);
    }
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
}


void closeLight(char light, char dir, boolean skipYellow) {
    char green  = getPinNumber(light, dir, COL_GREEN);
    char yellow = getPinNumber(light, dir, COL_YELLOW);
    char red    = getPinNumber(light, dir, COL_RED);

    digitalWrite(green, LOW);
    if (!skipYellow) {
        digitalWrite(yellow, HIGH);
        delay(YELLOW_TIME);
    }
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
}

// interrup function for button presses
// there is one for each of the directions
// because interrupt functions cannot have a parameter
void intNorthButtonPressed() {
    savePedLocation(LIGHT_PED_NORTH);
}

void intSouthButtonPressed() {
    savePedLocation(LIGHT_PED_SOUTH);
}

// Returns the corresponding pin the device is connected to
// These are defined at the top with the bitmasks
// This function requires GCC Builtins, the Arduino IDE uses GCC as compiler
int getPinNumber(int device, int dir, int color) {
    // Input sanitation: making sure the input is an unsigned 8bit value stored in 32bit
    device = device & 0xff;
    dir    = dir    & 0xff;
    color  = color  & 0xff;

    int devindex = __builtin_clz(device) - 24;
    if(dir) {
        int dirindex = __builtin_clz(device) - 24;
        int colorindex = __builtin_clz(device) - 24;
        return devindex * 9 + dirindex * 3 + colorindex;
    } else {
        return 6 * 9 + devindex;
    }
}


void loop() {
    char lights = 0x00;
  
    // check for pedestrians
    char peds = getAndClearPedLocations();
    char both = LIGHT_PED_NORTH | LIGHT_PED_SOUTH;
    switch (peds) {
        case LIGHT_PED_NORTH:
            // W/E road open + N peds
            addLight(lights, LIGHT_WEST);
            addLight(lights, LIGHT_EAST);
            addLight(lights, LIGHT_PED_SOUTH);
            changeLights(lights, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_EAST, DIR_RIGHT, COL_GREEN);
            break;

        case LIGHT_PED_SOUTH:
            // W/E road open + S peds
            addLight(lights, LIGHT_WEST);
            addLight(lights, LIGHT_EAST);
            addLight(lights, LIGHT_PED_SOUTH);
            changeLights(lights, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_WEST, DIR_RIGHT, COL_GREEN);
            break;
            
        default:
            if (peds != both) break;
            // W/E road open + peds
            addLight(lights, LIGHT_WEST);
            addLight(lights, LIGHT_EAST);
            addLight(lights, LIGHT_PED_NORTH);
            addLight(lights, LIGHT_PED_SOUTH);
            changeLights(lights, DIR_STRAIGHT, COL_GREEN);
            break;
    }
    
    // check for cars
    int cars = checkForCars();
    cars = cars & 0xff;
    switch (__builtin_clz(cars)) {
        case 0:
            // N road open
            changeLights(LIGHT_NORTH, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_NORTH, DIR_LEFT, COL_GREEN);
            changeLights(LIGHT_NORTH, DIR_RIGHT, COL_GREEN);
            break;
        case 1:
            // E road open
            changeLights(LIGHT_EAST, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_EAST, DIR_LEFT, COL_GREEN);
            changeLights(LIGHT_EAST, DIR_RIGHT, COL_GREEN);
            break;
        case 2:
            // S road open
            changeLights(LIGHT_SOUTH, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_SOUTH, DIR_LEFT, COL_GREEN);
            changeLights(LIGHT_SOUTH, DIR_RIGHT, COL_GREEN);
            break;
        case 3:
            // W road open
            changeLights(LIGHT_WEST, DIR_STRAIGHT, COL_GREEN);
            changeLights(LIGHT_WEST, DIR_LEFT, COL_GREEN);
            changeLights(LIGHT_WEST, DIR_RIGHT, COL_GREEN);
            break;
        default:
            // no cars pulled up; W/E road open
            addLight(lights, LIGHT_WEST);
            addLight(lights, LIGHT_EAST);
            changeLights(lights, DIR_STRAIGHT, COL_GREEN);
            break;
    }
    
    delay(GREEN_TIME);
    closeAllLights(false);
}



void setup() {
    pinMode(getPinNumber(LIGHT_NORTH, DIR_STRAIGHT, COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_LEFT,     COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_RIGHT,    COL_GREEN),  OUTPUT);

    pinMode(getPinNumber(LIGHT_NORTH, DIR_STRAIGHT, COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_LEFT,     COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_RIGHT,    COL_YELLOW), OUTPUT);

    pinMode(getPinNumber(LIGHT_NORTH, DIR_STRAIGHT, COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_LEFT,     COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_NORTH, DIR_RIGHT,    COL_RED),    OUTPUT);

    pinMode(getPinNumber(LIGHT_EAST,  DIR_STRAIGHT, COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_LEFT,     COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_RIGHT,    COL_GREEN),  OUTPUT);

    pinMode(getPinNumber(LIGHT_EAST,  DIR_STRAIGHT, COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_LEFT,     COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_RIGHT,    COL_YELLOW), OUTPUT);

    pinMode(getPinNumber(LIGHT_EAST,  DIR_STRAIGHT, COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_LEFT,     COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_EAST,  DIR_RIGHT,    COL_RED),    OUTPUT);

    pinMode(getPinNumber(LIGHT_SOUTH, DIR_STRAIGHT, COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_LEFT,     COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_RIGHT,    COL_GREEN),  OUTPUT);

    pinMode(getPinNumber(LIGHT_SOUTH, DIR_STRAIGHT, COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_LEFT,     COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_RIGHT,    COL_YELLOW), OUTPUT);

    pinMode(getPinNumber(LIGHT_SOUTH, DIR_STRAIGHT, COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_LEFT,     COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_SOUTH, DIR_RIGHT,    COL_RED),    OUTPUT);

    pinMode(getPinNumber(LIGHT_WEST, DIR_STRAIGHT,  COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_LEFT,      COL_GREEN),  OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_RIGHT,     COL_GREEN),  OUTPUT);

    pinMode(getPinNumber(LIGHT_WEST, DIR_STRAIGHT,  COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_LEFT,      COL_YELLOW), OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_RIGHT,     COL_YELLOW), OUTPUT);

    pinMode(getPinNumber(LIGHT_WEST, DIR_STRAIGHT,  COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_LEFT,      COL_RED),    OUTPUT);
    pinMode(getPinNumber(LIGHT_WEST, DIR_RIGHT,     COL_RED),    OUTPUT);

    pinMode(getPinNumber(SENS_NORTH, NONE, NONE), INPUT);
    pinMode(getPinNumber(SENS_EAST,  NONE, NONE), INPUT);
    pinMode(getPinNumber(SENS_SOUTH, NONE, NONE), INPUT);
    pinMode(getPinNumber(SENS_WEST,  NONE, NONE), INPUT);

    int buttonNorth = getPinNumber(BUTT_NORTH, NONE, NONE);
    int buttonSouth = getPinNumber(BUTT_SOUTH, NONE, NONE);
    pinMode(buttonNorth, INPUT);
    pinMode(buttonSouth, INPUT);

    // Attach the Interrupts to handle the pedestrian button presses
    attachInterrupt(
        digitalPinToInterrupt(buttonNorth),
        intNorthButtonPressed,
        RISING
    );
    attachInterrupt(
        digitalPinToInterrupt(buttonSouth),
        intSouthButtonPressed,
        RISING
    );

    // On start set all lights to red as a security precaution
    closeAllLights(true);
}
