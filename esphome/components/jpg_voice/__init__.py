import esphome.codegen as cg
import esphome.config_validation as cv

# from esphome.components import uart
from esphome.const import CONF_ID

# DEPENDENCIES = ["i2s"]

handvoice_ns = cg.esphome_ns.namespace("jpg_voice")
voice_component = handvoice_ns.class_("Voice", cg.Component)  # , uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(voice_component)}).extend(cv.COMPONENT_SCHEMA)
    # .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    # yield uart.register_uart_device(var, config)
