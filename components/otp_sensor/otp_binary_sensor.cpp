#include "otp_binary_sensor.h"
#include "esphome/core/log.h"
#include <cstring>
#include <ctime>
#include <cmath>
#include <mbedtls/md.h>
#include <sys/types.h>
#include <byteswap.h>
#include "base32_decode.h"

namespace esphome {
namespace otp_sensor {

static const char *const TAG = "otp.binary_sensor";

void TOTPBinarySensor::setup() {
    this->publish_initial_state(false);
}

void TOTPBinarySensor::set_sensor(text_sensor::TextSensor *text_sensor) {
  this->sensor_ = text_sensor;
  this->sensor_->add_on_state_callback([this](std::string sensor_value) {
    bool res = false;
    this->publish_state(res);
    uint64_t step = (time(nullptr)/ this->time_step_);

    for (int i=0;i<=this->interval_;i++){
      if ( !sensor_value.compare(this->get_otp(step+i)) ||
        (i && !sensor_value.compare(this->get_otp(step-i)))
      ){
        res = true;
        break;
      }
    }

    this->publish_state(res);
  });
}

void TOTPBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "OTP Binary Sensor", this);
  LOG_TEXT_SENSOR("  ", "Sensor", this->sensor_);
  ESP_LOGCONFIG(TAG, "  Secret: %s", this->secret_.c_str());
  ESP_LOGCONFIG(TAG, "  Interval: %d", this->interval_);
  ESP_LOGCONFIG(TAG, "  Time Step: %d", this->time_step_);
  ESP_LOGCONFIG(TAG, "  Digits: %d", this->digits_);
}

uint32_t hotp_dt(const uint8_t *digest)
{
    uint64_t offset;
    uint32_t bin_code;

    // dynamically truncates hash
    offset   = digest[19] & 0x0fU;

    bin_code = (digest[offset] & 0x7fU) << 24u
               | (digest[offset+1u] & 0xffU) << 16u
               | (digest[offset+2u] & 0xffU) << 8u
               | (digest[offset+3u] & 0xffU);

    return bin_code;
}

std::string TOTPBinarySensor::get_otp(){
  return this->get_otp(time(nullptr)/ this->time_step_);
}

std::string TOTPBinarySensor::get_otp(uint64_t step) {
  uint8_t digest[128];
  uint8_t key[64];//= {0x48,0x65,0x6c,0x6c,0x6f,0x21,0xde,0xad,0xbe,0xef};
  auto key_len = base32_decode(this->secret_.c_str(),key,sizeof(key));
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #ifndef bswap_64
    #define bswap_64(x)			\
    ((((x) & 0xff00000000000000ull) >> 56)	\
    | (((x) & 0x00ff000000000000ull) >> 40)	\
    | (((x) & 0x0000ff0000000000ull) >> 24)	\
    | (((x) & 0x000000ff00000000ull) >> 8)	\
    | (((x) & 0x00000000ff000000ull) << 8)	\
    | (((x) & 0x0000000000ff0000ull) << 24)	\
    | (((x) & 0x000000000000ff00ull) << 40)	\
    | (((x) & 0x00000000000000ffull) << 56))
  #endif
  step=bswap_64(step);
  #endif

  mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), key, key_len , (const unsigned char*)&step, sizeof(step), digest);
  uint32_t dbc = hotp_dt(digest);

    // Third Phase: calculate the mod_k of the dbc to get the correct number
    int power = (int)pow(10, this->digits_);
    uint32_t otp = dbc % power;
    char buf[10] = {0};
    snprintf(buf,10,"%0*d",this->digits_,otp);
    return std::string(buf);
}

}  // namespace analog_threshold
}  // namespace esphome
