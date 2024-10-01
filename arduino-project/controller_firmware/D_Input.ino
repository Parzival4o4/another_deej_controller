



//##########################################
class Input_state {
  //########################################
  std::vector<std::unique_ptr<Input_device>> Input_devices;
  std::vector<std::unique_ptr<Input_device>> Sensor_devices;
public:
  Input_state() {
    Input_devices.push_back(make_unique<Encoder_device>());
    Sensor_devices.push_back(make_unique<SCD_41_device>());
    Sensor_devices.push_back(make_unique<Movement_device>());
    delay(100);
  }

  void check_input(std::vector<std::unique_ptr<StateChange>>& vec) {
    for (const auto& id : Input_devices) {
      id->add_input(vec);
    }
  }

  void check_sensors(std::vector<std::unique_ptr<StateChange>>& vec) {
    for (const auto& id : Sensor_devices) {
      id->add_input(vec);
    }
  }
};
