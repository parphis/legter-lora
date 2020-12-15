from network import WLAN
import machine
wlan = WLAN(mode=WLAN.STA)

wlan.connect(ssid='vig2', auth=(WLAN.WPA2, '09778603'))
while not wlan.isconnected():
    machine.idle()
print("WiFi connected succesfully")
print(wlan.ifconfig())
