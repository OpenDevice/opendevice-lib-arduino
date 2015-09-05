/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/A.+First+Steps+with+ODev
 * This example code is in the public domain.
 * *****************************************************************************
 */


#include <SoftSerial.h>     /* Allows Pin Change Interrupt Vector Sharing */
#include <TinyPinChange.h>  /* Ne pas oublier d'inclure la librairie <TinyPinChange> qui est utilisee par la librairie <RcSeq> */
//#include <SoftwareSerial.h>

#include <Command.h>
#include <DeviceConnection.h>

SoftSerial serial(5, 4); // RX, TX

DeviceConnection connection(serial); 

void setup(){
  serial.begin(9600);
  connection.setDefaultListener(onMessageReceived);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop(){
  
  connection.checkDataAvalible();

}

void onMessageReceived(Command cmd) {
  if (connection.cmd.deviceID > 0) {
    digitalWrite(connection.cmd.deviceID - 1, connection.cmd.value);
    notifyReceived(true);
  }else{
    notifyReceived(false);
  }
}

/** Send reply stating that the command was received successfully */
void notifyReceived(bool status){
  connection.cmd.type = CommandType::DEVICE_COMMAND_RESPONSE;
  connection.cmd.value = (status ? ResponseStatus::SUCCESS : ResponseStatus::NOT_FOUND);
  connection.cmd.length = 0;
  connection.send(connection.cmd);
}

