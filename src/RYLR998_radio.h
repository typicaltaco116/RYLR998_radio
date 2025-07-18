#include <Arduino.h>

class RYLR998_radio
{
  public:

    typedef enum {
      TRANSCIEVER = 0, SLEEP, SMART
    } device_mode_e;

    RYLR998_radio(HardwareSerial*);


    void transmit(uint16_t, size_t, char*);
    void get_last_transmission(uint16_t*, size_t*, char*);

    void get_received(uint16_t*, size_t*, char*, uint8_t*, uint8_t*);


    void reset(void);

    void set_mode(device_mode_e);
    device_mode_e get_mode(void);

    void set_frequency(int);
    int get_frequency(void);

    void set_parameters(uint8_t, uint8_t, uint8_t);
    void get_parameters(uint8_t*, uint8_t*, uint8_t*);

    void set_address(uint16_t);
    uint16_t get_address(void);

    void set_networkID(uint8_t);
    uint8_t get_networkID(void);

    void set_radio_power(uint8_t);
    uint8_t get_radio_power(void);


  private:

    HardwareSerial *radio_serial;

    void wait_for_handshake(void);
    int parse_for_int(int, size_t);
    bool compare_string(const char*, const char*);

    int convert_text(size_t, char*);
    void convert_int(int, size_t, char*);
};
