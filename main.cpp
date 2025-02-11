// Define motor variables

#define MOTOR1_PIN_A 33
#define MOTOR1_PIN_B 22
#define MOTOR2_PIN_A 33
#define MOTOR2_PIN_B 22



//Skip driving forward?
bool driveforward = true;

//drive setup

int wheelspan = 3;

int in_per_sec = 1;
int circle_radius = 12;


#include <iostream>
#include <pigpio.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <cmath>

using namespace std;


void operateCamera(){
    system("libcamera-vid -t 0 --inline --listen -o tcp://0.0.0.0:8554");
}

void m1(int power){
    if(power < 0){
        gpioPWM(MOTOR1_PIN_A, power);
        gpioPWM(MOTOR1_PIN_B, 0);
    }else{
        gpioPWM(MOTOR1_PIN_B, power);
        gpioPWM(MOTOR1_PIN_A, 0);
    }
}

void m2(int power){
    if(power < 0){
        gpioPWM(MOTOR2_PIN_A, power);
        gpioPWM(MOTOR2_PIN_B, 0);
    }else{
        gpioPWM(MOTOR2_PIN_B, power);
        gpioPWM(MOTOR2_PIN_A, 0);
    }
}

int main(){
    //start camera operation
    thread camera = thread(operateCamera);
    camera.detach();

    //make sure gpio pins are good
    if (gpioInitialise() < 0) {
        cerr << "Error initializing pigpio" << endl;
        return 1;
    }

    //set up GPIO pins and PWM
    gpioSetMode(MOTOR1_PIN_A, PI_OUTPUT);
    gpioSetMode(MOTOR1_PIN_B, PI_OUTPUT);
    gpioSetMode(MOTOR2_PIN_A, PI_OUTPUT);
    gpioSetMode(MOTOR2_PIN_B, PI_OUTPUT);
    gpioSetPWMrange(MOTOR1_PIN_A, 100);
    gpioSetPWMrange(MOTOR1_PIN_B, 100);
    gpioSetPWMrange(MOTOR2_PIN_A, 100);
    gpioSetPWMrange(MOTOR2_PIN_B, 100);

    sleep(10);
    //start driving forwards
    if(driveforward){
        m1(100);
        m2(100);
        sleep(in_per_sec * circle_radius);
        m1(100);
        m2(-100);
        sleep(((2*wheelspan*M_PI)/4)*in_per_sec); // calculate for 90deg turn (1/4 of circumference)
    }
    m1(100);
    m2(80);
    cout << "Press ENTER to stop." << endl;
    cin.get();

    //terminate gpio at end and camera
    system("pkill libcamera-vid");
    gpioTerminate();
}