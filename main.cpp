// Define motor variables

#define MOTOR1_PIN_A 17
#define MOTOR1_PIN_B 27
#define MOTOR2_PIN_A 22
#define MOTOR2_PIN_B 23



//Skip driving forward?
bool driveforward = true;

//drive setup (measurements in inches)

int wheelspan = 2.5;

int in_per_sec = 1;
int circle_radius = 12;


#include <iostream>
#include <pigpio.h>
#include <string>
#include <thread>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cmath>

using namespace std;


void operateCamera(){
    system("libcamera-vid -t 0 --inline --listen -o tcp://0.0.0.0:8554");
}

void m1(int power){
    if(power < 0){
        gpioPWM(MOTOR1_PIN_A, abs(power));
        gpioPWM(MOTOR1_PIN_B, 0);
    }else{
        gpioPWM(MOTOR1_PIN_B, abs(power));
        gpioPWM(MOTOR1_PIN_A, 0);
    }
}

void m2(int power){
    if(power < 0){
        gpioPWM(MOTOR2_PIN_A, abs(power));
        gpioPWM(MOTOR2_PIN_B, 0);
    }else{
        gpioPWM(MOTOR2_PIN_B, abs(power));
        gpioPWM(MOTOR2_PIN_A, 0);
    }
}

int main(){
    //start camera operation
    thread camera = thread(operateCamera);
    
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
    
    gpioSetPWMfrequency(MOTOR1_PIN_A, 1000);
    gpioSetPWMfrequency(MOTOR1_PIN_B, 1000);
    gpioSetPWMfrequency(MOTOR2_PIN_A, 1000);
    gpioSetPWMfrequency(MOTOR2_PIN_B, 1000);

    gpioSetPWMrange(MOTOR1_PIN_A, 255);
    gpioSetPWMrange(MOTOR1_PIN_B, 255);
    gpioSetPWMrange(MOTOR2_PIN_A, 255);
    gpioSetPWMrange(MOTOR2_PIN_B, 255);
    
    //Start program on user command
    cout << "\033[1;46mPress ENTER to start program...\033[0m";
    cin.get();
    
    cout << "\033[46mStarting program...\033[0m" << endl;

    //start driving forwards
    if(driveforward){
        m1(255);
        m2(255);
        cout << "\033[42mDriving for " << in_per_sec * circle_radius << " seconds forward.\033[0m" << endl;
        sleep(in_per_sec * circle_radius);
        m1(255);
        m2(-255);
        cout << "\033[42mDriving for " << ((2*wheelspan*M_PI)/4)*in_per_sec*1000000 << " seconds turn.\033[0m" << endl;
        usleep(((2*wheelspan*M_PI)/4)*in_per_sec*1000000); // calculate for 90deg turn (1/4 of circumference)
    }

    //start driving and stop when made to
    m1(255);
    m2(204);
    cout << "\033[1;46mPress ENTER to stop.\033[0m";
    cin.get();

    //terminate gpio at end and camera
    system("pkill libcamera-vid");
    m1(0);
    m2(0);
    camera.join();
    gpioTerminate();
}