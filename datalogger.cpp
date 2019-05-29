#include "datalogger.h"

#include "config.h"

DataLogger::DataLogger() { }

void DataLogger::setQueue(QueueHandle_t queue) {
  _queue = queue;
}

void DataLogger::setGpsDateTime(long date, long time)
{
  _date = String(date);
  
  char buf[32];
  sprintf(buf, "%02u:%02u:%02u.%c00", time / 1000000, (time % 1000000) / 10000, (time % 10000) / 100, '0' + (time % 100) / 10);
  _time = String(buf);

  _lastGpsMillis = millis();

  /*
  Serial.print("Set current GPS date/time to: ");
  Serial.println(getDateTime());
  */
}

String DataLogger::getDateTime() {
  unsigned long currentMillis = millis();
  
  String _dt = String("20");
  
  String _d = _date;
  String _t = _time; //String(10 * _time + (currentMillis - _lastGpsMillis));

  _dt.concat(_d.charAt(4));
  _dt.concat(_d.charAt(5));
  _dt.concat("-");
  _dt.concat(_d.charAt(2));
  _dt.concat(_d.charAt(3));
  _dt.concat("-");
  _dt.concat(_d.charAt(0));
  _dt.concat(_d.charAt(1));
  _dt.concat("T");
  _dt.concat(_time);
  _dt.concat("Z");

  return _dt;
}

void DataLogger::log(uint16_t pid, float value)
{
  char buf[24];
  byte len = sprintf(buf, "%X,%.2f", pid, value);
  write(buf, len);
}

void DataLogger::log(uint16_t pid, int value)
{
  char buf[24];
  byte len = sprintf(buf, "%X,%d", pid, value);
  write(buf, len);
}

void DataLogger::log(uint16_t pid, unsigned int value)
{
  char buf[24];
  byte len = sprintf(buf, "%X,%u", pid, value);
  write(buf, len);
}

void DataLogger::logFloat(uint16_t pid, float value)
{
  char buf[32];
  byte len = sprintf(buf, "%X,%f", pid, value);
  write(buf, len);
}

void DataLogger::write(const char* buf, byte len)
{
  String payload = getDateTime();
  payload.concat(",");
  payload.concat(buf);

  char  *message = "";
  asprintf(&message, "%s", payload.c_str());
  
  xQueueSend(_queue, (void *)&message, portMAX_DELAY);
}

void DataLogger::loop()
{
}
