#include <EEPROM.h>
#include "Cmd.h"

#define VERSION "1.0.0"

// maximum input lines
#define MAX_INPUT    12

// keyboard values
char outputKeys[MAX_INPUT];

// pin numbers
char inputPins[MAX_INPUT];

// the state of pressed buttons
int16_t inputState = 0;

// is debugging on
bool debug = false;

// has the serial connection been initialized
int hasSerial = false;

// controller name
char controllerName[17];

void initPins() {
    // initialize pins
    for(int i = 0; i < MAX_INPUT; i++) {
        pinMode(inputPins[i], INPUT);
        digitalWrite(inputPins[i], HIGH);
    }
}

void loadDefaults() {
    inputPins[0] = 2;   
    inputPins[1] = 3;   
    inputPins[2] = 4;   
    inputPins[3] = 5;   
    inputPins[4] = 6;   
    inputPins[5] = 7;   
    inputPins[6] = 8;   
    inputPins[7] = 9;   
    inputPins[8] = 10;   
    inputPins[9] = 16;   
    inputPins[10] = 14;   
    inputPins[11] = 15;
    
    outputKeys[0] = 'a';
    outputKeys[1] = 'b';
    outputKeys[2] = 'c';
    outputKeys[3] = 'd';
    outputKeys[4] = 'e';
    outputKeys[5] = 'f';
    outputKeys[6] = 'g';
    outputKeys[7] = 'h';
    outputKeys[8] = 'i';
    outputKeys[9] = 'j';
    outputKeys[10] = 'k';
    outputKeys[11] = 'l';
    
    strcpy(controllerName, "unnamed-ctrl");
    initPins();
}

void writeToEeprom() {
    int i;
    int checksum = 127;
    int offset = 0;
    for(i = 0; i < MAX_INPUT; i++) {
        EEPROM.write(offset, inputPins[i]);
        checksum = (checksum + inputPins[i]) % 255;
        offset++;
    }
    for(i = 0; i < MAX_INPUT; i++) {
        EEPROM.write(offset, outputKeys[i]);
        checksum = (checksum + outputKeys[i]) % 255;
        offset++;
    }
    for(i = 0; i < sizeof(controllerName); i++) {
        EEPROM.write(offset, controllerName[i]);
        checksum = (checksum + controllerName[i]) % 255;
        offset++;   
    }
    EEPROM.write(offset, checksum);
}

void readFromEeprom() {
    int eChecksum;
    int i;
    int checksum = 127;
    int offset = 0;
    
    for(i = 0; i < MAX_INPUT; i++) {
        inputPins[i] = EEPROM.read(offset);
        checksum = (checksum + inputPins[i]) % 255;
        offset++;
    }
    for(i = 0; i < MAX_INPUT; i++) {
        outputKeys[i] = EEPROM.read(offset);
        checksum = (checksum + outputKeys[i]) % 255;
        offset++;
    }
    for(i = 0; i < sizeof(controllerName); i++) {
        controllerName[i] = EEPROM.read(offset);
        checksum = (checksum + controllerName[i]) % 255;
        offset++;   
    }
    eChecksum = EEPROM.read(offset);
    if (eChecksum != checksum) {
        if (Serial) {
            Serial.print("Error: Checksum mismatch for EEPROM: ");
            Serial.print(eChecksum);
            Serial.print(" vs calculated ");
            Serial.println(checksum);
        }
        
        loadDefaults();
    } else {
        initPins();
    }
}



void cmdStore(int argc, char **argv) {
    if (argc != 1) {
        Serial.print("Error: store expects no arguments, but ");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }
    
    writeToEeprom();
    Serial.println("done.");
}

void cmdLoad(int argc, char **argv) {
    if (argc != 1) {
        Serial.print("Error: load expects no arguments, but ");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }
    
    readFromEeprom();
    Serial.println("done.");
}

void cmdDebug(int argc, char **argv) {
    if (argc == 1) {
        if (debug) {
            Serial.println("Debug is on.");
        } else {
            Serial.println("Debug is off.");
        }
        return;   
    }
    
    
    if (argc != 2) {
        Serial.print("Error: debug expects 1 argument, but ");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }
    
    if (strcmp(argv[1], "1") == 0) {
        debug = true;
        Serial.println("Debug on.");
    } else if (strcmp(argv[1], "0") == 0) {
        debug = false;
        Serial.println("Debug off.");
    } else {
        Serial.print("Error: debug expects argument 1 to be eighter 1 or 0, but found ");
        Serial.print(argv[1]);
        Serial.println(".");
    }
}

void cmdDefaults(int argc, char **argv) {
    if (argc != 1) {
        Serial.print("Error: store expects no arguments, but ");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }
    
    loadDefaults();
    Serial.println("done.");    
}

void cmdButton(int argc, char **argv) {
    if (argc != 3) {
        Serial.print("usage: button input_num output_char");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }
    
    if (strlen(argv[2]) != 1) {
        Serial.println("Error: output_char must be a single character.");
        return;
    }
    
    int input = atoi(argv[1]);
    if (input < 0 || input > MAX_INPUT) {
        Serial.print("Error: input_num must be between 0 and ");
        Serial.print(MAX_INPUT);
        Serial.println(".");
        return;
    }
    
    outputKeys[input] = argv[2][0];
    
    Serial.println("done.");
}

void cmdDump(int argc, char **argv) {
    char buff[16];
    if (argc != 1) {
        Serial.print("Error: store expects no arguments, but ");
        Serial.print(argc - 1);
        Serial.println(" given.");
        return;
    }

    Serial.println(" # PIN CHAR");
    for(int i = 0; i < MAX_INPUT; i++) {
        snprintf(buff, 16, "%2d %3d %4c\n", i, inputPins[i], outputKeys[i]);
        Serial.print(buff);
    }
}

void cmdName(int argc, char **argv) {
    if (argc == 1) {
        Serial.print("Controller name is ");
        Serial.print(controllerName);
        Serial.println(".");
    } else if (argc == 2) {
        if (strlen(argv[1]) > 16) {
            Serial.println("Error: Controller name must not exceed 16 characters.");
            return;
        }
        
        strcpy(controllerName, argv[1]);
        Serial.println("done.");
    } else {
        Serial.println("usage: name [new_name]");
    }
}


void cmdHelp(int argc, char **argv) {
    Serial.println("button   - change button configuration.");
    Serial.println("store    - store current buttons setup in EEPROM memory.");
    Serial.println("load     - loads buttons setup from EEPROM memory.");
    Serial.println("name     - changes controller name.");
    Serial.println("defaults - replace current buttons setup with default values.");
    Serial.println("dump     - dumps current buttons setup.");
    Serial.println("debug    - changes debugging mode.");
    Serial.println("help     - list commands.");
}

void setup() {
    cmdInit(115200);
    Keyboard.begin();
    
    cmdAdd("store", cmdStore);
    cmdAdd("load", cmdLoad);
    cmdAdd("debug", cmdDebug);
    cmdAdd("defaults", cmdDefaults);
    cmdAdd("button", cmdButton);
    cmdAdd("dump", cmdDump);
    cmdAdd("name", cmdName);
    cmdAdd("help", cmdHelp);
    
    readFromEeprom();
}

void loop() {
    int newState, oldState;
  // put your main code here, to run repeatedly:
  
    if (Serial && !hasSerial) {
        // serial just connected
        hasSerial = true;
        Serial.println("P.A.I.C. - Programmable Arcade Input Controller - version " VERSION);
        Serial.print("Controller name: ");
        Serial.println(controllerName);
        cmdPrompt();
    } else if (!Serial && hasSerial) {
        // serial just disconnected
        hasSerial = false;
        cmdReset();
    }
    
    if (Serial && hasSerial) {
        // serial is connected
        cmdPoll();
    }
    
    for(int i = 0; i < MAX_INPUT; i++) {
        newState = digitalRead(inputPins[i]) == LOW ? 1 : 0;
        oldState = (inputState & (1 << i)) ? 1 : 0;
        if (newState == oldState) {
            // state didn't change, continue
            continue;
        } else if (newState == 1) {
            // key was pressed
            if (debug && Serial) {
                Serial.print(i); Serial.println(" was pressed");
            }
            Keyboard.press(outputKeys[i]);
            inputState = inputState | (1 << i);
        } else if (oldState == 1) {
            // key was released
            if (debug && Serial) {
                Serial.print(i); Serial.println(" was released");
            }
            Keyboard.release(outputKeys[i]);
            inputState = inputState & ~(1 << i);
        }
    }
    
    delay(20);
}

