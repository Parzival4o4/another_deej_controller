

enum ModeID {
  NO_CHANGE,
  DEFAULT_MODE,

  SCREEN_OFF,
  SCREEN_OFF_NO_AUTO_WAKE,
  DEEJ_MODE,
  SCD4X_MODE,
  SAVE_SCREEN,
  CO2_ALARM,

  
  MENU_SELECTOR,
  DELETE_ALL_FILES,
  SPIFFS_FILES,
  FORMAT_FILE,
  SENSOR_OFFSET,
  SET_MIC,
  C02_ALARM_VAL,
};



//##########################################
class Mode {
  //########################################


public:
  ~Mode() {}

  // this function is called before the object is destroyed
  virtual void destroy(State* state) {
    // Do nothing 
  }

  // handle input from the user (no sensor data)
  virtual ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state);

  // if the mode wants to tasks that need to happen without user input this function can be implemented
  virtual ModeID idle_task(State* state) {
    return NO_CHANGE;
  }

  // display
  virtual void display_state(State* state, Adafruit_SH1106G* display);
};


//##########################################
class Default_Handler {
  //########################################
private:
  int rotation = 0;
  int rotation_start = 0;

  ModeID default_mode = DEFAULT_MODE;

  ModeID handle_button(SWITCH_PRESS press_state) {
    if (press_state == JUST_PRESSED) {
      return default_mode;
    }
    return NO_CHANGE;
  }

  ModeID handle_rotation(int index, int rot) {
    if (millis() - rotation_start > 100) {
      rotation = 0;
      rotation_start = millis();
    }
    rotation += rot;
    if (rotation > 4 || -4 > rotation) {
      return default_mode;
    } else if (index == ENCODER_MENU && rot != 0) {
      return default_mode;
    }
    return NO_CHANGE;
  }

public:
  Default_Handler() {
    rotation_start = millis();
  }

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) {
    switch (input->get_type()) {
      case SCD41CO2:
        state->set_co2(input->get_int());  //Intentially dont check if the input has been handled since it is not supposed to be handled by the mode (program will panic, which fulfills the fail fast requirement)
        break;
      case SCD41TEMP:
        state->set_temp(input->get_int());
        break;
      case SCD41HUMID:
        state->set_humid(input->get_int());
        break;
      case MOVEMENT:
        if (input->get_bool()) {
          state->action();
        } else {
          my_panic("Input was not prefiltered (Movement returned false)");
        }
        break;
      case ROTATION:
        {
          if (!input->is_handled()) {
            return handle_rotation(input->get_index(), input->get_int());
          }
          break;
        }
      case SWITCH:
        {
          if (!input->is_handled()) {
            return handle_button(input->get_press_type());
          }
          break;
        }
      case INACTION:
        {
          if (!input->is_handled()) {  // the mode can disable turn off screen by handling the INACTION signal
            return SCREEN_OFF;
          }
          break;
        }
    }
    return NO_CHANGE;
  }
};
