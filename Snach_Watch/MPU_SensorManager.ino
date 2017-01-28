/*Copyright 2015 Giorgio Gross
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/*
Manages all gyro and acceleration sensor related tasks
as well as all initialisazions and definitions.
*/

// MPU 6050:
MPU6050 mpu(0x69);

#define MPU6050_ACCEL_FS_2 0x00
#define MPU6050_ACCEL_FS_4 0x01
#define MPU6050_ACCEL_FS_8 0x02
#define MPU6050_ACCEL_FS_16 0x03

#define MPU6050_GYRO_FS_250 0x00
#define MPU6050_GYRO_FS_500 0x01
#define MPU6050_GYRO_FS_1000 0x02
#define MPU6050_GYRO_FS_2000 0x03

#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536
#define M_PI 3.14159265359
#define dt 0.05

int GYRO_DATA_X;
int GYRO_DATA_Y;
int GYRO_DATA_Z;
int ACCEL_DATA_X;
int ACCEL_DATA_Y;
int ACCEL_DATA_Z;

// MPU control/status vars
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// averageValues:
int loopCount = 0;


void initializeMPU(){
  Wire.begin();
  TWBR = 24;
  
  // init MPU:
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000); 
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);  
  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  // load and configure the DMP
  devStatus = mpu.dmpInitialize();
  
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);
  
  if (devStatus == 0) {
    // turn on DMP
    mpu.setDMPEnabled(true);
    // enabel interrupt detection:
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR. Do default action..
    dmpReady = false;
  }
  
  mpu.resetFIFO();
  //mpu.setSleepEnabled(true);
}
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void manageSensorData(){
  /*
  Disables Sleep and waits for data input through "interrupt".
  Afterwards manages the received data and sends it to BLE.
  */
  
  //mpu.setSleepEnabled(false);
  // TODO uncomment this.... Though, system freezes sometimes..
  while (!mpuInterrupt && fifoCount < packetSize) {
    // while no MPU data available
    delay(20);
  }
  
  // -> OnInterrupt:
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  
  mpu.resetFIFO();
  // check for overflow (this should never happen)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
  } else if(mpuIntStatus & 0x02){
    
    // new code to test.....
    while (fifoCount >= packetSize){
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
    }
    
     // wait for correct available data length, should be a VERY short wait
     while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
     
     mpu.getFIFOBytes(fifoBuffer, packetSize);
     
     // Gyro values:
     mpu.dmpGetQuaternion(&q, fifoBuffer);
     mpu.dmpGetGravity(&gravity, &q);
     mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
     GYRO_DATA_X = ypr[0] * 180/M_PI;
     GYRO_DATA_Y = ypr[1] * 180/M_PI,
     GYRO_DATA_Z = ypr[2] * 180/M_PI;
     
     // Acceleration values:
     mpu.dmpGetQuaternion(&q, fifoBuffer);
     mpu.dmpGetAccel(&aa, fifoBuffer);
     mpu.dmpGetGravity(&gravity, &q);
     mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
     ACCEL_DATA_X = aaReal.x;
     ACCEL_DATA_Y = aaReal.y;
     ACCEL_DATA_Z = aaReal.z; 
     
     // sendG_DATA_PACK(GYRO_DATA_X, GYRO_DATA_Y, GYRO_DATA_Z, ACCEL_DATA_X, ACCEL_DATA_Y, ACCEL_DATA_Z);
     sendG_DATA_PACK(ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI, aaReal);
     mpu.resetFIFO();
     //mpu.setSleepEnabled(true);
  }
}

void resetMPU(){
  mpu.resetFIFO();
}

void sendG_DATA_PACK(float xG, float yG, float zG, VectorInt16 aaReal){
  String sG_x = String(xG,DEC);
  String sG_y = String(yG,DEC);
  String sG_z = String(zG,DEC);
  
  String sA_x = String(aaReal.x,DEC);
  String sA_y = String(aaReal.y,DEC);
  String sA_z = String(aaReal.z,DEC);
   /*
  Serial.print(xG);
  Serial.print(" ");
  Serial.print(yG);
  Serial.print(" ");
  Serial.print(zG);
  Serial.print(" ");
  Serial.print(aaReal.x);
  Serial.print(" ");
  Serial.print(aaReal.y);
  Serial.print(" ");
  Serial.print(aaReal.z);
  Serial.println(" ");
  
  Serial.println(sG_x+" "+sG_y+" "+sG_z+" "+sA_x+" "+sA_y+" "+sA_z);
  */
  
  MPU_XG = xG;
  MPU_YG = yG;
  MPU_ZG = zG;
  MPU_ACCEL = aaReal;
  
  // sendSnachDataViaBLE(xG,yG,zG,aaReal.x,aaReal.y,aaReal.z, SNACH_SCREEN, SNACH_BUTTON_CLICKED, SNACH_SCROLL_STATE);
  
}

void ComplementaryFilter(float gyro[3], VectorInt16 accel[3]){
  float yawAcc, pitchAcc, rollAcc;
  float yawGy, pitchGy, rollGy;
  
  yawGy = ((float)gyro[0] / GYROSCOPE_SENSITIVITY) *dt;
  pitchGy = ((float)gyro[1] / GYROSCOPE_SENSITIVITY) *dt;
  rollGy = ((float)gyro[2] / GYROSCOPE_SENSITIVITY) *dt;
    
}


