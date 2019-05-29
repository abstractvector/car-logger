#include <Arduino.h>
#include <FreematicsPlus.h>

#include "datalogger.h"
#include "recorder.h"

#include "config.h"
#include "debug.h"

#define BUF 1024

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

QueueHandle_t queue;

FreematicsESP32 sys;

DataLogger logger;
Recorder recorder;

COBD obd;
bool obdConnected = false;

GPS_DATA *gd = 0;
uint32_t lastGPStime = 0;

unsigned long _lastMillis = 0;

void setup()
{
  // init LED pin
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LED, HIGH);

  delay(1000);

  // initialize USB serial
  Serial.begin(115200);
  Serial.print("Hardware: ESP32 ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.print("MHz ");
  Serial.print(getFlashSize() >> 10);
  Serial.println("MB Flash");

  while (!sys.begin())
    ;

  DEBUG_PRINT((String) "Freematics firmware version: " + sys.version);

  recorder.begin();

  uint8_t *dataSerial = (uint8_t *)malloc(BUF);
  queue = xQueueCreate(100, sizeof(dataSerial));

  if (queue == NULL)
  {
    Serial.println("Error creating the queue");
  }

  logger.setQueue(queue);

  setupGps();
  setupObd();

  xTaskCreatePinnedToCore(
      Task1Loop, /* Task function. */
      "Task1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Task1,    /* Task handle to keep track of created task */
      1          /* pin task to core 1 */
  );

  xTaskCreatePinnedToCore(
      Task2Loop, /* Task function. */
      "Task2",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      3,         /* priority of the task */
      &Task2,    /* Task handle to keep track of created task */
      1          /* pin task to core 1 */
  );

  xTaskCreatePinnedToCore(
      Task3Loop, /* Task function. */
      "Task3",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      2,         /* priority of the task */
      &Task3,    /* Task handle to keep track of created task */
      1          /* pin task to core 1 */
  );

  pinMode(PIN_LED, LOW);
}

byte m_state = 0;

bool checkState(byte flags) { return (m_state & flags) == flags; }
void setState(byte flags) { m_state |= flags; }

void setupGps()
{
  if (!checkState(STATE_GPS_FOUND))
  {
    Serial.print("GPS...");
    if (sys.gpsBegin(GPS_SERIAL_BAUDRATE, false))
    {
      setState(STATE_GPS_FOUND);
      Serial.println("OK");
      //waitGPS();
    }
    else
    {
      sys.gpsEnd();
      Serial.println("NO");
    }
  }
}

void setupObd()
{
  obd.begin(sys.link);
}

void logGPSData()
{
  // issue the command to get parsed GPS data
  if (checkState(STATE_GPS_FOUND) && sys.gpsGetData(&gd) && lastGPStime != gd->time)
  {
    // set GPS ready flag
    setState(STATE_GPS_READY);

    logger.setGpsDateTime(gd->date, gd->time);

    logger.log(PID_GPS_DATE, gd->date);
    logger.log(PID_GPS_TIME, gd->time);
    logger.log(PID_GPS_ALTITUDE, gd->alt);
    logger.log(PID_GPS_SPEED, gd->speed);
    logger.log(PID_GPS_SAT_COUNT, gd->sat);

    logger.logFloat(PID_GPS_LATITUDE, gd->lat);
    logger.logFloat(PID_GPS_LONGITUDE, gd->lng);

    lastGPStime = gd->time;
  }
}

void logAdapterMetrics()
{
  logger.log(PID_DEVICE_TEMP, readChipTemperature());
}

void loop()
{
  // do nothing here because we're using FreeRTOS tasks to handle the loops
}

void Task1Loop(void *pvParameters)
{
  Serial.print("GPS logger running on core ");
  Serial.println(xPortGetCoreID());

  unsigned long currentMillis;

  for (;;)
  {
    currentMillis = millis();

    if (currentMillis - _lastMillis >= 1000)
    {
      _lastMillis = currentMillis;

      DEBUG_PRINT("[GPS] Logging");

      logGPSData();
      logAdapterMetrics();
    }

    logger.loop();
  }
}

void Task2Loop(void *pvParameters)
{
  Serial.print("Data recorder running on core ");
  Serial.println(xPortGetCoreID());

  unsigned long currentMillis;
  char *message;

  for (;;)
  {
    recorder.loop();

    currentMillis = millis();

    if (queue != 0 && xQueueReceive(queue, &message, portMAX_DELAY) == pdTRUE)
    {
      pinMode(PIN_LED, HIGH);
      recorder.send(message);
      free(message);
      pinMode(PIN_LED, LOW);
    }
  }
}

void Task3Loop(void *pvParameters)
{
  Serial.print("OBD-II logger running on core ");
  Serial.println(xPortGetCoreID());

  unsigned long currentMillis;

  for (;;)
  {
    currentMillis = millis();

    if (currentMillis - _lastMillis >= 1000)
    {
      _lastMillis = currentMillis;

      DEBUG_PRINT("[OBD-II] Logging");

      // log OBD data
      logObd2();
    }
  }
}

void logObd2()
{
  if (!obdConnected)
  {
    Serial.print("[OBD-II] Connecting");
    if (obd.init())
    {
      Serial.println("OK");
      obdConnected = true;
    }
    else
    {
      Serial.println();
      delay(1000);
    }
    return;
  }

  int value;

  if (obd.readPID(PID_RPM, value))
  {
    logger.log(PID_RPM | 0x100, value);
  }

  if (obd.readPID(PID_SPEED, value))
  {
    logger.log(PID_SPEED | 0x100, value);
  }

  if (obd.readPID(PID_DISTANCE, value))
  {
    logger.log(PID_DISTANCE | 0x100, value);
  }

  logger.log(PID_BATTERY_VOLTAGE, obd.getVoltage());

  if (obd.errors > 2)
  {
    Serial.println("OBD disconnected");
    obdConnected = false;
    obd.reset();
  }
}
