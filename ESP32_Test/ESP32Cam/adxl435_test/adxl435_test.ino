
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
 
//#define I2C_SDA_PIN 15
//#define I2C_SCL_PIN 14
 
#define I2C_SDA_PIN 23
#define I2C_SCL_PIN 17


/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


void displaySensorDetails(void)
{
 sensor_t sensor;
 accel.getSensor(&sensor);
 Serial.println("------------------------------------");
 Serial.print ("Sensor: "); Serial.println(sensor.name);
 Serial.print ("Driver Ver: "); Serial.println(sensor.version);
 Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
 Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
 Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
 Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
 Serial.println("------------------------------------");
 Serial.println("");
 delay(500);
}
 
void displayDataRate(void)
{
 Serial.print ("Data Rate: ");
 
 switch(accel.getDataRate())
 {
  case ADXL345_DATARATE_3200_HZ:
    Serial.print ("3200 ");
    break;
  case ADXL345_DATARATE_1600_HZ:
    Serial.print ("1600 ");
    break;
  case ADXL345_DATARATE_800_HZ:
    Serial.print ("800 ");
    break;
  case ADXL345_DATARATE_400_HZ:
    Serial.print ("400 ");
    break;
  case ADXL345_DATARATE_200_HZ:
    Serial.print ("200 ");
    break;
  case ADXL345_DATARATE_100_HZ:
    Serial.print ("100 ");
    break;
  case ADXL345_DATARATE_50_HZ:
    Serial.print ("50 ");
    break;
  case ADXL345_DATARATE_25_HZ:
    Serial.print ("25 ");
    break;
  case ADXL345_DATARATE_12_5_HZ:
    Serial.print ("12.5 ");
    break;
  case ADXL345_DATARATE_6_25HZ:
    Serial.print ("6.25 ");
    break;
  case ADXL345_DATARATE_3_13_HZ:
    Serial.print ("3.13 ");
    break;
  case ADXL345_DATARATE_1_56_HZ:
    Serial.print ("1.56 ");
    break;
  case ADXL345_DATARATE_0_78_HZ:
    Serial.print ("0.78 ");
    break;
  case ADXL345_DATARATE_0_39_HZ:
    Serial.print ("0.39 ");
    break;
  case ADXL345_DATARATE_0_20_HZ:
    Serial.print ("0.20 ");
    break;
  case ADXL345_DATARATE_0_10_HZ:
    Serial.print ("0.10 ");
    break;
  default:
    Serial.print ("???? ");
    break;
 }
 Serial.println(" Hz");
}
 
void displayRange(void)
{
 Serial.print ("Range: +/- ");
 
 switch(accel.getRange())
 {
    case ADXL345_RANGE_16_G:
      Serial.print ("16 ");
      break;
    case ADXL345_RANGE_8_G:
      Serial.print ("8 ");
      break;
    case ADXL345_RANGE_4_G:
      Serial.print ("4 ");
      break;
    case ADXL345_RANGE_2_G:
      Serial.print ("2 ");
      break;
    default:
      Serial.print ("?? ");
      break;
 }
 Serial.println(" g");
}



#define FILTER_N 12

float Filter(char DIR)   // DIR may be 'x','y','z' 
{
    sensors_event_t event;

    // 平均滤波法
    float filterSum = 0.0;
    
    for(int i = 0; i < FILTER_N; i++) 
    {
        accel.getEvent(&event);

        if(DIR == 'x')
          filterSum += event.acceleration.x ;
        else if(DIR == 'y')
          filterSum += event.acceleration.y ;
        else
          filterSum += event.acceleration.z ;

        delay(5);
    }
    
    return (float)(filterSum / FILTER_N);
}


 
void setup() {

 Serial.begin(115200);
 Serial.println("Accelerometer Test"); Serial.println("");

 Serial.println("Before wire");
 Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
 Serial.println("After wire");

 /* Initialise the sensor */
 if(!accel.begin())
 {
  /* There was a problem detecting the ADXL345 ... check your connections */
  Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  while(1);
 }
 else
  Serial.println("accel.begin sucessful");

 /* Set the range to whatever is appropriate for your project */
 // accel.setRange(ADXL345_RANGE_16_G);
 // accel.setRange(ADXL345_RANGE_8_G);
 // accel.setRange(ADXL345_RANGE_4_G);
 accel.setRange(ADXL345_RANGE_2_G);
 
 /* Display some basic information on this sensor */
 displaySensorDetails();
 
 /* Display additional settings (outside the scope of sensor_t) */
 displayDataRate();
 displayRange();
 Serial.println("");

}

void loop() 
{
 static float oldAccx, oldAccy, oldAccz, oldDot;
 static int stepNumber = 0;

 float accx = Filter('x');  // 读取滤波后的加速度 x 分量
 float accy = Filter('y');
 float accz = Filter('z');
 float dot = (oldAccx * accx)+(oldAccy * accy)+(oldAccz * accz);

 float oldAcc = abs(sqrt(oldAccx * oldAccx + oldAccy * oldAccy + oldAccz * oldAccz));
 float newAcc = abs(sqrt(accx * accx + accy * accy + accz * accz));
 dot /= (oldAcc * newAcc);  // 计算加速度变化程度

 if(abs(dot - oldDot) >= 0.05) 
 {
        // 变化程度超过阈值，则判定步数增加
        // 并打印
        stepNumber += 1;
        Serial.println(stepNumber);
 }
    
 oldAccx = accx;
 oldAccy = accy;
 oldAccz = accz;
 oldDot = dot;

 delay(100); 


 #if 0
 /* Get a new sensor event */
 sensors_event_t event;
 
 accel.getEvent(&event);
 /* Display the results (acceleration is measured in m/s^2) */
 Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
 Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
 Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");Serial.println("m/s^2 ");
 delay(500);
 #endif
}
