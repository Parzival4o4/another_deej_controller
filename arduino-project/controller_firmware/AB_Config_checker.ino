//##########################################
//         function definitions 
//##########################################
bool is_bool(int x);
bool check_array_values_in_range(int arr[], int min, int max);
bool check_array_values_positive(int arr[]);
void check_config();
bool check_serial_config();


/**
 *@brief checks the entirety of the config
 *
 * Checking is handled in one big function to (hopefully) avoid me forgetting to check a value 
 * (Easier to keep track, if its all in one place)
 *
 */
void check_config() {
  todo("change all values of the Config to strictly uppercase");

  //################################################
  //                  Hardware
  //################################################
  //---- Wire 0 ----
  if (WIRE0_SCL < 0) {
    my_panic("WIRE0_SCL is a pin on the micro controller and should be positive: " + String(WIRE0_SCL));
  }
  if (WIRE0_SDA < 0) {
    my_panic("WIRE0_SDA is a pin on the micro controller and should be positive: " + String(WIRE0_SDA));
  }
  if (WIRE0_SPEED < 100000 || WIRE0_SPEED > 700000) {
    my_panic("WIRE0_SPEED is out of range: " + String(WIRE0_SPEED));
  }
  //---- Wire 1 ----
  if (WIRE1_SCL < 0) {
    my_panic("WIRE1_SCL is a pin on the micro controller and should be positive: " + String(WIRE1_SCL));
  }
  if (WIRE1_SDA < 0) {
    my_panic("WIRE1_SDA is a pin on the micro controller and should be positive: " + String(WIRE1_SDA));
  }
  if (WIRE1_SPEED < 100000 || WIRE1_SPEED > 700000) {
    my_panic("WIRE1_SPEED is out of range: " + String(WIRE1_SPEED));
  }
  //---- PCF8575 ----
  if (PCF8575_ADDRESS < 0x20 || PCF8575_ADDRESS > 0x27) {
    my_panic("PCF8575_ADDRESS should be between 0x20 and 0x27 default is 0x20: " + String(PCF8575_ADDRESS));
  }
  //---- SCD4X Sensor ----
  if (SCD_interval_slow <= 0 ){
    my_panic("SCD_interval_slow should be positive: "  + String(SCD_interval_slow));
  }
  if (SCD_interval_fast <= 0 ){
    my_panic("SCD_interval_fast should be positive: "  + String(SCD_interval_slow));
  }
  //---- Movement Sensor ----
  if (Movement_interval < 0) {
    my_panic("Movement_interval should be positive: "  + String(Movement_interval));
  }
  //---- Encoder -----
  if (ENCODER_COUNT < 0) {
    my_panic("ENCODER_COUNT should be a int bigger 0: " + String(ENCODER_COUNT));
  }
  int Pin_A[] = ENCODER_PIN_A;
  int Pin_B[] = ENCODER_PIN_B;
  int Pin_S[] = ENCODER_PIN_S;
  int connected_to[] = ENCODER_CONNECTED_TO;
  if (!(sizeof(Pin_A) / sizeof(Pin_A[0]) == ENCODER_COUNT)) {
    my_panic("Pin_A arrays has the wrong size");
  }
  if (!(sizeof(Pin_B) / sizeof(Pin_B[0]) == ENCODER_COUNT)) {
    my_panic("Pin_B arrays has the wrong size");
  }
  if (!(sizeof(Pin_S) / sizeof(Pin_S[0]) == ENCODER_COUNT)) {
    my_panic("Pin_S arrays has the wrong size");
  }
  if (!(sizeof(connected_to) / sizeof(connected_to[0]) == ENCODER_COUNT)) {
    my_panic("connected_to arrays has the wrong size");
  }
  if (!(check_array_values_positive(Pin_A) && check_array_values_positive(Pin_B) && check_array_values_positive(Pin_S))) {
    my_panic("one of the ENCODER_PIN arrays contains a negative value");
  }
  if (!check_array_values_in_range(connected_to, 0, 1)) {
    my_panic("ENCODER_CONNECTED_TO contains a invalid value (min: 0, max: 1): ");
  }
  if (!(0 <= ENCODER_MENU && ENCODER_MENU <= ENCODER_COUNT)) {
    my_panic("ENCODER_MENU is invalid: " + String(ENCODER_MENU));
  }
  if (ENCODER_MENU != 3){
    my_panic("ENCODER_MENU should be 3: " + String(ENCODER_MENU));
  }
  //---- Display ----
  if (SCREEN_WIDTH != 128 && SCREEN_HEIGHT != 64) {
    my_panic("SCREEN_WIDTH is not 128 and or SCREEN_HEIGHT is not 64");
  }
  if (ENCODER_ROT_CHANGE_BLOCK_TIME < 0) {
    my_panic("ENCODER_ROT_CHANGE_BLOCK_TIME should be positive: "  + String(ENCODER_ROT_CHANGE_BLOCK_TIME));
  }
  if (FPS<1 || FPS>30){
    my_panic("FPS is not set to a value in range of 1-30: " + String(FPS));
  }

  //################################################
  //                  Software
  //################################################

  // Serial (redundant)
  if (!check_serial_config()) {
    my_panic("config has invalid Serial speed");  // can not be printed since the serial dose not work duh
  }

  if (IGNORE_ROTATION_TIME < 0) {
    my_panic("IGNORE_ROTATION_TIME should be positive as it represents a time: " + String(IGNORE_ROTATION_TIME));
  }

  if (SCREEN_TIMEOUT < 0) {
    my_panic("SCREEN_TIMEOUT should be positive as it represents a time: " + String(SCREEN_TIMEOUT));
  }

  //---- deej ----
  if (DELAY_AFTER_SENDING < 0){
    my_panic("DELAY_AFTER_SENDING should be bigger 0: " + String(DELAY_AFTER_SENDING));
  }

  if ( SLIDER_COUNT < 0) {
    my_panic("SLIDER_COUNT should be a int bigger 0: " + String(SLIDER_COUNT));
  }

  int def_pro[] = DEFAULT_PERCENT; 

  if (!(sizeof(def_pro) / sizeof(def_pro[0]) == SLIDER_COUNT)){
    my_panic("DEFAULT_PERCENT arrays has the wrong size");
  }

  if (!check_array_values_positive(def_pro)){
    my_panic("DEFAULT_PERCENT should be positive");
  }

  if (!check_array_values_in_range(def_pro, 0, 100)){
    my_panic("DEFAULT_PERCENT should be between 0 and 100");
  }

  if (!(0 <= MIC_BOOST_PERCENT && MIC_BOOST_PERCENT <= 100)){
    my_panic("MIC_BOOST_PERCENT should be between 0 and 100 (inclusive): " + String(MIC_BOOST_PERCENT));
  }

  if (!(0 <= DEAD_ZONE_PERCENT && DEAD_ZONE_PERCENT <= 100)) {
    my_panic("DEAD_ZONE_PERCENT should be between 0 and 100 (inclusive): " + String(DEAD_ZONE_PERCENT));
  }

  if (DEEJ_MAX_VAL != 1023) {
    my_panic("DEEJ_MAX_VAL should be 1023: " + String(DEEJ_MAX_VAL));
  }

  //---- co2 alarm ----
  if (CO2_ALARM_LEVEL < 0) {
    my_panic("CO2_ALARM_LEVEL should be positive" + String(CO2_ALARM_LEVEL));
  }

  if (CO2_ALARM_MUTE_TIME < 0){
    my_panic("CO2_ALARM_MUTE_TIME should be positive" + String(CO2_ALARM_MUTE_TIME));
  }


  //################################################
  //                  Input
  //################################################

  if (SHORT_PRESS_CUT_OF <= 0) {
    my_panic("SHORT_PRESS_CUT_OF should be positive (millis): " + String(SHORT_PRESS_CUT_OF));
  }

  //################################################
  //                  Debug
  //################################################


  if (!is_bool(DEBUG)) {
    my_panic("DEBUG should be a bool: " + String(PRINT_ERR));
  }

  if (!is_bool(PRINT_ERR)) {
    my_panic("PRINT_ERR should be a bool: " + String(PRINT_ERR));
  }

  if (!is_bool(ENABLE_PANIC)) {
    my_panic("ENABLE_PANIC should be a bool: " + String(ENABLE_PANIC));
  }

  if(!is_bool(DEEJ_PRINT)){
    my_panic("DEEJ_PRINT should be a bool: " + String(DEEJ_PRINT));
  }
}






//##########################################
//            helper functions
//##########################################

bool check_serial_config() {
  int valid_speeds[] = { 300, 600, 750, 1200, 2400, 4800, 9600, 19200, 31250, 38400, 57600, 74880, 115200, 230400, 230400, 250000, 460800, 500000, 921600, 1000000, 2000000 };
  for (int i = 0; i < sizeof(valid_speeds) / sizeof(valid_speeds[0]); i++) {
    if (SERIAL_SPEED == valid_speeds[i]) {
      return true;
    }
  }
  return false;
}

bool is_bool(int x) {
  return x == 0 || x == 1;
}

/**
 *@brief checks if all values are inside the range, min and max are both inclusive
 */
bool check_array_values_in_range(int arr[], int min, int max) {
  for (int i = 0; i < (sizeof(arr) / sizeof(arr[0])); i++) {
    if (!(min <= arr[i] && arr[i] <= max)) {
      return false;
    }
  }
  return true;
}

bool check_array_values_positive(int arr[]) {
  for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
    if (arr[i] < 0) {
      return false;
    }
  }
  return true;
}
