#include "BluefruitSteeringServer.h"

// -------------------------------------------------------------------------------------------
// COMPILER DIRECTIVE to allow/suppress DEBUG messages that help debugging...
// Uncomment general "#define DEBUG" to activate for this class
#define DEBUG
// Include these debug utility macros in all cases!
#include "DebugUtils.h"
// -------------------------------------------------------------------------------------------

//BLE Steering Service and Characteristics Definitions
#define STEERING_DEVICE_UUID      "347b0001-7635-408b-8918-8ff3949ce592"
#define STEERING_ANGLE_CHAR_UUID  "347b0030-7635-408b-8918-8ff3949ce592"    //notify
#define STEERING_RX_CHAR_UUID     "347b0031-7635-408b-8918-8ff3949ce592"    //write
#define STEERING_TX_CHAR_UUID     "347b0032-7635-408b-8918-8ff3949ce592"    //indicate

//#define UUID16_SVC_BATTERY        BLEUuid((uint16_t)0x180F)
//#define UUID16_CHR_BATTERY_LEVEL  BLEUuid((uint16_t)0x2A19)

#define MAX_STEER_ANGLE 35		// Steering Angle shoud not exceed this value
#define STEERANGLETHRESHOLD (1.5F)

/* -------------------------------------------------------------------------------------------
* Short Device Name --------------------------------------------------------------------------
* --------------------------------------------------------------------------------------------
* This project is named: "Open Virtual Steering", too long to be suitable for BLE device 
* naming and advertising. A made-up short device name is default inserted.
* However, users may need to configure the short device name manually for interoperability on 
* certain platforms to allow for instant steering device recognition and full functionality.  
* For general information on BLE naming conventions, 
*                    refer to https://www.bluetooth.com/specifications/specs/ 
* -------------------------------------------------------------------------------------------*/
#define SHORTDEVICENAME "STERZOPN"  // String length is max 8
// -------------------------------------------------------------------------------------------

// BluefruitSteeringServer Class Members----------------------------------------------------------------------------------------

BluefruitSteeringServer& BluefruitSteeringServer::getInstance() {
    static BluefruitSteeringServer instance;
    return instance;
}

void BluefruitSteeringServer::begin(void) {
  // Initialise the Bluefruit module
  DEBUG_PRINTLN("Initialise the Bluefruit nRF52 module");
  // begin (Peripheral = 1, Central = 0)
  Bluefruit.begin(1, 0);
  // Set the device name (keep it short!)
  Bluefruit.setName(SHORTDEVICENAME);
  //  Bluefruit.setTxPower(4);
  Setup_Service();
  Start_Advertising();
} // end setup

float BluefruitSteeringServer::constrainSteerAngle(float angle) {
  if (fabs(angle) > MAX_STEER_ANGLE) { 
    if (angle > 0) { angle = MAX_STEER_ANGLE; }
    else { angle = - MAX_STEER_ANGLE; }
  }
  if (fabs(angle) < STEERANGLETHRESHOLD) { angle= 0.0; }
  return angle;
}

void BluefruitSteeringServer::Setup_Service(void) {
  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  
  // Configure and start the Battery service
  DEBUG_PRINTLN("Configure and start the Battery service");
  BatteryService = BLEService(UUID16_SVC_BATTERY);
  BatteryService.begin();
  BatteryChar = BLECharacteristic(UUID16_CHR_BATTERY_LEVEL);
  BatteryChar.setProperties(CHR_PROPS_NOTIFY);  // type is "notify"
  BatteryChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); // readAccess, NO writeAccess
  BatteryChar.setFixedLen(1); // uint8_t stored 1 byte
  BatteryChar.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
  BatteryChar.begin(); // Start
  // Set first value of Battery Percentage to expose
  BatteryChar.write8((uint8_t)90); // uint8_t batteryPercentage 90%

  // Create the BLE Server Service
  DEBUG_PRINTLN("Configure and start Steering Server service...");
  SteeringDeviceService = BLEService(STEERING_DEVICE_UUID);
  SteeringDeviceService.begin();
  // Create BLE Server Characteristics
  // 30
  SteeringChar = BLECharacteristic(STEERING_ANGLE_CHAR_UUID);
  SteeringChar.setProperties(CHR_PROPS_NOTIFY);  // type is "notify"
  SteeringChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); // readAccess, NO writeAccess
  SteeringChar.setFixedLen(4); // sizeof a float number = 4
  SteeringChar.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
  SteeringChar.begin(); // Start
  const uint8_t defaultValue[4] = {0x0,0x0,0x0,0x0}; // Default 4 byte Value zero
  SteeringChar.write(defaultValue, 4); // Set default value
  // 31
  RxChar = BLECharacteristic(STEERING_RX_CHAR_UUID);
  RxChar.setProperties(CHR_PROPS_WRITE); // Write !!
  RxChar.setPermission(SECMODE_NO_ACCESS, SECMODE_OPEN); // NO readAccess, writeAccess !
  RxChar.setMaxLen(6); // The charactersitic's data length
  RxChar.begin();
  RxChar.setWriteCallback(SteeringRxChar_callback);  // Respond to write events !!
  // 32
  TxChar = BLECharacteristic(STEERING_TX_CHAR_UUID);
  TxChar.setProperties(CHR_PROPS_INDICATE); // | CHR_PROPS_READ);
  TxChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); // readAccess, NO writeAccess
  TxChar.setMaxLen(6);
  TxChar.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  TxChar.begin();
  const uint8_t defaultValueTxChar[4] = {0x03,0x10,0x12,0x34}; // Default 4 byte value
  TxChar.write(defaultValueTxChar, 4); // Set default value
}

void BluefruitSteeringServer::Start_Advertising(void) {
  // Setup advertising
  DEBUG_PRINTLN("Define the Server advertising data...");
  if (Bluefruit.Advertising.isRunning())
    Bluefruit.Advertising.stop();
  // Prepare a full Advertising packet
  //Bluefruit.Advertising.addAppearance(appearance); NOT USED value == 0
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  //Bluefruit.Advertising.addTxPower();
  // Include Steering Service
  Bluefruit.Advertising.addService(SteeringDeviceService); 
  // Include Name
  Bluefruit.Advertising.addName();
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   */
  Bluefruit.Advertising.restartOnDisconnect(true); // true is default --> auto restart
  Bluefruit.Advertising.setInterval(48, 244);      // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);        // number of seconds in fast mode

  // Flash led while advertising
  Bluefruit.autoConnLed(true);
  DEBUG_PRINTLN("Start Advertising...");
  Bluefruit.Advertising.start(0); // 0 = Don't stop advertising after n seconds  
  DEBUG_PRINTLN("Waiting for Client to connect and notify...");
}

//Characteristic Callback RXChar (31)
void BluefruitSteeringServer::SteeringRxChar_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  uint8_t RxDataLen = (uint8_t)len;    // Get the actual length of data bytes and type cast to (uint8_t)
  uint8_t RxData[RxDataLen];
  memset(RxData, 0, RxDataLen); // set to zero
  // Display the raw request packet actual length
  DEBUG_PRINTF("Steering RxChar (31) Data [Len: %d] [Data: ", len);
  // Transfer the contents of data to RxData for further processing!
  for (int i = 0; i < RxDataLen; i++) {
      RxData[i] = *data++;
      // Display the raw request packet byte by byte in HEX
      DEBUG_PRINTF("%02X ", RxData[i], HEX);
  }
  DEBUG_PRINTLN("]"); 
};

void BluefruitSteeringServer::PrintPeerAddress(uint8_t addr[6]) {
  for (int i = 1; i < 6; i++) {
      // Display byte by byte in HEX reverse: little Endian
      DEBUG_PRINTF("%02X:",addr[(6-i)], HEX);
  }
   DEBUG_PRINTF("%02X ",addr[0], HEX);
}

void BluefruitSteeringServer::cccd_callback(uint16_t conn_handle, BLECharacteristic* chr, uint16_t cccd_value) {
    // When changed, display the Notify Status for all NOTIFY charcteristics
#ifdef DEBUG
    DEBUG_PRINTF("Server CCCD Updated to: [%d] --> ", cccd_value);
    // Check the characteristic UUID this CCCD callback is associated with,
    // in case this handler is used for multiple CCCD records.
    if (chr->uuid == BluefruitSteeringServer::getInstance().SteeringChar.uuid) { 
        if (chr->notifyEnabled(conn_handle)) { 
            DEBUG_PRINT("SteeringChar (30) 'Notify' enabled");
        } else {
            DEBUG_PRINT("SteeringChar (30) 'Notify' disabled");
        }
    }
    if (chr->uuid == BluefruitSteeringServer::getInstance().TxChar.uuid) { 
        if (chr->indicateEnabled(conn_handle)) { 
            DEBUG_PRINT("TxChar (32) 'Indicate' enabled");
        } else {
            DEBUG_PRINT("TxChar (32) 'Indicate' disabled");
        }
    }
    if (chr->uuid == BluefruitSteeringServer::getInstance().BatteryChar.uuid) { 
        if (chr->notifyEnabled(conn_handle)) { 
            DEBUG_PRINT("BatteryChar 'Notify' enabled");
        } else {
            DEBUG_PRINT("BatteryCahr 'Notify' disabled");
        }
    }
    DEBUG_PRINTLN();
#endif    
} // end CCCD callback

void BluefruitSteeringServer::connect_callback(uint16_t conn_handle) {
#ifdef DEBUG
  char client_name[32] = {0};
  uint8_t client_addr[6] = {0};
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  connection->getPeerName(client_name, sizeof(client_name));
  ble_gap_addr_t peer_address = connection->getPeerAddr();
  memcpy(client_addr, peer_address.addr, 6);
  DEBUG_PRINTF("Server connected to Client: %s conn handle [%d] MAC Address: ", client_name, conn_handle);
  BluefruitSteeringServer::getInstance().PrintPeerAddress(client_addr);
  DEBUG_PRINTLN();
#endif 
  BluefruitSteeringServer::getInstance().isConnected = true;
}

void BluefruitSteeringServer::disconnect_callback(uint16_t conn_handle, uint8_t reason) {
#ifdef DEBUG
  DEBUG_PRINTF("Server disconnected from Client: conn handle [%d] reason [%02X]\n", conn_handle, reason, HEX);
#endif
  BluefruitSteeringServer::getInstance().isConnected = false;
  // Auto restart Advertising !!!!
}

bool BluefruitSteeringServer::updateSteeringValue(float angle) {
    if (SteeringChar.notifyEnabled()) {
        float steerAngle = constrainSteerAngle(angle);
        SteeringChar.notify32(steerAngle); // float stored in array of 4 bytes
        return true;
    }
    return false;
}

bool BluefruitSteeringServer::updateBatteryPercentage(uint8_t batteryPercentage) {
    if (BatteryChar.notifyEnabled()) {
        BatteryChar.notify8(batteryPercentage);      // Update present battery level percentage
        DEBUG_PRINTF("Updated Battery Level: %3d%%\n", batteryPercentage);
        return true;
    }
    return false;
}

bool BluefruitSteeringServer::sendResponse(const uint8_t *data, size_t size) {
    if (TxChar.indicateEnabled()) {
        TxChar.indicate(data, size);
        return true;
    }
    return false;
}