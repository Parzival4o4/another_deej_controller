//===================================================================================
///////////////////////////////////////SCREEN_OFF////////////////////////////////////
//===================================================================================

//##########################################
class Screen_off_Mode : public Mode {
  //########################################
public:
  Screen_off_Mode() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_type()) {
      case INACTION:
        {
          input->get_bool();  // handle inactivity so mode is not switched;
          break;
        }
    }
    return NO_CHANGE;  //changing away from this mode will be handled by the default handler
  }

  ModeID idle_task(State* state) override {
    if ((millis() - state->get_last_action()) < 10) {
      //action happened in the last 10 ms (likely movement)
      return DEEJ_MODE;
    }
    return NO_CHANGE;
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    //display is cleared for us
  }
};


//===================================================================================
//////////////////////////////SCREEN_OFF_NO_AUTO_WAKE////////////////////////////////
//===================================================================================

//##########################################
class Screen_off_Mode_no_auto_wake : public Mode {
  //########################################
public:
  Screen_off_Mode_no_auto_wake() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_type()) {
      case INACTION:
        input->get_bool();  // handle inactivity so mode is not switched;
        break;
    }
    return NO_CHANGE;  //changing away from this mode will be handled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    //display is cleared for us
  }
};


//===================================================================================
///////////////////////////////////////SCD4X_MODE////////////////////////////////////
//===================================================================================

//##########################################
class SCD41_Mode : public Mode {
  //########################################
public:
  SCD41_Mode() {
    SCD_user_fast_interval = true;
  }

  ~SCD41_Mode() {
    SCD_user_fast_interval = false;
  }

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    return NO_CHANGE;  //changing away from this mode will be handled by the default handler
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(7, 5);
    display->println("CO2: " + String(state->get_co2()));
    display->setCursor(7, 25);
    display->println("Tmp: " + String(state->get_temp()));
    display->setCursor(7, 45);
    display->println("Hum: " + String(state->get_humid()));
  }
};


//===================================================================================
///////////////////////////////////////DEEJ_MODE/////////////////////////////////////
//===================================================================================

String DJ_labels[] = { "master", "chrome", "current", "discord", "Zoom", "mic" };

//##########################################
class Deej_Mode : public Mode {
  //########################################
private:
  static constexpr int offset = 10;
  bool mic_mute_lock = false;
  bool mic_boosted = false;

  bool enable_button[6];

public:
  Deej_Mode() {
    for (int i = 0; i < 6; i++) {
      enable_button[i] = false;
    }
  }

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    switch (input->get_type()) {
      case SWITCH:
        {
          switch (input->get_index()) {
            case 0:
              {
                // short press => mute Main slider
                switch (input->get_press_type()) {
                  case JUST_PRESSED:
                    enable_button[0] = true;
                    break;
                  case SHORT_PRESS:
                    if (enable_button[0] == true) {
                      state->get_deej()->mute_slider_toggle(0);
                    }
                    break;
                  case LONG_PRESS:
                    if (enable_button[0] == true) {
                      state->get_deej()->save_values();
                      return SAVE_SCREEN;
                    }
                    break;
                }
                break;
              }
            case 1:
              {
                // short press => mute Chrome slider
                switch (input->get_press_type()) {
                  case JUST_PRESSED:
                    enable_button[1] = true;
                    break;
                  case SHORT_PRESS:
                    if (enable_button[1] == true) {
                      state->get_deej()->mute_slider_toggle(1);
                    }
                    break;
                  case LONG_PRESS:
                    if (enable_button[1] == true) {
                      return SCREEN_OFF_NO_AUTO_WAKE;
                    }
                    break;
                }
                break;
              }
            case 2:
              {
                // short press => mute curent slider
                if (input->get_press_type() == JUST_PRESSED) {
                  enable_button[2] = true;
                  state->get_deej()->mute_slider_toggle(2);
                }
                break;
              }
            case 3:
              {
                // Menu encoder
                // short press => change to scd41 mode
                if (input->get_press_type() == JUST_PRESSED) {
                  enable_button[3] = true;
                  return SCD4X_MODE;
                }
                break;
              }
            case 4:
              {
                // short press => Mute Mic
                // long press => Unmute
                SWITCH_PRESS sp = input->get_press_type();
                if (sp == JUST_PRESSED) {
                  enable_button[4] = true;
                  // boost_slider return true if the slider was not previously muted
                  // if that is the case mic_mute_lock will be set to true this will prevent the unmute for this press
                  // if a new button press is performed JUST_PRESSED will be send again
                  // this time the boost_slider() will return false and unlock the unmute
                  mic_mute_lock = state->get_deej()->mute_slider(5);
                } else if (!mic_mute_lock && sp == LONG_PRESS && enable_button[4] == true) {
                  state->get_deej()->unmute_slider(5);
                }
                break;
              }
            case 5:
              {
                // start press => boost Mic
                // stop press => unboost Mic
                SWITCH_PRESS sp = input->get_press_type();
                // no boosting or unboosting of muted sliders (deej class would panic)
                if (sp == JUST_PRESSED && state->get_deej()->get_slider_pos(5) > 0) {
                  enable_button[5] = true;
                  state->get_deej()->boost_slider(5);
                  mic_boosted = true;
                } else if ((sp == RELEASED || sp == SHORT_PRESS) && state->get_deej()->get_slider_pos(5) > 0 && enable_button[5] == true) {
                  if (mic_boosted) {
                    state->get_deej()->unboost_slider(5);
                    mic_boosted = false;
                  }
                }
                break;
              }
          }
          break;
        }
      case ROTATION:
        {
          int index = input->get_index();
          switch (index) {
            case 0:
            case 1:
            case 2:
              {
                state->get_deej()->offset_slider(index, input->get_int() * offset);
                break;
              }
            case ENCODER_MENU:  // 3
              {
                input->get_int();
                return MENU_SELECTOR;
                break;
              }
            case 4:
            case 5:
              {
                state->get_deej()->offset_slider(index - 1, input->get_int() * offset);
              }
          }
          break;
        }
      default:
        {
          //SCD41CO2, SCD41TEMP, SCD41HUMID, MOVEMENT will be handeled by the default handler
        }
    }
    return NO_CHANGE;
  }

  ModeID idle_task(State* state) override {
    if (state->co2_alarm()) {
      return CO2_ALARM;
    }
    return NO_CHANGE;
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    // Draw the loading bars for all six sliders on the OLED display
    int yPos = 0;
    for (int i = 0; i < SLIDER_COUNT; i++) {
      // Calculate the width of the loading bar based on the progress
      int value = state->get_deej()->get_slider_pos(i);
      int barWidth = static_cast<int>((static_cast<double>(value) / 1023) * (SCREEN_WIDTH - 51));
      // Draw the label
      display->setTextSize(1);  // Set text size to 1 (small)
      display->setTextColor(SH110X_WHITE);
      display->setCursor(0, yPos);
      display->print(DJ_labels[i]);
      // Fill the loading bar with the calculated width
      display->fillRect(51, yPos + 2, barWidth, 5, SH110X_WHITE);

      yPos += 10;  // Increase the y position for the next loading bar
    }
  }
};

//===================================================================================
///////////////////////////////////////SAVE_SCREEN///////////////////////////////////
//===================================================================================

//##########################################
class Save_Screen : public Mode {
  //########################################
private:
  static constexpr int input_lock = 200;        //millis // time that the save screen is displayed even if there is user input
  static constexpr int change_back_time = 900;  //millis // time after which it changes back to deej
  long mode_start_time = 0;                     // time since the mode was initiated

public:
  Save_Screen() {
    mode_start_time = millis();
  }

  ~Save_Screen() {}

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    if (millis() - mode_start_time < input_lock) {
      //handle all input
      switch (input->get_type()) {
        case SWITCH:
          input->get_press_type();
          break;
        case ROTATION:
          input->get_int();
          break;
      }
    }
    return NO_CHANGE;
  }

  ModeID idle_task(State* state) override {
    if (millis() - mode_start_time > change_back_time) {
      return DEEJ_MODE;
    } else {
      return NO_CHANGE;
    }
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(3);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(10, 5);
    display->println("Values");
    display->setCursor(20, 35);
    display->println("Saved");
  }
};


//===================================================================================
///////////////////////////////////////CO2_ALARM/////////////////////////////////////
//===================================================================================

//##########################################
class Co2_Alarm : public Mode {
  //########################################
public:
  Co2_Alarm() {}

  ~Co2_Alarm() {}

  void destroy(State* state) override {
    state->mute_co2_alarm();
  }

  ModeID handle_input(const std::unique_ptr<StateChange>& input, State* state) override {
    return NO_CHANGE;  //changing away from this mode will be handeled by the default handler
  }

  ModeID idle_task(State* state) override {
    return NO_CHANGE;
  }

  void display_state(State* state, Adafruit_SH1106G* display) override {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(12, 10);
    display->println("Co2 Alarm");
    display->setCursor(40, 38);
    display->println(String(state->get_co2()));
  }
};
