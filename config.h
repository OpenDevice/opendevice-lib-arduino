#ifndef OPENDEVICE_CONFIG_H_
#define OPENDEVICE_CONFIG_H_

#define ENABLE_SERIAL 1
#define DEBUG_CON	1 // set 1 to enable (receiving debug)

#define API_VERSION   1 // software version of this library
#define OPENDEVICE_LIBRARY_VERSION 050


#define DATA_BUFFER  160
#define MAX_LISTENERS 10
#define MAX_DEVICE 10
#define MAX_COMMAND 10 // this is used for user command callbacks
#define MAX_COMMAND_STRLEN 14
#define READING_INTERVAL 100 // sensor reading interval (ms)
#define KEEP_ALIVE_INTERVAL 1500
#define DEFAULT_BAUD 9600

#endif /* OPENDEVICE_CONFIG_H_ */
