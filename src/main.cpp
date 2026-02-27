
/*-----------------------------------------------------------------------------------------------------------------

   Edit an integer number with the line editor 

   Revision History:
   V1.0 2017 01 18 ChrisMicro, initial version

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

  ---------------------------------------------------------------------------------------------------------------*/

#include <Arduino.h>
#include "FastAccelStepper.h"
#include "editor.h"
#include "ext_editor.h"
#include <Bluepad32.h>

#define BEGIN_LINE 8u
#define BEGIN_COL 10u

#define DRV8825

#ifdef DRV8825
#define EN_PIN 4
#define DIR_PIN 5
#define STEP_PIN 6
#define SLEEP_PIN 7
#define RESET_PIN 8

#define M0 9
#define M1 10
#define M2 11
#endif

#define MOVENL(BL,BC) \
  line_cnt++;\
  move((BL)+line_cnt,(BC));

// TaskHandle_t Task1;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper1 = NULL;


bool last_pressed = false;
struct States{
    int32_t lt;
    int32_t rt;
    uint16_t x;
    uint16_t y;
    bool xy_switch;
};

struct States gamepad_states = {
    .lt = 0,
    .rt = 0,
    .x = 0,
    .y = 0,
    .xy_switch = 0

};

char Arduino_getchar();
void Arduino_putchar(uint8_t c);


void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            move(0,0);
            clrtobot();
            move(BEGIN_LINE,BEGIN_COL);
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            Serial.println(" ");
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            Serial.println(" ");
            myControllers[i] = ctl;
            foundEmptySlot = true;
            Serial.println("Press any key to confirm.");
            Arduino_getchar();
            break;
        }
    }
    // if (!foundEmptySlot) {
        // Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    // }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            move(0,0);
            clrtobot();
            move(BEGIN_LINE,BEGIN_COL);
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            Serial.println(" ");
            myControllers[i] = nullptr;
            foundController = true;
            Serial.println("Press any key to confirm.");
            Arduino_getchar();
            break;
        }
    }

    // if (!foundController) {
        // Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    // }
}

void customGetGamepadInput(ControllerPtr ctl) {
    // if (ctl->x() && ctl->y() && last_pressed == 0){
        // if (states_holder.x==true) states_holder.x=false;
        // else if (states_holder.x==false) states_holder.x=true;
        // last_pressed=1;
    // }else if(!ctl->x() && !ctl->y()) last_pressed = 0;
    // 
    // if(ctl->axisRY()<=-99)
        // states_holder.x_range = map(ctl->axisRY(),0,-512,1000,2000);
    // else states_holder.x_range = 1000;
    // states_holder.lt = ctl->brake();
    // states_holder.rt = ctl->throttle();
    
    // states_holder.left_reverse = (LB & (ctl->buttons())) ? 1 : 0;
    // states_holder.right_reverse = (RB & (ctl->buttons())) ? 1 : 0;

    Serial.printf(
        "LT[%4d] RT[%4d]",
        gamepad_states.lt,
        gamepad_states.rt
    );
        // "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake(LT): %4d, throttle(RT): %4d, "
        // "misc: 0x%02x\n",
        // ctl->index(),        // Controller Index
        // ctl->dpad(),         // D-pad
        // ctl->buttons(),      // bi/tmask of pressed buttons
        // ctl->axisX(),        // (-511 - 512) left X Axis
        // ctl->axisY(),        // (-511 - 512) left Y axis
        // ctl->axisRX(),       // (-511 - 512) right X axis
        // ctl->axisRY(),       // (-511 - 512) right Y axis
        // ctl->brake(),        // (0 - 1023): brake button
        // ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        // ctl->miscButtons()  // bitmask of pressed "misc" buttons
        // ctl->gyroX(),        // Gyro X
        // ctl->gyroY(),        // Gyro Y
        // ctl->gyroZ(),        // Gyro Z
        // ctl->accelX(),       // Accelerometer X
        // ctl->accelY(),       // Accelerometer Y
        // ctl->accelZ()        // Accelerometer Z
    // );
}

void processGamepad(ControllerPtr ctl) {
    if (ctl->x() && ctl->y() && last_pressed == 0){
        // if (gamepad_states.xy_switch==true) gamepad_states.xy_switch=false;
        // else if (gamepad_states.xy_switch==false) gamepad_states.xy_switch=true;
        gamepad_states.xy_switch = true;
        last_pressed=1;
    }else if(!ctl->x() && !ctl->y()) {
      last_pressed = 0;
      gamepad_states.xy_switch = false;
    }

    gamepad_states.lt = ctl->brake();
    gamepad_states.rt = ctl->throttle();
    gamepad_states.y = ctl->y();
    gamepad_states.x = ctl->x();

    // if (ctl->x()) {
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S, Stadia support rumble.
        // It is possible to set it by calling
        // Some controllers have two motors: "strong motor", "weak motor".
        // It is possible to control them independently.
        // ctl->playDualRumble(0 /* delayedStartMs */, 100 /* durationMs */, 0xFF /* weakMagnitude */,
                            // 0xFF /* strongMagnitude */);
    // }


    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
    // dumpGamepad(ctl);
    // customGetGamepadInput(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() /*&& myController->hasData()*/) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                move(2,BEGIN_COL);
                clrtoeol();
                Serial.print("Unsupported controller");
            }
        }
    }
}

class Menu {
  public:
   
    // Menu(){
      // gamepad_setup();
    // }
    struct MenuStates {
      uint8_t bt;
    }menu_states;

    struct Servo {
      int16_t step_size;
      uint32_t speed_hz;
      uint32_t max_speed;
      uint32_t min_speed;
      int32_t acceleration;
      int32_t current_pos;
    } servo_states;
    
    void motor_setup(){
      move(0,0);
      clrtobot();
      move(BEGIN_LINE,BEGIN_COL);
      Serial.print("MOTOR1: INFO: Setup begins.");
      pinMode(M0,OUTPUT);
      digitalWrite(M0,LOW);
      pinMode(M1,OUTPUT);
      digitalWrite(M1,LOW);
      pinMode(M2,OUTPUT);
      digitalWrite(M2,LOW);

      servo_states.speed_hz = 4000;
      servo_states.max_speed = 8000;
      servo_states.min_speed = 500;
      servo_states.speed_hz = 4000;
      servo_states.acceleration = 10000;
      servo_states.current_pos = 0;
      servo_states.step_size = 1;

      engine.init();
      stepper1 = engine.stepperConnectToPin(STEP_PIN);
      if (stepper1) {
      
        stepper1->setDirectionPin(DIR_PIN);
        stepper1->setEnablePin(EN_PIN);
        stepper1->setAutoEnable(true);
      
        // If auto enable/disable need delays, just add (one or both):
        // stepper->setDelayToEnable(50);
        // stepper->setDelayToDisable(1000);
      
        stepper1->setSpeedInHz(servo_states.speed_hz);  // the parameter is us/step !!!
        stepper1->setAcceleration(servo_states.acceleration);
        stepper1->setCurrentPosition(servo_states.current_pos);
        move(BEGIN_LINE+1,BEGIN_COL);
        Serial.print("MOTOR1: INFO: Stepper motor was successfully set up.");
        move(BEGIN_LINE+2,BEGIN_COL);
        Serial.print("Press any key to confirm.");
        Arduino_getchar();
      } else {
        move(0,0);
        clrtobot();
        move(BEGIN_LINE,BEGIN_COL);
        Serial.print("MOTOR1: ERROR: Stepper motor setup was unsuccessful.");
        move(BEGIN_LINE+1,BEGIN_COL);
        Serial.print("Resetting ESP32, press any key to proceed...");
        Arduino_getchar();
        ESP.restart();
      }
    }
    
    void gamepad_setup(){
      move(0,0);
      clrtobot();
      move(BEGIN_LINE,BEGIN_COL);
      Serial.printf("Setting up Bluetooth begins.");
      move(BEGIN_LINE+1,BEGIN_COL);
      Serial.printf("Bluepad32 Firmware: %s\n", BP32.firmwareVersion());
      const uint8_t* addr = BP32.localBdAddress();
      move(BEGIN_LINE+2,BEGIN_COL);
      Serial.printf("BD local Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
      // Setup the Bluepad32 callbacks
      BP32.setup(&onConnectedController, &onDisconnectedController);
      // "forgetBluetoothKeys()" should be called when the user performs
      // a "device factory reset", or similar.
      // Calling "forgetBluetoothKeys" in setup() just as an example.
      // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
      // But it might also fix some connection / re-connection issues.
      // BP32.forgetBluetoothKeys();
      // Enables mouse / touchpad support for gamepads that support them.
      // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
      // - First one: the gamepad
      // - Second one, which is a "virtual device", is a mouse.
      // By default, it is disabled.
      // BP32.enableVirtualDevice(false);
      // BP32.enableNewBluetoothConnections(true);
      move(BEGIN_LINE+3,BEGIN_COL);
      Serial.printf("Bluetooth has been set up, Press any key to confirm");
      Arduino_getchar();
      menu_states.bt = 1;
    }
    
    void ChangeStepSize(int16_t size){
      servo_states.step_size = size;
      switch(servo_states.step_size){
        case(1):// Full step => m0=0,m1=0,m2=0
          digitalWrite(M0,LOW);
          digitalWrite(M1,LOW);
          digitalWrite(M2,LOW);
          break;
        case(2)://1/2 step
          digitalWrite(M0,HIGH);
          digitalWrite(M1,LOW);
          digitalWrite(M2,LOW);
          break;
        case(4)://1/4 step
          digitalWrite(M0,LOW);
          digitalWrite(M1,HIGH);
          digitalWrite(M2,LOW);
          break;
        case(8)://1/8 step
          digitalWrite(M0,HIGH);
          digitalWrite(M1,HIGH);
          digitalWrite(M2,LOW);
          break;
        case(16)://1/16 step
          digitalWrite(M0,LOW);
          digitalWrite(M1,LOW);
          digitalWrite(M2,HIGH);
          break;
        case(32)://1/32 step
          digitalWrite(M0,LOW);
          digitalWrite(M1,HIGH);
          digitalWrite(M2,HIGH);
          break;
        default:
          break;
      }
    }
    
    void ChangeSpeedHz(uint32_t speed){
      servo_states.speed_hz = speed;
      stepper1->setSpeedInHz(servo_states.speed_hz);  // the parameter is us/step !!!
    }
    
    void ChangeAcceleration(int32_t a){
      servo_states.acceleration = a;
      stepper1->setAcceleration(servo_states.acceleration);  // the parameter is us/step !!!
    }
    
    void SetCurrentPosition(int32_t this_pos){
      stepper1->setCurrentPosition(this_pos);
    }
    
    void GetPosition(){
      servo_states.current_pos = stepper1->getCurrentPosition();
    }
    
    void Move(int32_t steps){
      stepper1->move(steps);
      while(stepper1->isRunning()) {}
    }
    
    void MoveTo(int32_t pos){
      stepper1->moveTo(pos);
      while(stepper1->isRunning()) {}
    }
    
    void restart(){
      ESP.restart();
    }
    
    void STOP(){
      stepper1->stopMove();
    }

    void ProcessMenuInput(){
      // clear();
      uint32_t line_cnt = 0;
      uint8_t ch;
      move(0,0);
      clrtobot();
      move(BEGIN_LINE-5,BEGIN_COL);
      Serial.printf("BT[%d]",
        menu_states.bt);
      move(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("Servo controller Monsieur."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      Serial.print("Current stepper motor config:");
      MOVENL(BEGIN_LINE,BEGIN_COL);
      Serial.printf("Speed:[%u], Accel:[%d], Step size:[1/%u]",
        servo_states.speed_hz,
        servo_states.acceleration,
        servo_states.step_size
      );
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("Choose option by pressing a key:"));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [0] : Change stepper step size 1/x."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [1] : Change stepper speed in Hz."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("  - [q] : Change stepper minimum speed."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("  - [w] : Change stepper maximum speed."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [2] : Change stepper acceleration in step/s^2."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [3] : Set current position."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [4] : Move x steps."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [5] : Move to x position."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [6] : Stop the motor."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("-----------------------------"));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [7] : Enable Bluetooth and connect to controllers."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [8] : Set up motor."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [s] : Steer motor with gamepad (only after establishing connection)."));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("-----------------------------"));
      MOVENL(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR(" [9] : Restart ESP32."));
      
      ch = Arduino_getchar();
      switch (ch){
          case 0x30:          opt_0();                        break;
          case 0x31:          opt_1();                        break;
          case 0x32:          opt_2();                        break;
          case 0x33:          opt_3();                        break;
          case 0x34:          opt_4();                        break;
          case 0x35:          opt_5();                        break;
          case 0x36:          opt_6();                        break;
          case 0x37:          opt_7();                        break;
          case 0x38:          opt_8();                        break;
          case 0x39:          opt_9();                        break;
          case 0x73:          opt_s();                        break;
          case 0x71:          opt_q();                        break;
          case 0x77:          opt_w();                        break;
          // case KEY_ESCAPE:    addstr_P (PSTR("KEY_ESCAPE"));  break;
          case ERR:           delay(100);                     break;
          default:                                 break;
        }
    }

    void opt_q(){
      uint32_t line_cnt = 0;
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static uint32_t minspd = servo_states.min_speed;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set stepper minimum speed in Hz.");
          MOVENL(BEGIN_LINE,BEGIN_COL);
          Serial.printf("After typing in a number press ENTER to confirm input:");
          MOVENL(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Current minimum speed [%u][Hz]",servo_states.min_speed);
          MOVENL(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set new min speed: ");
          MOVENL(BEGIN_LINE,BEGIN_COL);
          minspd = editUInt32(minspd);
          servo_states.min_speed = minspd;
          move(0,0);
          clrtobot();
          line_cnt = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Minimum speed has been changed to [%u]",servo_states.min_speed);
          MOVENL(BEGIN_LINE,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          MOVENL(BEGIN_LINE,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new step size.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }

    void opt_w(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static uint32_t maxspd = servo_states.max_speed;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set stepper maximum speed in Hz.");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("After typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.printf("Current maximum speed [%u][Hz]",servo_states.max_speed);
          move(BEGIN_LINE+3,BEGIN_COL);
          Serial.printf("Set new max speed: ");
          move(BEGIN_LINE+4,BEGIN_COL);
          maxspd = editUInt32(maxspd);
          servo_states.max_speed = maxspd;
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Maximum speed has been changed to [%u]",servo_states.max_speed);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new step size.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }

    void opt_0(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static int16_t stp = servo_states.step_size;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set step size x in {1,2,4,8,16,32}: 1/x.");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("After typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.printf("Current step size [1/%d][step]",servo_states.step_size);
          move(BEGIN_LINE+3,BEGIN_COL);
          Serial.printf("Set step size: ");
          move(BEGIN_LINE+4,BEGIN_COL);
          stp = editInt16(stp);
          if (stp==1 || stp==2 || stp==4 || stp==8 || stp==16 || stp==32){
            ChangeStepSize(stp);
          } else {
            move(0,0);
            clrtobot();
            move(BEGIN_LINE,BEGIN_COL);
            Serial.printf("Invalid step size input.");
            move(BEGIN_LINE+1,BEGIN_COL);
            Serial.printf("Step size must be one of the following {1,2,4,8,16,32}.");
            move(BEGIN_LINE+2,BEGIN_COL);
            Serial.print("Press ENTER or any other key to proceed.");
            Arduino_getchar();
            continue; // if not legal values skip iteration
          }
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Step size has been changed to [%d]",servo_states.step_size);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new step size.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    void opt_1(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static uint32_t spd = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set servo speed in Hz, after typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("Current speed [%u][Hz]",servo_states.speed_hz);
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.printf("Set speed: ");
          move(BEGIN_LINE+3,BEGIN_COL);
          spd = editUInt32(spd);
          ChangeSpeedHz(spd);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Speed has been changed to [%u]",servo_states.speed_hz);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new speed.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    void opt_2(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static int32_t a = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set servo acceleration in step/s^2, after typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("Current acceleration [%d][step/s^2]",servo_states.acceleration);
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.printf("Set acceleration: ");
          move(BEGIN_LINE+3,BEGIN_COL);
          a = editInt32(a);
          ChangeAcceleration(a);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Acceleration has been changed to [%d]",servo_states.acceleration);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new acceleration.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    void opt_3(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          servo_states.current_pos = stepper1->getCurrentPosition();
          uint8_t c1;
          static int32_t p = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Set current position, after typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("Current position [%d]",servo_states.current_pos);
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.printf("Set current position: ");
          move(BEGIN_LINE+3,BEGIN_COL);
          p = editInt32(p);
          SetCurrentPosition(p);
          servo_states.current_pos = p;
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Current position has been changed to [%d]",servo_states.current_pos);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new current position.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    void opt_4(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static int32_t a = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Move x steps, after typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("x = ");
          move(BEGIN_LINE+2,BEGIN_COL);
          a = editInt32(a);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Moving [%d] steps...",a);
          Move(a);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Moved [%d] steps",a);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new steps to move.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    void opt_5(){
      if (stepper1) {
        while(1){
          move(0,0);
          clrtobot();
          uint8_t c1;
          static int32_t a = 0;
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Move to x position, after typing in a number press ENTER to confirm input:");
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.printf("x = ");
          move(BEGIN_LINE+2,BEGIN_COL);
          a = editInt32(a);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Moving to [%d] ...",a);
          MoveTo(a);
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          Serial.printf("Moved to position: [%d].",a);
          move(BEGIN_LINE+1,BEGIN_COL);
          Serial.print("Press ESC to exit to main menu.");
          move(BEGIN_LINE+2,BEGIN_COL);
          Serial.print("Press ENTER or any other key to input new position to move to.");
          c1 = Arduino_getchar();
          switch(c1){
            case '\r': break;
            case KEY_ESCAPE: return;
            default: break;
          }
        }
      }
    }
    
    void opt_6(){
      if (stepper1) {
        while(stepper1->isRunning()){
          move(0,0);
          clrtobot();
          move(BEGIN_LINE,BEGIN_COL);
          STOP();
          Serial.printf("Stopping the motor...");
        }
        move(0,0);
        clrtobot();
        move(BEGIN_LINE,BEGIN_COL);
        Serial.printf("Motor is stationary.");
        move(BEGIN_LINE+1,BEGIN_COL);
        Serial.print("Press ENTER or any other key to proceed.");
        Arduino_getchar();
      }
    }
    
    void opt_7(){
      gamepad_setup();
    }
    
    void opt_8(){
      motor_setup();
    }

    void opt_9(){
      restart();
    }

    void opt_s(){
      move(0,0);
      clrtobot();
      move(BEGIN_LINE,BEGIN_COL);
      addstr_P (PSTR("Steer the motor with LT and RT."));
      move(BEGIN_LINE+1,BEGIN_COL);
      addstr_P (PSTR("Push (X)+(Y) buttons combination to exit gamepad steering mode."));
      // move(7,BEGIN_COL);

      while (1){
        bool dataUpdated = BP32.update();
        processControllers();
        char str[100] = {0}; 
        if (last_pressed) {
          ChangeSpeedHz(servo_states.speed_hz);
          return;
        }
        if (gamepad_states.lt>=99 && gamepad_states.rt<99){
          stepper1->setSpeedInHz(map(gamepad_states.lt,99,1023,servo_states.min_speed,servo_states.max_speed));
          stepper1->move(5);
        }
        if (gamepad_states.rt>=99 && gamepad_states.lt<99){
          stepper1->setSpeedInHz(map(gamepad_states.rt,99,1023,servo_states.min_speed,servo_states.max_speed));
          stepper1->move(-5);
        }
        if(gamepad_states.lt < 99 && gamepad_states.rt < 99){
          stepper1->stopMove();
        }
        
        
        servo_states.current_pos = stepper1->getCurrentPosition();
        move(7,BEGIN_COL);
        clrtobot();
        move(7,BEGIN_COL);
        // clrtoeol();
        // move(7,BEGIN_COL);
        // snprintf(str,100,"LT[%d]RT[%d]X[%d]Y[%d]POS[%d]",
        // Serial.printf("LT[%d]RT[%d]X[%d]Y[%d]POS[%d].",
          // gamepad_states.lt,
          // gamepad_states.rt,
          // gamepad_states.x,
          // gamepad_states.y,
          // servo_states.current_pos
        // );
        // move(8,BEGIN_COL);
        snprintf(str,100,"LT[%d]RT[%d].",
          map(gamepad_states.lt,0,1023,servo_states.min_speed,servo_states.max_speed),
          map(gamepad_states.rt,0,1023,servo_states.min_speed,servo_states.max_speed)
        );
        addstr_P (PSTR(str));
        memset(str,0,sizeof(str));
      }
    }

    void wrong_key(){
      return;
    }

};

Menu menu;


void Arduino_putchar(uint8_t c)
{
  Serial.write(c);
}

char Arduino_getchar()
{
  while (!Serial.available());
  return Serial.read();
}

void setup()
{
  Serial.begin(460800);
  setFunction_putchar(Arduino_putchar); // tell the library which output channel shall be used
  setFunction_getchar(Arduino_getchar); // tell the library which input channel shall be used
  initscr();                            // initialize MCURSES
  clear();
}

void loop()
{
  menu.ProcessMenuInput();
}
