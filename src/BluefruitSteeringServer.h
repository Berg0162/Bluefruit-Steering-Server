#ifndef BLUEFRUIT_STEERING_SERVER_H
#define BLUEFRUIT_STEERING_SERVER_H
#include <arduino.h>
#include <bluefruit.h>

class BluefruitSteeringServer {
public:
    static BluefruitSteeringServer& getInstance();

    void begin(void);
    bool updateSteeringValue(float angle);
    bool updateBatteryPercentage(uint8_t batteryPercentage);
    bool isConnected = false;

private:
    // Singleton
    BluefruitSteeringServer() = default;
    BluefruitSteeringServer(const BluefruitSteeringServer&) = delete;
    BluefruitSteeringServer& operator=(const BluefruitSteeringServer&) = delete;

    BLEService SteeringDeviceService;
    BLECharacteristic SteeringChar; //notify
    BLECharacteristic RxChar;       //write
    BLECharacteristic TxChar;       //indicate
    BLEService BatteryService;
    BLECharacteristic BatteryChar;  //notify

    void PrintPeerAddress(uint8_t addr[6]);
    float constrainSteerAngle(float angle);

    void static connect_callback(uint16_t conn_handle) ;
    void static disconnect_callback(uint16_t conn_handle, uint8_t reason);
    void static cccd_callback(uint16_t conn_handle, BLECharacteristic* chr, uint16_t cccd_value);
    void static SteeringRxChar_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);
    void Start_Advertising(void);
    void Setup_Service(void);
    bool sendResponse(const uint8_t *data, size_t size);

};



#endif // BLUEFRUIT_STEERING_SERVER_H