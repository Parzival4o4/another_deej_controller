
//##########################################
//         function definitions 
//##########################################
void setup_serial();
void setup_wire();
void setup_PFC();



//##########################################
//              global objects
//##########################################
//i need access to these objects in the loop function
State state;
Input_state I_state;  

ModeManager mm(DEEJ_MODE);


//#############################################################################
//##############        _____ ______ _______ _    _ _____        ##############
//##############       / ____|  ____|__   __| |  | |  __ \       ##############
//##############      | (___ | |__     | |  | |  | | |__) |      ##############
//##############       \___ \|  __|    | |  | |  | |  ___/       ##############
//##############       ____) | |____   | |  | |__| | |           ##############
//##############      |_____/|______|  |_|   \____/|_|           ##############
//##############                                                 ##############
//#############################################################################



void setup() {
  // put your setup code here, to run once:
  setup_serial();
  check_config();
  if (check_panic()) {
    return;
  }
  setup_wire();
  setup_PFC();
  setup_display();
  setup_filesystem();
  delay(200);
  I_state = Input_state();
  state.get_deej()->reload_values();
  state.reload_state_config();
  setup_print(); //for debugging, development and testing
}


//##########################################
//            specific setup
//##########################################

void setup_filesystem() {
  if (!SPIFFS.begin(true)) {
    my_panic("Failed to mount SPIFFS");
  }
}

void setup_display() {
  OLED_display.begin();
  OLED_display.display();
  OLED_display.setTextWrap(false);
}

/*
 * Setup Serial communication with the Computer
 */
void setup_serial() {
  // check Serial config
  if (!check_serial_config()) {
    my_panic("config has invalid Serial speed");  //yes i am aware that this msg cant be printed
    return;
  }
  Serial.begin(SERIAL_SPEED);
  delay(100);
}

void setup_wire() {
  Wire.begin(WIRE0_SDA, WIRE0_SCL, WIRE0_SPEED);
  Wire1.begin(WIRE1_SDA, WIRE1_SCL, WIRE1_SPEED);
}

void setup_PFC() {
  // ----- setup PCF -----
  PCF.setAddress(PCF8575_ADDRESS);
  PCF.begin(0b1111111111111111);
  delay(200);
  if (!PCF.isConnected()) {
    my_panic("PFC IO expander connection failed (address not visible)");
  }
}
