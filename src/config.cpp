#include "config.h"

namespace od {
	ConfigClass Config = {
    CONFIG_VERSION,
    // The default values
    "OpenDevice",
    "api.opendevice.io",
    "*", // APP_ID
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // MAC
    { 0, 0, 0, 0 }, // IP
    2, // ResetPIN
    false, // debugMode
    true, // keepAlive
    DEBUG_SERIAL, // debugTarget
    CONNECTION_MODE_SERVER, // ConnectionMode
    0 // devicesLength
    // Device ...
  };
}
