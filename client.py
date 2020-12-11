import os
import socket
import time
import struct
from network import LoRa

# A basic package header, B: 1 byte for the deviceId, B: 1 byte for the pkg size
_LORA_PKG_FORMAT = "BB%ds"
_LORA_PKG_ACK_FORMAT = "BBB"
DEVICE_ID = 0x01
MAX_ACK_WAIT = 50


# Open a Lora Socket, use tx_iq to avoid listening to our own messages
# Please pick the region that matches where you are using the device:
# Asia = LoRa.AS923
# Australia = LoRa.AU915
# Europe = LoRa.EU868
# United States = LoRa.US915
lora = LoRa(mode=LoRa.LORA, tx_iq=True, region=LoRa.EU868)
lora_sock = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
lora_sock.setblocking(False)
lora.sf(11)
for i in range(3, 16):
    lora.remove_channel(i)
for i in range (0, 2):
    lora.add_channel(i, frequency=868100000, dr_min=3, dr_max=3)
i = 0
to = 0
while(True):
    # Package send containing a simple string
    msg = "Device {1} is here {0}".format(i, DEVICE_ID)
    pkg = struct.pack(_LORA_PKG_FORMAT % len(msg), DEVICE_ID, len(msg), msg)
    lora_sock.send(pkg)
    print("{0}, {1}".format(msg, i))
    i = i + 1
    # Wait for the response from the gateway. NOTE: For this demo the device does an infinite loop for while waiting the response. Introduce a max_time_waiting for you application
    waiting_ack = True
    to = 0
    while(waiting_ack or to <= MAX_ACK_WAIT):
        to = to + 1

        recv_ack = lora_sock.recv(256)

        if (len(recv_ack) > 0):
            device_id, pkg_len, ack = struct.unpack(_LORA_PKG_ACK_FORMAT, recv_ack)
            if (device_id == DEVICE_ID):
                if (ack == 200):
                    waiting_ack = False
                    # If the uart = machine.UART(0, 115200) and os.dupterm(uart) are set in the boot.py this print should appear in the serial port
                    print("ACK")
                else:
                    waiting_ack = False
                    # If the uart = machine.UART(0, 115200) and os.dupterm(uart) are set in the boot.py this print should appear in the serial port
                    print("Message Failed")
        else:
            if (MAX_ACK_WAIT is to):
                waiting_ack = False

    time.sleep(5)
