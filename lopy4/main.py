import ujson
import socket
import machine
import time
import pycom

from network import LoRa
from machine import RTC
from pytrack import Pytrack
from L76GNSV4 import L76GNSS

print("Starting Legter-LoRa")

rtc = RTC()
rtc.init((2020,12,14,23,57,0))

lora = LoRa(
    mode=LoRa.LORA,
    region=LoRa.EU868,
    frequency=868000000,
    bandwidth=LoRa.BW_125KHZ,
    sf=7,
    public=False)

s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)

py = Pytrack()

L76 = L76GNSS(pytrack=py)
L76.setAlwaysOn()
print("heartbeat off")
pycom.heartbeat(0)
print("start while")
i = 0
BRODCAST_ID = '255255255255'
while (1):
    pycom.rgbled(0x330000)
    print("getting GGA")
    gga_msg = L76.gps_message('GGA',debug=False)

    pycom.rgbled(0x003333)
    id = i
    machine_hex = machine.unique_id()
    machine_id = ''.join(['{:02x}'.format(b) for b in machine_hex])
    timestamp = time.time()
    freetext = "greetings from {0}!".format(machine_id)
    msg = "{{'ID':{0},'FROM':{1},'LAT':{2},'LON':{3},'ALT':{4},'FREETEXT':'{5}','TIMESTAMP':{6}}}".format(
        id,
        machine_id,
        gga_msg['Latitude'],
        gga_msg['Longitude'],
        gga_msg['Altitude'],
        freetext,
        timestamp
    )
    s.setblocking(True)
    t = rtc.now()
    s.send(msg)
    pycom.rgbled(0x003300)
    print('sent: {0}'.format(msg))

    pycom.rgbled(0x001100)

    s.setblocking(False)
    new_message = False
    data = s.recv(1024)
    if data:
        pycom.rgbled(0x003300)
        try:
            request = ujson.loads(data)
            print(request)
            if (request['to'] == id_str) or (request['to'] == BRODCAST_ID):
                new_message = True
                print('\t***New message: ', request['message'])
                s.setblocking(True)
                s.send("ACK")
        except:
            if new_message:
                print('malformed JSON string')
                s.setblocking(True)
                s.send("NOK")
    i = i +1

    pycom.rgbled(0x001100)
    time.sleep(5)
