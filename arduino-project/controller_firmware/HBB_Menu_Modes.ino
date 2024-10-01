//===================================================================================
////////////////////////////////////MENU_SELECTOR////////////////////////////////////
//===================================================================================
const int MM_item_count = 7;

String MM_MenuItems[] = {
  "-Format SPIFFS-",
  "-Delete Files--",
  "-SPIFFS Files--",
  "-Sensor Offsets",
  "-C02 alarm val-",
  "-Set mic val---",
  "==============="
};
// make sure that MM_MenuItems array is the same size as MM_ModeID_Items
ModeID MM_ModeID_Items[] = {
  FORMAT_FILE,
  DELETE_ALL_FILES,
  SPIFFS_FILES,
  SENSOR_OFFSET,
  C02_ALARM_VAL,
  SET_MIC,
  NO_CHANGE,
};


// display constants
const int center_y = (SCREEN_HEIGHT / 2) - 4;  //(SCREEN_HEIGHT / 2) - 4
const int default_x_offset = 20;
const int radius = 70;

//##########################################
class Menu_Mode : public Mode {
  //########################################

  int menu_item = 0;

  int display_offset = 0;
  const int line_space = 14;

  void change_item(int rotation) {
    display_offset = line_space * -rotation;
    menu_item = (menu_item - rotation) % MM_item_count;
    if (menu_item < 0) {
      menu_item += MM_item_count;
    }
  }

  void frame_done() {
    int sign = getSign(display_offset);
    display_offset = display_offset - 4 * sign;
    if (getSign(display_offset) != sign) {
      display_offset = 0;
    }
  }

  int get_y(int i) {
    int y = (line_space * i) + display_offset;
    return y;
  }

  double radius_quad = std::pow(radius, 2);

  int calc_x_pos(int y) {
    double y_prime_quad = std::pow((center_y - y), 2);
    double x_prime = std::sqrt(radius_quad - y_prime_quad);
    int x = default_x_offset + (radius - x_prime);
    return x;
  }

  ModeID go_to_item() {
    return MM_ModeID_Items[menu_item];
  }

public:
  Menu_Mode() {}

  ~Menu_Mode() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_index()) {
      case ENCODER_MENU:
        switch (input->get_type()) {
          case SWITCH:
            if (input->get_press_type() == JUST_PRESSED) {
              return go_to_item();
              todo("implement switch to menu items");
            }
            break;
          case ROTATION:
            change_item(input->get_int()); 
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    int j = (MM_item_count + menu_item - 3) % MM_item_count;

    for (int i = -1; i < 6; i++) {
      display->setTextSize(1);
      display->setTextColor(SH110X_WHITE);  // Change text color for alarm condition
      int y = get_y(i);
      int x = calc_x_pos(y);
      display->setCursor(x, y);
      display->println(MM_MenuItems[j]);
      j = (j + 1) % MM_item_count;
    }

    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(0, 14 * 2);
    display->println("->");
    frame_done();
  }
};


//===================================================================================
/////////////////////////////////////SPIFFS_FILES////////////////////////////////////
//===================================================================================

//##########################################
class Spiffs_Files : public Mode {
private:
  String file_names = "nothing";

public:
  Spiffs_Files() {
    file_names = spiffs_read_All_Files();
  }

  ~Spiffs_Files() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
     return NO_CHANGE;
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(0, 0);

    display->println(file_names);
  }

};

//===================================================================================
/////////////////////////////////////DELETE_ALL_FILES////////////////////////////////
//===================================================================================

//##########################################
class Delete_All_Files : public Mode {
  //########################################
private:
  bool ok_selected = false;
public:
  Delete_All_Files() {}

  ~Delete_All_Files() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_index()) {
      case ENCODER_MENU:
        switch (input->get_type()) {
          case SWITCH:
            if (input->get_press_type() == JUST_PRESSED) {
              if (ok_selected) {
                spiffs_delete_files();
              }
              return DEFAULT_MODE;
            }
            break;
          case ROTATION:
            int change = input->get_int() % 2;
            if (change != 0) {
              ok_selected = !ok_selected;
            }
            break;
        }
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(5, 0);
    display->println("DEL files");

    display->setTextSize(2);
    display->setCursor(3, 40);
    if (ok_selected) {
      display->println(" NO  [YES]");
    } else {
      display->println("[NO]  YES ");
    }
  }
};

//===================================================================================
/////////////////////////////////////FORMAT_FILE/////////////////////////////////////
//===================================================================================

//##########################################
class Format_File : public Mode {
  //########################################
private:
  bool ok_selected = false;
public:
  Format_File() {}

  ~Format_File() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_index()) {
      case ENCODER_MENU:
        switch (input->get_type()) {
          case SWITCH:
            if (input->get_press_type() == JUST_PRESSED) {
              if (ok_selected) {
                spiffs_format();
              }
              return DEFAULT_MODE;
            }
            break;
          case ROTATION:
            int change = input->get_int() % 2;
            if (change != 0) {
              ok_selected = !ok_selected;
            }
            break;
        }
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(5, 0);
    display->println("MEM format");

    display->setTextSize(2);
    display->setCursor(3, 40);
    if (ok_selected) {
      display->println(" NO  [YES]");
    } else {
      display->println("[NO]  YES ");
    }
  }
};


//===================================================================================
////////////////////////////////////C02_ALARM_VAL////////////////////////////////////
//===================================================================================

//##########################################
class C02_Alarm_Val : public Mode {
  //########################################
private:
  int alarm_value = 0;

public:
  C02_Alarm_Val() {
    int not_needed;
    spiffs_state_config(&not_needed, &not_needed, &not_needed, &alarm_value);
  }

  ~C02_Alarm_Val() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_type()) {
      case SWITCH:
        switch (input->get_index()) {
          case ENCODER_MENU:
            if (input->get_press_type() == JUST_PRESSED) {
              state->save_alarm_val(alarm_value);
              state->reload_state_config();
              return DEFAULT_MODE;
            }
        }
        break;
      case ROTATION:
        switch (input->get_index()) {
          case ENCODER_MENU:
            alarm_value = alarm_value - (input->get_int() * 10);
            break;
        }
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(17, 0);
    display->println("C02 ALARM Value");

    
    display->setCursor(0, 42);
    display->println("save by pressing the"); 
    display->setCursor(0, 55);
    display->println("menu encoder");

    display->setTextSize(2);
    display->setCursor(20, 20);
    display->println(String(alarm_value));

    
  }
};


//===================================================================================
/////////////////////////////////////SENSOR_OFFSET///////////////////////////////////
//===================================================================================

//##########################################
class Sensor_Offset : public Mode {
  //########################################
private:
  bool ok_selected = false;

  //old offsets
  int old_co2_offset = 0;
  int old_temp_offset = 0;
  int old_humid_offset = 0;

  //hidden values
  const int encoder_rotation_factor = 8;  // 4 encoder signals == +1 to offset
  int hidden_co2_offset = 0;
  int hidden_temp_offset = 0;
  int hidden_humid_offset = 0;

  int clean_offset(int encoder_rotation) {
    return encoder_rotation / encoder_rotation_factor;
  }

public:
  Sensor_Offset() {
    int not_needed;
    spiffs_state_config(&old_co2_offset, &old_temp_offset, &old_humid_offset, &not_needed);
    hidden_co2_offset = encoder_rotation_factor * old_co2_offset;
    hidden_temp_offset = encoder_rotation_factor * old_temp_offset;
    hidden_humid_offset = encoder_rotation_factor * old_humid_offset;
  }

  ~Sensor_Offset() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_type()) {
      case SWITCH:
        switch (input->get_index()) {
          case ENCODER_MENU:
            if (input->get_press_type() == JUST_PRESSED) {
              if (ok_selected) {
                state->save_sensor_offsets(clean_offset(hidden_co2_offset), clean_offset(hidden_temp_offset), clean_offset(hidden_humid_offset));
                state->reload_state_config();
              }
              return DEFAULT_MODE;
            }
        }
        break;
      case ROTATION:
        switch (input->get_index()) {
          case 0:
            hidden_co2_offset += input->get_int();
            break;
          case 1:
            hidden_temp_offset += input->get_int();
            break;
          case 2:
            hidden_humid_offset += input->get_int();
            break;
          case ENCODER_MENU:
            int change = input->get_int() % 2;
            if (change != 0) {
              ok_selected = !ok_selected;
            }
            break;
        }
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(0 + 11, 0);
    display->println("CO2");
    display->setCursor(43 + 11, 0);
    display->println("TMP");
    display->setCursor(86 + 11, 0);
    display->println("HUM");

    display->setTextSize(2);

    display->setCursor(0 + 10, 13);
    display->println(old_co2_offset);
    display->setCursor(43 + 10, 13);
    display->println(old_temp_offset);
    display->setCursor(86 + 10, 13);
    display->println(old_humid_offset);

    display->setCursor(0 + 10, 30);
    display->println(clean_offset(hidden_co2_offset));
    display->setCursor(43 + 10, 30);
    display->println(clean_offset(hidden_temp_offset));
    display->setCursor(86 + 10, 30);
    display->println(clean_offset(hidden_humid_offset));

    display->setCursor(3, 48);
    if (ok_selected) {
      display->println(" NO  [YES]");
    } else {
      display->println("[NO]  YES ");
    }
  }
};


//===================================================================================
/////////////////////////////////////////SET_MIC/////////////////////////////////////
//===================================================================================

//##########################################
class Set_Mic : public Mode {
  //########################################
private:
  const int mic_slider = 5;

  int offset = 0;

public:
  Set_Mic() {}

  ~Set_Mic() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_index()) {
      case ENCODER_MENU:
        switch (input->get_type()) {
          case SWITCH:
            if (input->get_press_type() == JUST_PRESSED) {
              state->get_deej()->offset_slider(mic_slider, offset);
              return DEFAULT_MODE;
            }
            break;
          case ROTATION:
            offset += -10 * input->get_int();
            int value = state->get_deej()->get_slider_pos(mic_slider);
            if (value + offset > DEEJ_MAX_VAL) {
              offset = DEEJ_MAX_VAL - value;
            }
            break;
        }
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(48, 0);
    display->println("MIC");

    display->setCursor(0, 25);
    int value = state->get_deej()->get_slider_pos(mic_slider) + offset;
    double percent = (static_cast<double>(value) / DEEJ_MAX_VAL) * 100;
    display->println(String(percent) + "%");
    display->setCursor(64 + 25, 25);
    display->println(String(value));

    display->setTextSize(1);
    display->setCursor(110, 0);
    display->println("327");

    display->setCursor(17, 50);
    display->println("remember to save");
  }
};
