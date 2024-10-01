

//##########################################
class Deej {
  //########################################
  // the deej desktop application expects slider values between 0 and 1023
  // there is also a deadzone on either side (defined in DJ_DEAD_ZONE)
private:

  int deadzone = 50; // percent_to_intern(DEAD_ZONE_PERCENT);

  int next_index = 0;
  long wait_start = 0; //millis();

  int slider_positions[SLIDER_COUNT];
  int slider_positions_cache[SLIDER_COUNT];

  int percent_to_intern(int value) {
    return map_value(value, 100, DEEJ_MAX_VAL);
  }

  int get_clean_value(int sliderID) {
    // the internal slider_positions array can exceed the rang of 0-DEEJ_MAX_VAL;
    // this dead zone has to be cleaned up for sending
    int value = slider_positions[sliderID];
    if (value < 0) {
      value = 0;
    } else if (value > DEEJ_MAX_VAL) {
      value = DEEJ_MAX_VAL;
    }
    return value;
  }

  bool valid_slider_ID (int sliderID) {
    if (sliderID < 0 || sliderID >= SLIDER_COUNT) {
      my_panic("called a deej function with a invalid sliderID: " + String(sliderID));
      return false;
    }
    return true;
  }

public:
  

  Deej() {
    int def_percent[] = DEFAULT_PERCENT;
    if (!def_percent) {
      my_panic("deej class did not get valid def_percent values from spiffs");
    }
    for (int i = 0; i < SLIDER_COUNT; i++) {
      slider_positions[i] = static_cast<int>((static_cast<double>(def_percent[i]) / 100) * 1023);
      slider_positions_cache[i] = -deadzone;
    }
  }

  ~Deej(){};

  void reload_values(){
    std::unique_ptr<int[]> def_percent = spiffs_get_default();
    
    if (!def_percent) {
      my_panic("deej class did not get valid def_percent values from spiffs");
    }
    for (int i = 0; i < SLIDER_COUNT; i++) {
      slider_positions[i] = static_cast<int>((static_cast<double>(def_percent[i]) / 100) * 1023);
      slider_positions_cache[i] = -deadzone;
    }
  }

  void save_values() {
    std::unique_ptr<int[]> slider_positions(new int[ENCODER_COUNT]);
    for (int i = 0; i < ENCODER_COUNT; i++){
      slider_positions[i] = static_cast<int>((static_cast<double>(get_clean_value(i)) /1023 ) * 100);
    }
    todo("it is possible to save the mic positin at 0 wich is bad");
    //slider_positions[5] = 43;
    spiffs_save_default(ENCODER_COUNT, std::move(slider_positions));
  }

  int get_slider_pos(int sliderID) {
    if (!valid_slider_ID(sliderID)) { return 0;}
    return get_clean_value(sliderID);
  }

  void send_sliders() {
    // Serial.println can take a loot of time
    // from my testing it appears that there is a buffer
    // if the buffer is empty println writes its value in the buffer and return (this is relatively fast)
    // if the buffer is full println waits until it can send its message (this can take several milli seconds)
    // to avoid this the message is send in small chunks and there is a wait time of 60 millis 
    if (((millis() - wait_start) > DELAY_AFTER_SENDING) && DEEJ_PRINT) {
      Serial.print(get_clean_value(next_index));
      if (next_index < SLIDER_COUNT - 1) {
        Serial.print("|");
        next_index = next_index + 1;
        wait_start = millis();
      } else {
        Serial.println("");
        wait_start = millis();
        next_index = 0;
      }
    } 
  }

  bool mute_slider(int sliderID) {
    if (!valid_slider_ID(sliderID)) { return false;}
    if (slider_positions[sliderID] > 0) {
      // was not muted -> need to mute
      slider_positions_cache[sliderID] = slider_positions[sliderID];
      slider_positions[sliderID] = -deadzone;
      return true;
    }
    return false;
  }

  bool unmute_slider(int sliderID){
    if (!valid_slider_ID(sliderID)) { return false;}
    if (slider_positions[sliderID] <= 0){
      // was muted -> unmute
      slider_positions[sliderID] = slider_positions_cache[sliderID];
      slider_positions_cache[sliderID] = -deadzone; //cleanup cache // not actually needed
      return true;
    }
    return false;
  }

  void mute_slider_toggle(int sliderID) {
    if (!valid_slider_ID(sliderID)) { return;}
    if (slider_positions[sliderID] > 0) {
      // was not muted -> need to mute
      slider_positions_cache[sliderID] = slider_positions[sliderID];
      slider_positions[sliderID] = -deadzone;
    } else {
      // was muted -> unmute
      slider_positions[sliderID] = slider_positions_cache[sliderID];
      slider_positions_cache[sliderID] = -deadzone; //cleanup cache // not actually needed
    }
  }

  void boost_slider(int sliderID) {
    if (!valid_slider_ID(sliderID)) { return;}
    if (slider_positions[sliderID] <= 0) {
      //slider is muted (no boosting)
      my_panic("attempted to boost muted slider ID: " + String(sliderID));
    }
    // check if value is maxed
    if  (slider_positions[sliderID] >= percent_to_intern(MIC_BOOST_PERCENT)) {
      // slider value is already bigger or just as big as MIC_BOOST_PERCENT 
      return;
    }
    // swap old value to cache
    slider_positions_cache[sliderID] = slider_positions[sliderID];
    // set slider pos to 100%
    slider_positions[sliderID] = percent_to_intern(MIC_BOOST_PERCENT);
  }

  void unboost_slider(int sliderID) {
    if (!valid_slider_ID(sliderID)) { return;}
    if (slider_positions[sliderID] < percent_to_intern(MIC_BOOST_PERCENT)){
      my_panic("called unboost on a slider that has not been boosted, SliderID: " + String(sliderID) + " old value: " + String(slider_positions[sliderID]));
    }
    slider_positions[sliderID] = slider_positions_cache[sliderID];
    slider_positions_cache[sliderID] = -deadzone; //cleanup cache // not actually needed
  }


  void offset_slider(int sliderID, int offset) {
    if (!valid_slider_ID(sliderID)) { return;}
    //add offset
    slider_positions[sliderID] = slider_positions[sliderID] + offset;
    //cleanup slider position
    if (slider_positions[sliderID] < -deadzone) {
      slider_positions[sliderID] = -deadzone;
    } else if (slider_positions[sliderID] > DEEJ_MAX_VAL + deadzone) {
      slider_positions[sliderID] = DEEJ_MAX_VAL + deadzone;
    }
  }
};
