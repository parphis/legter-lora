from network import LoRa
import socket
import machine
import time

# initialise LoRa in LORA mode
# Please pick the region that matches where you are using the device:
# Asia = LoRa.AS923
# Australia = LoRa.AU915
# Europe = LoRa.EU868
# United States = LoRa.US915
# more params can also be given, like frequency, tx power and spreading factor
#lora = LoRa(mode=LoRa.LORA, region=LoRa.EU868)
lora = LoRa(
    mode=LoRa.LORA,
    region=LoRa.EU868,
    frequency=868000000,
    bandwidth=LoRa.BW_125KHZ,
    sf=7,
    public=False)

#for i in range(3, 16):
#    lora.remove_channel(i)
#for i in range (0, 2):
#    lora.add_channel(i, frequency=868100000, dr_min=3, dr_max=3)

# create a raw LoRa socket
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
s.setblocking(False)
i = 0
while True:
    # send some data
    #s.setblocking(True)
    #s.send('Hello from parphis-laptop')

    # get any data received...
    #s.setblocking(False)
    data = s.recv(128)
    print('{1}. received: {0}'.format(data, i))
    i = i + 1
    # wait a random amount of time
    time.sleep(1) #machine.rng() & 0x0F)
