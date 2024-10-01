
// if a action needs to be performed continuously while the button is held listen for PRESSED
// if a action is to be performed if the button is kicked listen for SHORT_PRESS
// if a action is to be performed only if the button is long pressed listen for LONG_PRESS
// dont hold switch state anywhere other than in the encoder class
enum SWITCH_PRESS {
  NOT_PRESSED,  // the button is not pressed
  JUST_PRESSED, // button just got pressed
  PRESSED,      // the button is pressed
  SHORT_PRESS,  // Short press signal is given
  LONG_PRESS,   // Long press signal is given
  RELEASED      //  the SHORT_PRESS signal is also a release signal !!!! check for both !!!!!!
};

enum StateChangeType {
  ROTATION,
  SWITCH,
  SCD41CO2,
  SCD41TEMP,
  SCD41HUMID,
  MOVEMENT,
  INACTION // will be added in the main loop if state shows that there was no activity for a long time
};


//-----------------------------------------------------------------------------------------

//#################################################################
//#################################################################
class StateChange {
  //###############################################################
  //###############################################################
protected:
  StateChangeType type;

  bool value_handled = false;

  int index = 0;  // used for to identify origin encoder

  void handle() {
    if (value_handled) {
      my_panic("tried to access state change value twice");
    }
    value_handled = true;
  }

public:
  virtual ~StateChange() {}

  StateChangeType get_type() {
    return type;
  }

  int get_index() {
    return index;
  }

  virtual int get_int() {
    my_panic("called get_int() on a StateChange type that dose not support it. type: " + String(type));
    return 0;
  }
  virtual bool get_bool() {
    my_panic("called get_bool() on a StateChange type that dose not support it. type: " + String(type));
    return 0;
  }
  virtual SWITCH_PRESS get_press_type() {
    my_panic("called get_press_type() on a StateChange type that dose not support it. type: " + String(type));
    return NOT_PRESSED;
  }

  virtual bool is_handled() const {
    return value_handled;
  }
};


//-----------------------------------------------------------------------------------------


//#################################################################
class IntChange : public StateChange {
  //###############################################################
protected:
  int value;

public:
  int get_int() override {
    handle();
    return value;  // Return void pointer to the bool value
  }
};




//#################################################################
class BoolChange : public StateChange {
  //###############################################################
protected:
  bool value;

public:
  bool get_bool() override {
    handle();
    return value;  // Return void pointer to the bool value
  }
};

//#################################################################
class Press_TypeChange : public StateChange {
  //###############################################################
protected:
  SWITCH_PRESS value;

public:
  SWITCH_PRESS get_press_type() override {
    handle();
    return value;  // Return void pointer to the bool value
  }
};


//-----------------------------------------------------------------------------------------


// ----- Encoder ----
class RotChange : public IntChange {
public:
  RotChange(int v, int i) {
    value = v;
    index = i;
    type = ROTATION;
  }
};

class ButtonChange : public Press_TypeChange {
public:
  ButtonChange(SWITCH_PRESS v, int i) {
    value = v;
    index = i;
    type = SWITCH;
  }
};

// ----- SCD ----
class Co2Change : public IntChange {
public:
  Co2Change(int v) {
    value = v;
    type = SCD41CO2;
  }
};

class TempChange : public IntChange {
public:
  TempChange(int v) {
    value = v;
    type = SCD41TEMP;
  }
};

class HumidChange : public IntChange {
public:
  HumidChange(int v) {
    value = v;
    type = SCD41HUMID;
  }
};

// ----- movement ----
class MovementChange : public BoolChange {
public:
  MovementChange(bool v) {
    value = v;
    type = MOVEMENT;
  }
};

// ----- inactivity -----
class InactiveChange : public BoolChange {
public:
  InactiveChange(bool v) {
    value = v;
    type = INACTION;
  }
};