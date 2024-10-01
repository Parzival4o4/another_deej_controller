// abstract class, represents a input device
//#################################################################
class Input_device {
  //###############################################################
public:
  virtual ~Input_device() {}

  virtual bool add_input(std::vector<std::unique_ptr<StateChange>>& vec);
  // return true if you added something
};