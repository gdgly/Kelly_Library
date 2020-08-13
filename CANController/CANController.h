#ifndef _CAN_CONTROLLER_H_
#define _CAN_CONTROLLER_H_


typedef struct
{
	size_t (*can_write)();
} can_controller_t;


//class CANControllerClass : public Stream {
//
//public:
//  virtual int begin(long baudRate);
//  virtual void end();
//
//  int beginPacket(int id, int dlc = -1, bool rtr = false);
//  int beginExtendedPacket(long id, int dlc = -1, bool rtr = false);
//  virtual int endPacket();
//
//  virtual int parsePacket();
//  long packetId();
//  bool packetExtended();
//  bool packetRtr();
//  int packetDlc();
//
//  // from Print
//  virtual size_t write(uint8_t byte);
//  virtual size_t write(const uint8_t *buffer, size_t size);
//
//  // from Stream
//  virtual int available();
//  virtual int read();
//  virtual int peek();
//  virtual void flush();
//
//  virtual void onReceive(void(*callback)(int));
//
//  virtual int filter(int id) { return filter(id, 0x7ff); }
//  virtual int filter(int id, int mask);
//  virtual int filterExtended(long id) { return filterExtended(id, 0x1fffffff); }
//  virtual int filterExtended(long id, long mask);
//
//  virtual int observe();
//  virtual int loopback();
//  virtual int sleep();
//  virtual int wakeup();
//
//protected:
//  CANControllerClass();
//  virtual ~CANControllerClass();
//
//protected:
//  void (*_onReceive)(int);
//
//  bool _packetBegun;
//  long _txId;
//  bool _txExtended;
//  bool _txRtr;
//  int _txDlc;
//  int _txLength;
//  uint8_t _txData[8];
//
//  long _rxId;
//  bool _rxExtended;
//  bool _rxRtr;
//  int _rxDlc;
//  int _rxLength;
//  int _rxIndex;
//  uint8_t _rxData[8];
//};


#endif /* KELLY_LIBRARY_CANCONTROLLER_CANCONTROLLER_H_ */
