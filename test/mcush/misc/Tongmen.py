# coding: utf8
__doc__ = 'products from Dongguan Tongmen Electronics Tech.'
__author__ = 'Peng Shulin <trees_peng@163.com>'
import re
import time
from .. import Env, Instrument
import pymodbus
import pymodbus.client
import pymodbus.client.sync
from pymodbus.client.sync import ModbusSerialClient
from pymodbus.exceptions import ModbusIOException

DEFAULT_UNIT = 0x01  # default instrument address

REG_OUTPUT      = 0x0001
REG_PROTECT     = 0x0002
REG_MODEL       = 0x0003
REG_CLASS       = 0x0004
REG_DIGIT       = 0x0005
REG_VOLTAGE_DISP = 0x0010
REG_CURRENT_DISP = 0x0011
REG_POWER_DISP   = 0x0011
REG_VOLTAGE_SET = 0x0030
REG_CURRENT_SET = 0x0031
REG_OVP_SET = 0x0020
REG_OCP_SET = 0x0021
REG_OPP_SET = 0x0022


class ETM( Instrument.SerialInstrument ):
    DEFAULT_CHECK_RETURN_COMMAND = False
    DEFAULT_IDN = re.compile( '.*' )

    def __init__( self, *args, **kwargs ):
        kwargs['baudrate'] = 9600  # this is fixed
        if not 'unit' in kwargs:
            kwargs['unit'] = DEFAULT_UNIT
        Instrument.SerialInstrument.__init__( self, *args, **kwargs ) 

    def connect( self ):
        self.client = ModbusSerialClient('rtu', port=self.port.port, baudrate=self.baudrate, timeout=self.port.timeout)
        self.client.connect()
         
    def outputEnable( self ):
        self.client.write_register(REG_OUTPUT, 1, unit=self.unit)

    def outputDisable( self ):
        self.client.write_register(REG_OUTPUT, 0, unit=self.unit)
 
    def output( self, volt=None, amp=None, enable=True ):
        if volt is not None:
            volt = float(volt)
            if volt < 0 or volt > self.VMAX:
                raise Exception('Volt out of range')
            self.client.write_register(REG_VOLTAGE_SET, int(volt*100), unit=self.unit)
        if amp is not None:
            amp = float(amp)
            if amp < 0 or amp > self.IMAX:
                raise Exception('Current out of range')
            self.client.write_register(REG_CURRENT_SET, int(amp*1000), unit=self.unit)
        if enable is not None:
            if enable:
                self.outputEnable()
            else:
                self.outputDisable()
        
    def getSetting( self ):
        response = self.client.read_holding_registers(REG_VOLTAGE_SET, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        volt = regs[0] / 100.0
        response = self.client.read_holding_registers(REG_CURRENT_SET, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        amp = regs[0] / 1000.0
        return float(volt), float(amp)

    def readVoltage( self ):
        response = self.client.read_holding_registers(REG_VOLTAGE_DISP, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return regs[0] / 100.0
         
    def readCurrent( self ):
        response = self.client.read_holding_registers(REG_CURRENT_DISP, 1, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return regs[0] / 1000.0

    def readPower( self ):
        response = self.client.read_holding_registers(REG_CURRENT_DISP, 2, unit=self.unit)
        if isinstance(response, ModbusIOException):
            raise Instrument.CommandTimeoutError
        regs = response.registers
        return ((regs[0]<<16)+regs[1]) / 1000.0

    def read( self ):
        return (self.readVoltage(), self.readCurrent())



class ETM605P( ETM ):
    DEFAULT_NAME = 'ETM605P'
    VMAX = 60.0
    IMAX = 5.0

