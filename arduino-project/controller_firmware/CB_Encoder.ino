
PCF8575 PCF(0x20, &Wire);

//##########################################
class Encoder_State {
  //########################################
  /**this represents a single encoder**/
protected:
  //pins
  int pin_a;
  int pin_b;
  int pin_s;
  int connected_to;
  //rot state
  int old_rot_pos = 0;
  int rot_dir = 0;
  unsigned long state_time = 0;  // millis();  //time since state change (if a skip is detected this is not updated)
  //switch state
  bool old_switch_state = false;
  bool is_long_press = false;
  int switch_press_start = -1;
  static constexpr int debounce = 5; //ms // time after state change that further state change is ignored;
  long debounce_time = 0; 

  bool read_pin(int16_t pinStates, int pin) {
    switch (connected_to) {
      case 0:
        return !digitalRead(pin);
      case 1:
        return !bitRead(pinStates, pin);
      default:
        my_panic("connection type is not supported check the ENCODER_CONNECTED_TO array in the config");
        return 0;
    }
  }

  int read_rot_pos(int16_t pinStates) {
    bool A = read_pin(pinStates, pin_a);
    bool B = read_pin(pinStates, pin_b);
    if (A && B) {
      return 0;
    } else if (!A && B) {
      return 1;
    } else if (!A && !B) {
      return 2;
    } else if (A && !B) {
      return 3;
    }
    my_panic("reached unreachable code in read_rot_pos()");
    return 0;
  }

  SWITCH_PRESS checkSwitch(bool new_switch_state) {
    //== debounce ==
    if (new_switch_state != old_switch_state) {
      if ((millis() - debounce_time) > debounce){
        //outside of debounce time -> allow state change and begin debounce time;
        debounce_time = millis();
      } else {
        //within debounce time -> dont allow state change 
        if (old_switch_state){
          return PRESSED;
        } else {
          return NOT_PRESSED;
        }
      }
    }
    //== actual check ==
    // switch state == true -> switch pressed
    //switch is and was not pressed -> no state change return no
    if (!new_switch_state && !old_switch_state) {
      // old and new are the same no need to set old = new
      return NOT_PRESSED;
    }
    //switch was just pressed -> start timer, is long false, update state
    if (new_switch_state && !old_switch_state) {
      old_switch_state = new_switch_state;  //switch is pressed
      switch_press_start = millis();
      is_long_press = false;
      return JUST_PRESSED;  //short press is signaled on release
    }
    //switch was released -> check if its short press
    if (!new_switch_state && old_switch_state) {
      old_switch_state = new_switch_state;
      if (!is_long_press) {
        return SHORT_PRESS;
      } else {
        is_long_press = false;  // redundant
        return RELEASED;     // useful for mic boost 
      }
    }
    //switch continues to be pressed -> check short press cut of time
    if (new_switch_state && old_switch_state) {
      // old and new are the same no need to set old = new
      if (!is_long_press && (millis() - switch_press_start > SHORT_PRESS_CUT_OF)) {
        //switch is long pressed
        is_long_press = true;
        return LONG_PRESS;
      } else {
        // no state change
        return PRESSED;
      }
    }
    my_panic("checkSwitch(): a unreachable stat was reached");
    return NOT_PRESSED;
  }

  // helper function for checkRotation
  int rot_by_one(int dir, int new_rot_pos) {
    //direction has to be either 1 or -1

    // debounce dose not prevent doubled inputs and apparently there are still missing inputs
    // but the result might be acceptable
    if (rot_dir != dir && millis() - state_time < ENCODER_ROT_CHANGE_BLOCK_TIME) {
      // new rotation direction was seen and the block time is not over -> ignore change (debounce)
      // we also do not set the old_rot_pos
      return 0;
    }
    old_rot_pos = new_rot_pos;
    state_time = millis();
    rot_dir = dir;
    return dir;
  }

  // helper function for checkRotation
  int skipped_setp(int new_rot_pos) {
    old_rot_pos = new_rot_pos;
    state_time = millis();
    // we skiped a step and dont know the turning direction
    // use most probable rotation
    if (rot_dir > 0) {
      return 2;
    } else {
      return -2;
    }
  }

  int checkRotation(int new_rot_pos) {
    switch (old_rot_pos) {
      // ----- previous postion 0 -----
      case 0:
        switch (new_rot_pos) {
          case 0:  // same position
            return 0;
          case 1:  // right turn +1
            return rot_by_one(1, new_rot_pos);
          case 3:  // left turn -1
            return rot_by_one(-1, new_rot_pos);
          case 2:  // skipped
            return skipped_setp(new_rot_pos);
        }
        break;
      // ----- previous postion 1 -----
      case 1:
        switch (new_rot_pos) {
          case 1:  // same position
            return 0;
          case 2:  // right turn +1
            return rot_by_one(1, new_rot_pos);
          case 0:  // left turn -1
            return rot_by_one(-1, new_rot_pos);
          case 3:  // skipped
            return skipped_setp(new_rot_pos);
        }
        break;
      // ----- previous postion 2 -----
      case 2:
        switch (new_rot_pos) {
          case 2:  // same position
            return 0;
          case 3:  // right turn +1
            return rot_by_one(1, new_rot_pos);
          case 1:  // left turn -1
            return rot_by_one(-1, new_rot_pos);
          case 0:  // skipped
            return skipped_setp(new_rot_pos);
        }
        break;
      // ----- previous postion 3 -----
      case 3:
        switch (new_rot_pos) {
          case 3:  // same position
            return 0;
          case 0:  // right turn +1
            return rot_by_one(1, new_rot_pos);
          case 2:  // left turn -1
            return rot_by_one(-1, new_rot_pos);
          case 1:  // skipped
            return skipped_setp(new_rot_pos);
        }
        break;
    }
    my_panic("checkRotation(): a unreachable state was reached\n old_rotpos " + String(old_rot_pos) + " new_rotpos " + String(new_rot_pos));
    return 0;
  }

  void setup(int a, int b, int s, int c, int16_t pinStates) {
    // call this function in the childs constructor
    if (c == 0) {
      // encoder is connected to the esp directly -> setup pins
      pinMode(pin_a, INPUT_PULLUP);
      pinMode(pin_b, INPUT_PULLUP);
      pinMode(pin_s, INPUT_PULLUP);
    }
    get_button_StatChange(pinStates);
    get_rot_StatChange(pinStates);
  }

public:
  Encoder_State(int a, int b, int s, int c)
    : pin_a(a), pin_b(b), pin_s(s), connected_to(c) {
  }

  ~Encoder_State() {}

  SWITCH_PRESS get_button_StatChange(int16_t pinStates) {
    return checkSwitch(read_pin(pinStates, pin_s));
  }

  virtual int get_rot_StatChange(int16_t pinStates);

  virtual bool need_prio();
};


//##########################################
class Smooth_Encoder : public Encoder_State {
  //########################################
  // this type favors higher resolution per rotation over 100% accuracy
  // due to non perfect debounce it is not possible to guarantee that no steps are missed or duplicated
public:
  Smooth_Encoder(int a, int b, int s, int c, int16_t pinStates)
    : Encoder_State(a, b, s, c) {
    setup(a, b, s, c, pinStates);
  }

  int get_rot_StatChange(int16_t pinStates) override {
    return checkRotation(read_rot_pos(pinStates));
  }

  bool need_prio() override {
    return false;
  }
};


//##########################################
class stepped_Encoder : public Encoder_State {
  //########################################
  // In one step, the encoder goes through all 4 states.
  // this mode insures that sent signals are sent in sync with the encoders valleys
  // lower resolution in one rotation but better accuracy
private:
  int state_count = 0;
  bool prio_needed = false;

public:
  stepped_Encoder(int a, int b, int s, int c, int16_t pinStates)
    : Encoder_State(a, b, s, c) {
    setup(a, b, s, c, pinStates);
  }

  int get_rot_StatChange(int16_t pinStates) override {
    int rrp = read_rot_pos(pinStates);
    int rot = checkRotation(rrp);
    if (rot == 0) {
      prio_needed = false;
      return 0;
    }
    state_count += rot;
    prio_needed = true;
    if (old_rot_pos == 2) {
      int local_sc = state_count;
      state_count = 0;
      if (local_sc > 2) {
        return 1;
      } else if (-2 > local_sc) {
        return -1;
      }
    }
    return 0;
  }

  bool need_prio() override {
    //panic("you should not check if a smooth encoder needs prio with this function, instead check if the get_rot_StatChange() function returns something other than 0");
    return prio_needed;
  }
};




//##########################################
class Encoder_device : public Input_device {
  //########################################
  /**this represents a set of encoders**/
private:
  // -- PFC --
  int16_t pinStates = 0b1111111111111111;
  // -- encoder --
  std::unique_ptr<Encoder_State> encoder_state_arr[ENCODER_COUNT];
  long ignore_rotation = millis();
  long prio_time = millis();
  int prio_index = -1;

  void pull_PFC_pin_state() {
    pinStates = PCF.read16();
  }

  void prio(int i) {
    prio_time = millis();
    prio_index = i;
  }

  bool prio_exists() {
    return (millis() - prio_time) < PRIO_TIME;
  }

  bool check_encoder(int index, std::vector<std::unique_ptr<StateChange>>& vec) {
    bool added_something = false;
    // --- buttons ---
    SWITCH_PRESS press_state = encoder_state_arr[index]->get_button_StatChange(pinStates);
    if (press_state != NOT_PRESSED) {
      //this means the button is pressed and all rotation should be locked
      ignore_rotation = millis();
      vec.push_back(make_unique<ButtonChange>(press_state, index));
      added_something = true;
    }
    // --- rotation ---
    if ((millis() - ignore_rotation) < IGNORE_ROTATION_TIME) {
      return added_something;
    }
    // Get rotation state change
    int rotation = encoder_state_arr[index]->get_rot_StatChange(pinStates);
    if (rotation != 0) {
      // this encoder has movement and is given priority
      prio(index);
      vec.push_back(make_unique<RotChange>(rotation, index));
      added_something = true;
    } else if (encoder_state_arr[index]->need_prio()) {
      prio(index);
    }
    return added_something;
  }

public:
  Encoder_device() {
    // ----- setup encoder objects -----
    int Pin_A[] = ENCODER_PIN_A;
    int Pin_B[] = ENCODER_PIN_B;
    int Pin_S[] = ENCODER_PIN_S;
    int connected_to[] = ENCODER_CONNECTED_TO;
    pull_PFC_pin_state();
    for (int i = 0; i < ENCODER_COUNT; i++) {
      if (i == ENCODER_MENU) {
        encoder_state_arr[i] = make_unique<stepped_Encoder>(Pin_A[i], Pin_B[i], Pin_S[i], connected_to[i], pinStates);
      } else {
        encoder_state_arr[i] = make_unique<Smooth_Encoder>(Pin_A[i], Pin_B[i], Pin_S[i], connected_to[i], pinStates);
      }
    }
    delay(100);
  }

  bool add_input(std::vector<std::unique_ptr<StateChange>>& vec) override {
    // Get the latest state of the IO Pins
    bool added_something = false;
    pull_PFC_pin_state();
    if (false && prio_exists()) {
      todo("remove prio system");
      
      if (check_encoder(prio_index, vec)) {
        added_something = true;
      }
      return added_something;
    }
    // Check all encoders
    for (int i = 0; i < ENCODER_COUNT; i++) {
      // rechecking prio is unnecessary due to physical constrains the rotation starts slow and we can afford checking all encoders in the first cycle of the input
      if (check_encoder(i, vec)) {
        added_something = true;
      }
    }
    return added_something;
  }
};
