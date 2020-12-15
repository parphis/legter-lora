from network import LoRa
import socket
import machine
import time
from machine import RTC

rtc = RTC()
rtc.init((2020,12,14,23,57,0))


# initialise LoRa in LORA mode
# Please pick the region that matches where you are using the device:
# Asia = LoRa.AS923
# Australia = LoRa.AU915
# Europe = LoRa.EU868
# United States = LoRa.US915
# more params can also be given, like frequency, tx power and spreading factor
lora = LoRa(
    mode=LoRa.LORA,
    region=LoRa.EU868,
    frequency=868000000,
    bandwidth=LoRa.BW_125KHZ,
    sf=7,
    public=False)
#for i in range(3, 16):
    #lora.remove_channel(i)
#for i in range (0, 2):
#    lora.add_channel(i, frequency=868000000, dr_min=3, dr_max=3)
# create a raw LoRa socket
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
i = 0
while True:
    # send some data
    s.setblocking(True)
    t = rtc.now()
    # s.send('checks from bendi-laptop at {0}-{1}-{2} {3}:{4}:{5}'.format(t[0],t[1],t[2],t[3],t[4],t[5],t[6]))
    s.send('hello')
    print('sent: {0}-{1}-{2} {3}:{4}:{5}'.format(t[0],t[1],t[2],t[3],t[4],t[5],t[6]))

    # get any data received...
    #s.setblocking(False)
    #data = s.recv(64)
    #print('{1}. received: {0}'.format(data, i))
    i = i + 1
    # wait a random amount of time
    time.sleep(1) #machine.rng() & 0x0F)
