


//##########################################
class Movement_device : public Input_device {
  //########################################
private:
  long time_since_measurement = millis();

  bool movement = false;

  bool pull_measurement() {
    if ((millis() - time_since_measurement) > Movement_interval) {
      time_since_measurement = millis();
      movement = digitalRead(MOTION_PIN);
      return true;
    }
    return false;
  }

public:
  Movement_device() {
    pinMode(MOTION_PIN, INPUT_PULLDOWN);
  }

  ~Movement_device() {}

  bool add_input(std::vector<std::unique_ptr<StateChange>>& vec) override {
    if (pull_measurement() && movement) {
      vec.push_back(make_unique<MovementChange>(movement));
      return true;
    }
    return false;
  }
};