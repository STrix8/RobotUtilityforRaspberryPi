#pragma once

#include "ScrpMaster.hpp"

namespace rbutil {
  extern int MaxMotorPower;
  class Motor {
  public :
    Motor();
    Motor(unsigned char, unsigned char, double, ScrpMaster&, short maxPower = MaxMotorPower);
    Motor(MotorDataFormat, ScrpMaster&, short maxPower = MaxMotorPower);
    short changeMaxPower(short);
    virtual bool spin(short, bool asyncFlag = false);
    virtual ~Motor();
  protected :
    ScrpMaster* sm;
    unsigned char id;
    unsigned char mNum;
    double magni;
    short maxPower;
    bool initFlag;
  };
  int loadMotorSetting(char*, MotorDataFormat*, int);
}
