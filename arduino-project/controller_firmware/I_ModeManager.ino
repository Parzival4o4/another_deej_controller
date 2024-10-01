
Adafruit_SH1106G OLED_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

//##########################################
class ModeManager {
  //########################################
private:
  Mode* current_mode;
  Default_Handler dh;


  long frame_start_time = 0;
  int Time_between_frames = 1000 / FPS;

  void switch_mode(ModeID mode_id) {
    delete current_mode;
    switch (mode_id) {
      case DEFAULT_MODE: // Default is a synonym for deej
      case DEEJ_MODE:
        current_mode = new Deej_Mode();
        break;
      case SCREEN_OFF:
        current_mode = new Screen_off_Mode();
        break;
      case SCD4X_MODE:
        current_mode = new SCD41_Mode();
        break;
      case MENU_SELECTOR:
        current_mode = new Menu_Mode();
        break;
      case SAVE_SCREEN:
        current_mode = new Save_Screen();
        break;
      case CO2_ALARM:
        current_mode = new Co2_Alarm();
        break;  
      case FORMAT_FILE:
        current_mode = new Format_File();
        break;
      case SENSOR_OFFSET:
        current_mode = new Sensor_Offset();
        break;
      case SET_MIC:
        current_mode = new Set_Mic();
        break;  
      case SCREEN_OFF_NO_AUTO_WAKE:
        current_mode = new Screen_off_Mode_no_auto_wake();
        break; 
      case C02_ALARM_VAL:
        current_mode = new C02_Alarm_Val();
        break;
      case DELETE_ALL_FILES:
        current_mode = new Delete_All_Files();
        break;
      case SPIFFS_FILES:
        current_mode = new Spiffs_Files();
        break;
      default:
        my_panic("attemted to chang to a unknown mode: " + String(mode_id));
        break;
    }
  }

  void change_mode(ModeID mode_id, State* state){
    if (mode_id == NO_CHANGE) {
      return;
    }
    current_mode->destroy(state);
    switch_mode(mode_id);
  }

public:
  ModeManager(ModeID initial_mode) {
    switch_mode(initial_mode);
    frame_start_time = millis();
  }

  void handle_sensors(const std::unique_ptr<StateChange>& input, State* state) {
    ModeID next_mode = dh.handle_input(input, state);
    change_mode(next_mode, state);
  }

  void handle_input(const std::unique_ptr<StateChange>& input, State* state) {
    ModeID next_mode = current_mode->handle_input(input, state);
    change_mode(next_mode, state);
    next_mode = dh.handle_input(input, state);
    change_mode(next_mode, state);
  }

  virtual void idle_task(State* state) {
    ModeID next_mode = current_mode->idle_task(state);
    change_mode(next_mode, state);
  }

  void display_state(State* state) {
    if (millis() - frame_start_time > Time_between_frames) {
      OLED_display.clearDisplay();
      current_mode->display_state(state, &OLED_display);
      frame_start_time = millis();
      xTaskCreatePinnedToCore([](void* parameter) {
        OLED_display.display();  // Call display function
        vTaskDelete(NULL);       // Delete the task after execution
      },
                              "Display Task", 10000, NULL, 1, NULL, 1);  // Create display task
    }
  }
};