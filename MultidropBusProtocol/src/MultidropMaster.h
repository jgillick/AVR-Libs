

#ifndef MultidropMaster_H
#define MultidropMaster_H

#include <avr/io.h>
#include <stdint.h>
#include "Multidrop.h"

// How many times master will try to get a node's address, before deciding it is done
#ifndef MD_MASTER_ADDR_MAX_TIMEOUTS
#define MD_MASTER_ADDR_MAX_TIMEOUTS 3
#endif

class MultidropMaster: public Multidrop {

public:
  enum adr_state_t {
    ADR_WAITING,
    ADR_DONE,
    ADR_ERROR
  };

  MultidropMaster(MultidropData *_serial);

  // Set the number of nodes on the bus
  void setNodeLength(uint8_t);

  // Add the pin and registers for the next daisy chain line
  // This is for master nodes that only have an out line and the 
  // bus does not come back around to master
  void addDaisyChain(volatile uint8_t next_pin_num,
                     volatile uint8_t* next_ddr_register, 
                     volatile uint8_t* next_port_register, 
                     volatile uint8_t* next_pin_register);

  // Start a new message to send
  uint8_t startMessage(uint8_t command,
                      uint8_t destination=BROADCAST_ADDRESS,
                      uint8_t dataLength=0,
                      uint8_t batchMode=false,
                      uint8_t responseMessage=false);

  // Start the addressing processes
  // You'll need to call `checkForAddresses` regularly to handle the addressing process
  void startAddressing();
  
  // Check for new addresses received
  adr_state_t checkForAddresses(uint16_t time);

  // When sending a response request message, we need three more values:
  //   * buff: The buffer to store the responses for all nodes. This needs to be initialized
  //        large enough for everything (number of nodes * size of response for each).
  //   * time: The current system time (used for timeout).
  //   * timeout: How long master will wait for any node to respond. You will need to call
  //        setTime frequently to let master know what the current time is.
  //   * defaultResponse: If a node doesn't respond by the timeout, this is the response that
  //        is registered for that node. This must be an array, the same length as the expected
  //        node response.
  void setResponseSettings(uint8_t buff[], uint16_t time, uint16_t timeout, uint8_t defaultResponse[]);

  // Call regularly to check for node responses.
  //  - time: Used to keep accurate time and timeout nodes who take too long to respond
  // Return: true when all nodes have responded
  uint8_t checkForResponses(uint16_t time);

  // Send a single byte of data
  uint8_t sendData(uint8_t data);

  // Send several bytes of data
  uint8_t sendData(uint8_t *data, uint16_t len);

  // Send the message
  uint8_t finishMessage();

private:
  enum State {
    EOM,
    HEADER_SENT,
    ADDRESSING,
    DATA_SENDING
  };

  uint16_t timeoutTime,
           timeoutDuration,
           responseIndex;

  uint8_t  nodeNum,
           destAddress,
           dataLength,
           state,
           waitingOnNodes,
           nodeAddressTimeouts,
           lastAddressReceived;

  uint8_t *responseBuff,
          *defaultResponseValues;

  // Send a byte and, optionally, update the messageCRC value
  void sendByte(uint8_t b, uint8_t updateCRC=1);
};

#endif
