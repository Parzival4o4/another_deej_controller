
class State {
private:
  // it is unavoidable that the modes need to hold state
  // this state is held in here
  // this class dose not hold state for the input devices, this creates a clearer informaiton flow
  // Input_device -> Input_state ----> handler -> state --> Display
  //                                      ^-------'   '---> Serial

  //----- semi const ------
  Deej deej_obj;

  int co2_offset = 0;
  int temp_offset = 0;
  int humid_offset = 0;

  int co2_alarm_value = CO2_ALARM_LEVEL;


  //----- variables ------
  

  int co2 = -1;
  int temp = -1;
  int humid = -1;

  long last_action = 0;

  long co2_mute_time = -CO2_ALARM_MUTE_TIME * 1000 * 60;

public:
  State() {}

  ~State() {}

  //==== getter ====

  Deej* get_deej() {
    return &deej_obj;
  }

  int get_co2() {
    return co2;
  }

  int get_temp() {
    return temp;
  }

  int get_humid() {
    return humid;
  }

  // returns the time since the last action (action is either user input or movement)
  long get_last_action() {
    return last_action;
  }

  //==== setter ====

  void set_co2(int c) {
    co2 = c + co2_offset ;
  }

  void set_temp(int t) {
    temp = t + temp_offset;
  }

  void set_humid(int h) {
    humid = h + humid_offset;
  }

  //==== other ====

  void reload_state_config(){
    spiffs_state_config(&co2_offset, &temp_offset, &humid_offset, &co2_alarm_value);
  }

  void save_sensor_offsets(int new_co2_offset, int new_tmp_offset, int new_hum_offset){
    spiffs_save_state_config(new_co2_offset, new_tmp_offset, new_hum_offset, co2_alarm_value);
  }

  void save_alarm_val(int new_co2_alarm_value){
    spiffs_save_state_config(co2_offset, temp_offset, humid_offset, new_co2_alarm_value);
  }

  void action() {
    last_action = millis();
  }

  void mute_co2_alarm() {
    co2_mute_time = millis();
  }

  bool co2_alarm() {
    return (co2 > co2_alarm_value) && (millis() - co2_mute_time) > (CO2_ALARM_MUTE_TIME * 1000 * 60);
  }


};
