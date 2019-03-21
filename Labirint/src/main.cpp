#include <MPU9250.h>
#include <Tweak.h>
#include "maze.h"
// #include "FreeMemory.h"

#define IMU_FILTER  0.3F
// #define TO_DEG 57.29577951308232087679815481410517033f
// #define FK 0.1
#define BUTTON_PIN  0
#define BUTTON_IRQ  2

void tweak_irq();
void button_irq();

volatile float GX = 0;
volatile float GY = 0;

uint16_t imu_period_ms = 0;
volatile uint8_t flag_upd_maze = 0;

Maze* maze;
MPU9250* imu;


void setup() {
    // Serial.begin(9600);
    // delay(2000);

    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(BUTTON_IRQ, button_irq, FALLING);

    maze = new Maze();
    imu = new MPU9250(Wire, 0x68);

    imu->begin();
    imu->calibrateGyro();
    // imu->calibrateAccel();

    Tweak::attachMsCallback(tweak_irq, 300);
}

void loop() {
    uint32_t t1 = millis();
    imu->readSensor();
    /* Accel */
    // float ax = imu->getAccelX_mss();
    // float ay = imu->getAccelY_mss();
    // float angle_ax = 90 - TO_DEG*acos(ay);
    // float angle_ay = 90 - TO_DEG*acos(ax);
    /* Gyroscope */
    float gx = imu->getGyroX_rads();
    float gy = imu->getGyroY_rads();
    if (imu_period_ms > 0) {
        GX += gx * imu_period_ms/1000;
        GY += gy * imu_period_ms/1000;
    }
    /* Complementary filter */
    // GX = GX*(1 - FK) + angle_ax*FK;
    // GY = GY*(1 - FK) + angle_ay*FK;

    if (flag_upd_maze) {
        flag_upd_maze = 0;
        uint8_t direction;
        if ((abs(GX) - IMU_FILTER) < 0)
            GX = 0.0;
        if ((abs(GY) - IMU_FILTER) < 0)
            GY = 0.0;

        float GX_abs = abs(GX);
        float GY_abs = abs(GY);

        if (GX_abs > 0 || GY_abs > 0) {
            if (GX_abs >= GY_abs) {
                /* UP or DOWN */
                direction = (GX > 0) ? DOWN : UP;
                GY = 0.0;
            } else {
                /* LEFT or RIGHT */
                direction = (GY > 0) ? LEFT : RIGHT;
                GX = 0.0;
            }
            maze->upd(direction);
        }
    }

    uint32_t t2 = millis();
    imu_period_ms = t2 - t1;
}

void tweak_irq() {
    flag_upd_maze = 1;
}

void button_irq() {
    GX = 0.0;
    GY = 0.0;
}