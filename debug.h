#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  #define DEBUG_PRINT(x)     Serial.println (x)
#else
  #define DEBUG_PRINT(x)
#endif

#endif
