
SensirionI2CScd4x scd4x;

//##########################################
class SCD_41_device : public Input_device {
  //########################################
private:
  long time_since_measurement = millis();

  uint16_t last_co2 = -1;
  float last_temperature = -1;
  float last_humidity = -1;

  bool pull_measurement() {
    if (
      (SCD_user_fast_interval && (millis() - time_since_measurement) > SCD_interval_fast)
      || (millis() - time_since_measurement) > SCD_interval_slow) {
      time_since_measurement = millis();
      scd4x.readMeasurement(last_co2, last_temperature, last_humidity);
      if (last_co2 == 65535) {
        // sensor has not made the first measurement 
        last_co2 = 0;
      }
      return true;
    }
    return false;
  }

public:
  SCD_41_device() {
    scd4x.begin(Wire);
    scd4x.stopPeriodicMeasurement();
    scd4x.startPeriodicMeasurement();
    delay(1000);
    scd4x.readMeasurement(last_co2, last_temperature, last_humidity);
  }
  ~SCD_41_device() {}

  bool add_input(std::vector<std::unique_ptr<StateChange>>& vec) override {
    if (pull_measurement()) {
      vec.push_back(make_unique<Co2Change>(int(last_co2)));
      vec.push_back(make_unique<TempChange>(last_temperature));
      vec.push_back(make_unique<HumidChange>(last_humidity));
      return true;
    }
    return false;
  }
};