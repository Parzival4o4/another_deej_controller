//-----------------------------------------------------------------------------------
bool spiffs_format() {
  //---------------------------------------------------------------------------------
  println("format");
  return SPIFFS.format();
}

//-----------------------------------------------------------------------------------
void spiffs_delete_files() {
  //---------------------------------------------------------------------------------
  File file = SPIFFS.open(FILE_NAME_SLIDER_VALUES, "r");
  println("Delete FILE_NAME_SLIDER_VALUES: " + String(SPIFFS.remove(FILE_NAME_SLIDER_VALUES)));
  file.close();

  file = SPIFFS.open(FILE_NAME_STATE_CONFIG, "r");
  println("Delete FILE_NAME_STATE_CONFIG: " + String(SPIFFS.remove(FILE_NAME_STATE_CONFIG)));
  file.close();
}

//-----------------------------------------------------------------------------------
String spiffs_read_All_Files() {
  //---------------------------------------------------------------------------------
  String filenames = "";

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  // List all files in the directory
  while (file) {
    filenames += file.name();
    filenames += "\n"; // Add a newline character for separation
    file = root.openNextFile();
  }
  root.close();

  return filenames;
}

//-----------------------------------------------------------------------------------
bool fille_is_present(String file_name){
  //---------------------------------------------------------------------------------
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  String file_name_clean = file_name.substring(1);
  
  bool file_is_present = false;

  while (file) {
    if (file_name_clean == file.name()){
      file_is_present = true;
    }
    file = root.openNextFile();
  }
  root.close();
  return file_is_present;
}


//===================================================================================
////////////////////////////////////state_config/////////////////////////////////////
//===================================================================================

//-----------------------------------------------------------------------------------
void spiffs_save_state_config(int co2_offset, int tmp_offset, int hum_offset, int co2_alarm_value) {
  //---------------------------------------------------------------------------------
  File file = SPIFFS.open(FILE_NAME_STATE_CONFIG, "w");
  if (!file) {
    my_panic("Failed to open file for writing: FILE_NAME_STATE_CONFIG");
    return;
  }
  file.write((uint8_t*)&co2_offset, sizeof(co2_offset));
  file.write((uint8_t*)&tmp_offset, sizeof(tmp_offset));
  file.write((uint8_t*)&hum_offset, sizeof(hum_offset));
  file.write((uint8_t*)&co2_alarm_value, sizeof(co2_alarm_value));
  file.close();
}

//-----------------------------------------------------------------------------------
void spiffs_state_config(int* co2_offset, int* tmp_offset, int* hum_offset, int* co2_alarm_value) {
  //---------------------------------------------------------------------------------
  if (!fille_is_present(FILE_NAME_STATE_CONFIG)) {
    println("file is not event there FILE_NAME_STATE_CONFIG");
    *co2_offset = 0;
    *tmp_offset = 0;
    *hum_offset = 0;
    *co2_alarm_value = CO2_ALARM_LEVEL;
    return;
  }

  File file = SPIFFS.open(FILE_NAME_STATE_CONFIG, "r");
  if (!file) {
    // file dose not exist yet so we use the values from the config section
    println("failed to load");
    *co2_offset = 0;
    *tmp_offset = 0;
    *hum_offset = 0;
    *co2_alarm_value = CO2_ALARM_LEVEL;
    return;
  }
  file.read((uint8_t*)co2_offset, sizeof(int));
  file.read((uint8_t*)tmp_offset, sizeof(int));
  file.read((uint8_t*)hum_offset, sizeof(int));
  file.read((uint8_t*)co2_alarm_value, sizeof(int));
  file.close();
}

//===================================================================================
/////////////////////////////////////slider values///////////////////////////////////
//===================================================================================


//-----------------------------------------------------------------------------------
void spiffs_save_default(int slider_count, std::unique_ptr<int[]> slider_positions) {
  //---------------------------------------------------------------------------------

  File file = SPIFFS.open(FILE_NAME_SLIDER_VALUES, "w");
  if (!file) {
    my_panic("Failed to open file for writing: default sliders");
    return;
  }

  // Write array size as the first element
  file.write((uint8_t*)&slider_count, sizeof(slider_count));

  // Write array elements
  file.write((uint8_t*)slider_positions.get(), sizeof(int) * slider_count);
  file.close();
}


//-----------------------------------------------------------------------------------
std::unique_ptr<int[]> spiffs_get_default() {
  //---------------------------------------------------------------------------------
  if (!fille_is_present(FILE_NAME_SLIDER_VALUES)) {
    println("file is not event there FILE_NAME_SLIDER_VALUES");
    std::unique_ptr<int[]> default_percent_from_config(new int[ENCODER_COUNT] DEFAULT_PERCENT);
    return default_percent_from_config;
  }

  File file = SPIFFS.open(FILE_NAME_SLIDER_VALUES, FILE_READ);
  if (!file) {
    // file dose not exist yet so we use the values from the config section
    std::unique_ptr<int[]> default_percent_from_config(new int[ENCODER_COUNT] DEFAULT_PERCENT);
    return default_percent_from_config;
  }
  // Read array size
  int size;
  file.read((uint8_t*)&size, sizeof(size));
  // Allocate memory for the array
  std::unique_ptr<int[]> default_positions(new int[size]);
  // Read array elements
  file.read((uint8_t*)default_positions.get(), sizeof(int) * size);
  file.close();
  // Return the default values
  
  return default_positions;
}


//-----------------------------------------------------------------------------------
void printFileContents(const char* filename) {
  //---------------------------------------------------------------------------------
    File file = SPIFFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    while (file.available()) {
        Serial.print("0x");
        Serial.print(file.read(), HEX);
        Serial.print(" ");
    }

    file.close();
}
