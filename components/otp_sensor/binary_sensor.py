import esphome.codegen as cg
from esphome.components import binary_sensor, text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_SOURCE,
    CONF_TYPE,
    CONF_SENSOR_ID,
)

CODEOWNERS = ["@xecut-me"]
AUTO_LOAD = ["binary_sensor","text_sensor"]

otp_sensor_ns = cg.esphome_ns.namespace("otp_sensor")

TOTPBinarySensor = otp_sensor_ns.class_(
    "TOTPBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONF_TOTP = "totp"
CONF_SOURCES = "sources"
CONF_SECRET = "secret"
CONF_INTERVAL = "interval"
CONF_TIME_STEP = "time_step"
CONF_DIGITS = "digits"


CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(TOTPBinarySensor).extend(
{
    cv.Required(CONF_SENSOR_ID): cv.use_id(text_sensor.TextSensor),
    cv.Required(CONF_SECRET): cv.string_strict,
    cv.Optional(CONF_INTERVAL, default=0): cv.int_,
    cv.Optional(CONF_TIME_STEP, default=30): cv.int_,
    cv.Optional(CONF_DIGITS, default=6): cv.int_range(1,8),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)
    cg.add(var.set_secret(config[CONF_SECRET]))
    cg.add(var.set_interval(config[CONF_INTERVAL]))
    cg.add(var.set_time_step(config[CONF_TIME_STEP]))
    cg.add(var.set_digits(config[CONF_DIGITS]))
    sens = await cg.get_variable(config[CONF_SENSOR_ID])
    cg.add(var.set_sensor(sens))
