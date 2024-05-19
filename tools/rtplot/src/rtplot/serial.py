import numpy as np
import serial
import struct
import time

from collections import deque
from datetime import datetime

from .yaml_loader import TenokMsgManager


class DataQueue:
    def __init__(self, max_count):
        self.max_count = max_count
        self.data = deque([0.0] * max_count)

    def add(self, value):
        if len(self.data) < self.max_count:
            self.data.append(val)
        else:
            self.data.pop()
            self.data.appendleft(value)


class CSVSaver:
    def __init__(self, file_name, msg_id):
        self.file_name = file_name
        self.msg_id = msg_id
        self.fd = open(file_name, "w")

    def save(self, data_list):
        for i in range(0, len(data_list)):
            data_str = "{:.7f}".format(float(data_list[i]))

            if i == (len(data_list) - 1):
                self.fd.write(data_str)
                self.fd.write('\n')
            else:
                self.fd.write(data_str)
                self.fd.write(',')

            self.fd.flush()

    def close(self):
        self.fd.close()


class SerialManager:
    def __init__(self, port_name, baudrate, msg_manager):
        # Open the serial port
        self.ser = serial.Serial(
            port_name,
            baudrate,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0.1)  # Timeout = 0.1 second

        print("connected to: " + self.ser.portstr)

        self.recvd_time_last = [time.time()
                                for i in range(0, len(msg_manager.msg_list))]
        self.update_rate_last = 0
        self.msg_manager = msg_manager

    def close(self):
        self.ser.close()

    def parse_bool(self, buffer, i):
        binary_data = struct.pack("B", buffer[i*4])
        bool_data = struct.unpack("?", binary_data)
        # print("payload #%d: %d" % (i, bool_data))
        return bool_data[0]

    def parse_uint8(self, buffer, i):
        binary_data = struct.pack("B", buffer[i*4])
        uint8_data = struct.unpack("H", binary_data)
        # print("payload #%d: %d" % (i, uint8_data))
        return uint8_data[0]

    def parse_int8(self, buffer, i):
        binary_data = struct.pack("B", buffer[i*4])
        int8_data = struct.unpack("h", binary_data)
        # print("payload #%d: %d" % (i, int8_data))
        return int8_data[0]

    def parse_uint16(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        binary_data = data1 + data2
        uint16_data = struct.unpack("H", binary_data)
        # print("payload #%d: %d" % (i, uint16_data))
        return uint16_data[0]

    def parse_int32(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        binary_data = data1 + data2
        int16_data = struct.unpack("h", binary_data)
        # print("payload #%d: %d" % (i, int16_data))
        return int16_data[0]

    def parse_uint32(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        data3 = struct.pack("B", buffer[i*4+2])
        data4 = struct.pack("B", buffer[i*4+3])
        binary_data = data1 + data2 + data3 + data4
        uint32_data = struct.unpack("I", binary_data)
        # print("payload #%d: %d" % (i, uint32_data))
        return uint32_data[0]

    def parse_int32(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        data3 = struct.pack("B", buffer[i*4+2])
        data4 = struct.pack("B", buffer[i*4+3])
        binary_data = data1 + data2 + data3 + data4
        int32_data = struct.unpack("i", binary_data)
        # print("payload #%d: %d" % (i, int32_data))
        return int32_data[0]

    def parse_float(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        data3 = struct.pack("B", buffer[i*4+2])
        data4 = struct.pack("B", buffer[i*4+3])
        binary_data = data1 + data2 + data3 + data4
        float_data = struct.unpack("f", binary_data)
        # print("payload #%d: %f" % (i, float_data))
        return float_data[0]

    def parse_double(self, buffer, i):
        data1 = struct.pack("B", buffer[i*4])
        data2 = struct.pack("B", buffer[i*4+1])
        data3 = struct.pack("B", buffer[i*4+2])
        data4 = struct.pack("B", buffer[i*4+3])
        data5 = struct.pack("B", buffer[i*4+4])
        data6 = struct.pack("B", buffer[i*4+5])
        data7 = struct.pack("B", buffer[i*4+6])
        data8 = struct.pack("B", buffer[i*4+7])
        binary_data = data1 + data2 + data3 + data4 + data5 + data6 + data7 + data8
        double_data = struct.unpack("f", binary_data)
        # print("payload #%d: %f" % (i, double_data))
        return double_data[0]

    def decode_field(self, buffer, c_type, i):
        try:
            if c_type == 'bool':
                return self.parse_bool(buffer, i)
            elif c_type == 'uint8_t':
                return self.parse_uint8(buffer, i)
            elif c_type == 'int8_t':
                return self.parse_int8(buffer, i)
            elif c_type == 'uint16_t':
                return self.parse_uint16(buffer, i)
            elif c_type == 'int16_t':
                return self.parse_int16(buffer, i)
            elif c_type == 'uint32_t':
                return self.parse_uint32(buffer, i)
            elif c_type == 'int32_t':
                return self.parse_int32(buffer, i)
            elif c_type == 'uint64_t':
                return self.parse_uint64(buffer, i)
            elif c_type == 'int64_t':
                return self.parse_int64(buffer, i)
            elif c_type == 'float':
                return self.parse_float(buffer, i)
            elif c_type == 'double':
                return self.parse_double(buffer, i)
        except:
            return None

    def prompt(self, msg_name, msg_id):
        print_str = ''

        curr_time = time.time()
        update_rate = 1.0 / (curr_time - self.recvd_time_last[msg_id])
        self.recvd_time_last[msg_id] = curr_time

        # If the frequency change is too rapidly, then it means the communication is unstable
        if abs(update_rate - self.update_rate_last) > 50:
            print_str = '[{}] received \'{}\' (message, id={})\n'.format(
                datetime.now().strftime('%H:%M:%S'), msg_name, msg_id)
            print_str = print_str + "telemetry speed is too slow, the update rate is unknown!\n"
        else:
            print_str = '[{}] received \'{}\' message (id={}, rate={:.1f}Hz)\n'.format(
                datetime.now().strftime('%H:%M:%S'), msg_name, msg_id, update_rate)

        self.update_rate_last = update_rate

        return print_str

    def decode_msg(self, buffer, msg_id):
        msg_info = self.msg_manager.find_id(msg_id)

        # Message type does not exist
        if msg_info is None:
            return 'failed', None, None, None

        msg_name = msg_info.name
        var_cnt = len(msg_info.fields)

        print_str = self.prompt(msg_name, msg_id)

        data_list = []
        recept_cnt = 0

        for i in range(0, var_cnt):
            var_name = msg_info.fields[i].var_name
            array_size = msg_info.fields[i].array_size
            c_type = msg_info.fields[i].c_type

            # The field is a variable if the array_size is set zero
            is_array = True
            if array_size == 0:
                is_array = False
                array_size = 1

            # Decode array elements of each fields
            for j in range(0, array_size):
                decoded_data = self.decode_field(buffer, c_type, recept_cnt)
                if decoded_data is None:
                    return 'failed', None, None, None

                data_list.append(decoded_data)
                recept_cnt = recept_cnt + 1

                if is_array == True:
                    # Print array
                    print_str = print_str + \
                        '- {}[{}]: {}\n'.format(var_name, j, decoded_data)
                else:
                    # Print variable
                    print_str = print_str + \
                        '- {}: {}\n'.format(var_name, decoded_data)

        # Print prompt message
        print(print_str, end='')

        return 'success', msg_id, msg_name, data_list

    def receive_msg(self):
        buffer = []
        checksum = 0

        try:
            c = self.ser.read(1)
            c = c.decode("ascii")
        except:
            return 'failed', None, None, None

        # print("c:")
        # print(c)

        # Wait for the start byte
        if c == '@':
            pass
        else:
            return 'failed', None, None, None

        # Use try block in case encounter serial port timeout
        try:
            # Receive payload size
            payload_count, =  struct.unpack("B", self.ser.read(1))
            # print('payload size: %d' %(payload_count))

            # Receive message ID
            _msg_id, =  struct.unpack("c", self.ser.read(1))
            msg_id = ord(_msg_id)

            # Receive payloads + checksum
            buf = self.ser.read(payload_count + 1)

            # Verify the checksum
            for i in range(0, payload_count):
                buffer.append(buf[i])
                buffer_checksum = buffer[i]
                checksum ^= buffer_checksum

            received_checksum = buf[payload_count]

            if received_checksum != checksum:
                print("error: checksum mismatched")
                return 'failed', None, None, None
        except:
            return 'failed', None, None, None

        # Decode message fields
        result, msg_id, msg_name, data = self.decode_msg(buffer, msg_id)
        if result == 'failed':
            return 'failed', None, None, None

        return 'success', msg_id, msg_name, data
