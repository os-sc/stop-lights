
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

byte pedLocation = 0x00;

void setup() {
  // initialize digital pin 13 as an output.
  //pinMode(13, OUTPUT);

  closeAllLights();
}

void loop() {

}

void closeAllLights() {

}

void savePedLocation(byte location) {
    pedLocation = location;
}

// Returns the Light the pedestrian is waiting at
// or 0 if there is no pedestrian waiting
byte getAndClearPedLocation() {
    byte location = pedLocation;
    pedLocation = 0x00;
    return location;
}

// Returns the Lights a car is waiting at
// or 0 if there is no car waiting
byte checkForCars() {

}

void openLights(byte lights, byte direction) {
    if ( lights & 0x80 ) {
        // Open Light North
    }



}

