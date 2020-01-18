#ifndef CRAFT_SRC_CRAFT_INTERFACES_SIGNINTERFACE_H_
#define CRAFT_SRC_CRAFT_INTERFACES_SIGNINTERFACE_H_

#include "craft/interfaces/Interface.h"

struct SignInterface : public Interface {
  public:
    static std::string name() { return "Sign"; }
    std::string get_name() const override { return name(); }


  private:
};

#endif // CRAFT_SRC_CRAFT_INTERFACES_SIGNINTERFACE_H_
