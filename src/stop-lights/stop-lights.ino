typedef int boolean;
#define TRUE  1
#define FALSE 0

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
#define GREEN_TIME      6000
#define PEDESTRIAN_TIME 3000

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

char lights[5]={
    LIGHT_NORTH,
    LIGHT_EAST,
    LIGHT_SOUTH,
    LIGHT_WEST,
    LIGHT_PED_NORTH,
    LIGHT_PED_SOUTH
};

char sensors[3]={
    SENS_NORTH,
    SENS_EAST,
    SENS_SOUTH,
    SENS_WEST
};

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
        intNorthButtonPressed(),
        RISING
    );
    attachInterrupt(
        digitalPinToInterrupt(buttonSouth),
        intSouthButtonPressed(),
        RISING
    );

    // On start set all lights to red as a security precaution
    closeAllLights(TRUE);
}

void loop() {

}

// Sets all lights first to yellow and then to red
// If noYellow is supplied, the yellow phase will be skipped
void closeAllLights(boolean noYellow) {
    // This assumes there are only 6 traffic lights
    for (int i = 0; i < 6; i++) {
        char pin = getPinNumber(lights[i], COL_GREEN);
        if (pin != -1)
            digitalWrite(pin, LOW);

        if (!noYellow) {
            pin = getPin(lights[i], COL_YELLOW);
            if (pin != -1)
                digitalWrite(pin, HIGH);
        }
    }

    delay(YELLOW_TIME);

    for (int i = 0; i < 6; i++) {
        char pin = getPin(lights[i], COL_YELLOW);
        if (pin != -1)
            digitalWrite(pin, LOW);

        pin = getPin(lights[i], COL_RED);
        if (pin != -1)
            digitalWrite(pin, HIGH);
    }
}

// Saves the Pedestrians location without overwriting the old one
void savePedLocation(char location) {
    pedLocations = pedLocations | location;
}

// Returns the Light the pedestrian is waiting at
// or 0 if there is no pedestrian waiting
// and resets the variable
char getAndClearPedLocation() {
    char location = pedLocations;
    pedLocations = 0x00;
    return location;
}

// Returns the Lights a car is waiting at
// or 0 if there is no car waiting
char checkForCars() {
    carLocations=0x00;
    for (int i = 0; i < 6; i++) {
        char sensor = getPinNumber(sensors[i], NONE);
        if ( digitalRead(sensor) == HIGH )
            carLocations = carLocations | sensor;
    }
    return carLocations;
}

void openLights(char lights, char direction) {
    if ()
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

    int lightindex = __builtin_clz(device) - 24;
    if(dir) {
        int dirindex = __builtin_clz(device) - 24;
        int colorindex = __builtin_clz(device) - 24;
        return lightindex * 9 + dirindex * 3 + colorindex;
    } else {
        return 6 * 9 + lightindex;
    }
}

/*
// TODO: solve this with the magical power of math
int getPinNumber(char lightName, char dir, char color) {
    switch(lightName) {
        case LIGHT_NORTH:
            switch(dir) {
                case DIR_STRAIGHT:
                    switch(color) {
                        case COL_GREEN:  return  1;
                        case COL_YELLOW: return  2;
                        case COL_GREEN:  return  3;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return  4;
                        case COL_YELLOW: return  5;
                        case COL_GREEN:  return  6;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 10;
                        case COL_YELLOW: return 11;
                        case COL_GREEN:  return 12;
                    }
                    break;
            }

        case LIGHT_EAST:
            switch(dir) {
                case DIR_STRAIGHT:
                    switch(color) {
                        case COL_GREEN:  return 13;
                        case COL_YELLOW: return 14;
                        case COL_GREEN:  return 15;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 16;
                        case COL_YELLOW: return 17;
                        case COL_GREEN:  return 18;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 19;
                        case COL_YELLOW: return 20;
                        case COL_GREEN:  return 21;
                    }
                    break;
            }

        case LIGHT_SOUTH:
            switch(dir) {
                case DIR_STRAIGHT:
                    switch(color) {
                        case COL_GREEN:  return 22;
                        case COL_YELLOW: return 23;
                        case COL_GREEN:  return 24;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 25;
                        case COL_YELLOW: return 26;
                        case COL_GREEN:  return 27;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 28;
                        case COL_YELLOW: return 29;
                        case COL_GREEN:  return 30;
                    }
                    break;
            }

        case LIGHT_WEST:
            switch(dir) {
                case DIR_STRAIGHT:
                    switch(color) {
                        case COL_GREEN:  return 31;
                        case COL_YELLOW: return 32;
                        case COL_GREEN:  return 33;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 34;
                        case COL_YELLOW: return 35;
                        case COL_GREEN:  return 36;
                    }
                    break;

                case DIR_LEFT:
                    switch(color) {
                        case COL_GREEN:  return 37;
                        case COL_YELLOW: return 38;
                        case COL_GREEN:  return 39;
                    }
                    break;
            }

        case LIGHT_PED_NORTH:
            switch(color) {
                case COL_GREEN:  return 40;
                case COL_RED:    return 41;
            }
            break;

        case LIGHT_PED_SOUTH:
            switch(color) {
                case COL_GREEN:  return 42;
                case COL_RED:    return 43;
            }
            break;

        case SENS_NORTH:         return 44;
        case SENS_EAST:          return 45;
        case SENS_SOUTH:         return 46;
        case SENS_WEST:          return 47;
        case BUTT_NORTH:         return 48;
        case BUTT_SOUTH:         return 49;

        default: return -1;
    }*/

