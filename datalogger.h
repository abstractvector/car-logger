#ifndef DATALOGGER_H
#define DATALOGGER_H

#include "Arduino.h"

class DataLogger
{
  public:
    DataLogger();

    void setQueue(QueueHandle_t queue);
    
    void setGpsDateTime(long date, long time);
    String getDateTime();
    
    void log(uint16_t pid, float value);
    void log(uint16_t pid, int value);
    void log(uint16_t pid, unsigned int value);
    void logFloat(uint16_t pid, float value);

    void write(const char* buf, byte len);

    void loop();

  private:
    String _date;
    String _time;
    unsigned long _lastGpsMillis;

    QueueHandle_t _queue;
};

#endif
