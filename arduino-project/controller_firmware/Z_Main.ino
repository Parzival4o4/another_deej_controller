

//######################################################################
//##############        _      ____   ____  _____         ##############
//##############       | |    / __ \ / __ \|  __ \        ##############
//##############       | |   | |  | | |  | | |__) |       ##############
//##############       | |   | |  | | |  | |  ___/        ##############
//##############       | |___| |__| | |__| | |            ##############
//##############       |______\____/ \____/|_|            ##############
//##############                                          ##############
//######################################################################


long display_time = 0;

long my_time = 0;
void loop() {
  //long time2 = micros() - my_time;
  //my_time = micros();
  //println("time = " + String(time2) + " free heap: " + String(ESP.getFreeHeap()));
  //println(String(time2));


  // ====== Check panic ======
  if (loop_debug()) { return; }

  // ====== sensors ======
  std::vector<std::unique_ptr<StateChange>> sensors;
  I_state.check_sensors(sensors);
  for (const auto& sc : sensors) {
    mm.handle_sensors(sc, &state);
  }

  // ==== user input ====
  std::vector<std::unique_ptr<StateChange>> inputs;
  I_state.check_input(inputs);
  if (!inputs.empty()){
    state.action(); 
  } else if (millis() - state.get_last_action() > (SCREEN_TIMEOUT * 60000)) {
    inputs.push_back(make_unique<InactiveChange>(true));
  }
  // call mode handler
  // call default handler
  for (const auto& sc : inputs) {
    mm.handle_input(sc, &state);
  }

  // ==== idle tasks ====
  mm.idle_task(&state);
  
  // === display/send ===
  state.get_deej()->send_sliders();
  mm.display_state(&state);


  //println(String(micros() - my_time));
}
