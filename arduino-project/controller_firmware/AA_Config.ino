//################################################
//                  Hardware
//################################################

//---- Wire 0 ----
// IO expander, scd4X,
//esp8266 scl = D1 = 5; sda = D2 = 4
//esp32
#define WIRE0_SCL 23        //positive int (esp pin)
#define WIRE0_SDA 22        //positive int (esp pin)
#define WIRE0_SPEED 400000  // default speed is 100000 max is 400000 oc up to 700000

//---- Wire 1 ----
// Display (sending a new frame to a display can take around 30 ms, which is wy it gets its own bus and threat)
// esp32 (in my configuration): scl = 32, sda = 33
#define WIRE1_SCL 32        //positive int (esp pin)
#define WIRE1_SDA 33        //positive int (esp pin)
#define WIRE1_SPEED 100000  // default speed is 100000 max is 400000 oc up to 700000


//---- PCF8575 ----
#define PCF8575_ADDRESS 0x20  // default address is 0x20 (address min 0x20 max 0x27)

//---- SCD4X Sensor ----
#define SCD_interval_slow 60000  //SCD_interval_slow>0 // millis between measurements
#define SCD_interval_fast 5000   //SCD_interval_fast>0 // millis between measurements

//---- Movement Sensor ----
#define MOTION_PIN 25          //positive int (esp pin) // GPIO 25 in my build
#define Movement_interval 2000  //Movement_interval > 0 // most movement sensor hold the pin signal for some time

//---- Encoder -----
#define ENCODER_COUNT 6  //number of encoders (int)

#define ENCODER_PIN_A \
  { 1, 4, 7, 19, 10, 13 }  //int[]
#define ENCODER_PIN_B \
  { 0, 3, 6, 18, 9, 12 }  //int[]
#define ENCODER_PIN_S \
  { 2, 5, 8, 21, 11, 14 }  //int[]
#define ENCODER_CONNECTED_TO \
  { 1, 1, 1, 0, 1, 1 }  //int[] // 1 connoted to IO Expander 0 connected to ESP

#define ENCODER_MENU 3  //int //if you want to change this you probably will need to change more in the code just leaf it at 3 // 0 < ENCODER_MENU < ENCODER_COUNT

#define ENCODER_ROT_CHANGE_BLOCK_TIME 1  // millis > 0 // blocks a change in rotation direction for the number of millis

//---- Display ----
#define SCREEN_WIDTH 128  //int //program dose not support scaling.
#define SCREEN_HEIGHT 64  //int //dont change
#define FPS 28            //int // min=1 max=30


//################################################
//                  Software
//################################################

//---- file system ----
#define FILE_NAME_SLIDER_VALUES "/slider_values.bin"
#define FILE_NAME_STATE_CONFIG "/state_config.bin"


//---- Serial speed ----
#define SERIAL_SPEED 4800 //115200  //int

//---- Encoder and swich stuff ----
#define IGNORE_ROTATION_TIME 20  // millis, for how long should rotation be ignored after button release

//        once rotation on one encoder is detected all other encoders will be ignored (both button and rotation)
//        this PRIO_TIME defines for how long after the rotation ends the encoder will be prioritized
#define PRIO_TIME 100

//---- Screen off timer -----
#define SCREEN_TIMEOUT 10  // minutes, how long should the program wait to turn of the screen

//---- deej ----
#define DELAY_AFTER_SENDING 8  //positive // millis
#define SLIDER_COUNT 6         // positive // this dose not have to be the same value as ENCODER_COUNT
#define DEFAULT_PERCENT \
  { 75, 50, 50, 75, 50, 43 }  // default volume levels

#define MIC_BOOST_PERCENT 100  // positive 0-100 
#define DEAD_ZONE_PERCENT 50   //  0<= DEAD_ZONE_PERCENT <= 100

#define DEEJ_MAX_VAL 1023  //dont change this // should be 1023

//---- co2 alarm ----
#define CO2_ALARM_LEVEL 1100 //int > 0 //by default if co2 is higher than this an alarm is triggered 

#define CO2_ALARM_MUTE_TIME 15 //int > 0 //minutes 

//################################################
//                  Input
//################################################

//---- switch ----
// any press less than this is short every thing more is a long press
#define SHORT_PRESS_CUT_OF 650  // int, positive, millis


//################################################
//                  Debug
//################################################
#define DEBUG false  //bool //prints free heap and some test text if defined

#define PRINT_ERR true  //bool

#define ENABLE_PANIC true  //bool //true: errors will break the program; false: errors will cause undefined behavior

#define DEEJ_PRINT true  //bool //for debuting it can be useful to disable the deej messages
