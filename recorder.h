#ifndef RECORDER_H
#define RECORDER_H

#include "Arduino.h"

class Recorder
{
  public:
    Recorder();

    void begin();

    void loop();

    bool send(char* message);
    
  private:

    void heartbeat();
  
};

#endif
