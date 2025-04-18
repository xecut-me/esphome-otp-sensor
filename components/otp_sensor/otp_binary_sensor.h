#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
// #include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace otp_sensor {

class TOTPBinarySensor : public Component, public binary_sensor::BinarySensor {
 public:
  void dump_config() override;
  void setup() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_sensor(text_sensor::TextSensor *input_sensor);
  void set_secret(const std::string &secret) { this->secret_ = secret; }
  void set_interval(int interval) { this->interval_ = interval; }
  void set_time_step(int time_step) { this->time_step_ = time_step; }
  void set_digits(int digits) { this->digits_ = digits; }

  std::string get_otp();
  std::string get_otp(uint64_t step);
  // template<typename T> void set_upper_threshold(T upper_threshold) { this->upper_threshold_ = upper_threshold; }
  // template<typename T> void set_lower_threshold(T lower_threshold) { this->lower_threshold_ = lower_threshold; }

 protected:
  text_sensor::TextSensor *sensor_{nullptr};
  std::string secret_;
  int interval_;
  int time_step_;
  int digits_;
  // TemplatableValue<float> upper_threshold_{};
  // TemplatableValue<float> lower_threshold_{};
};

}  // namespace otp_sensor
}  // namespace esphome
