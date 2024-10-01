

//#############################################
//              panic stuff
//#############################################

//i prefer bugs in my program to cause it to catastrophically fail
//so I don’t overlook any mistakes.
bool panic = false;  //if this is true during execution the program will brake
String panic_msg = "general Panic";



void my_panic() {
  if (ENABLE_PANIC) {
    panic = true;
    Serial.println("Well something went to shit");
  }
}

void my_panic(String msg) {
  if (ENABLE_PANIC) {
    panic = true;
    panic_msg = msg;
    Serial.println("!!! Panic: " + String(msg) + "  !!!");
  }
}

bool check_panic() {
  return ENABLE_PANIC && panic;
}

void do_panic() {
  Serial.println(panic_msg);
  delay(1000);
}

//#############################################
//                other helper
//#############################################

void println(String msg) {
  //Serial.println(msg);
}


void todo(String msg) {
  //Serial.println("todo: " + msg);
}

void do_this(String msg) {
  //my_panic("you have to do this: " + msg);
}

//#############################################
//                Debug print
//#############################################


void setup_print() {
  if (DEBUG) {
    Serial.println("\n ---Debug--- \n");
    _setup_print();
    Serial.println("\n ---Debug end--- \n");
  }
}

void _setup_print() {
  //intended for quick tests
}



//#############################################
//                Debug loop
//#############################################


bool loop_debug() {

  if (check_panic()) {
    do_panic();
    return true;
  }

  return false;
}