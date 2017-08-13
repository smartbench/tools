#!/usr/bin/python3

#>>> PYFTDI
# Requires pyftdi:
# installation process extracted from http://eblot.github.io/pyftdi/installation.html
# requisites:
#   pyusb >= 1.0.0
#   pyserial >= 3.0
# repo way:
#   sudo pip3 install pyftdi
# from source:
#   sudo pip3 install pyusb
#   sudo pip3 install pyserial
#   git clone https://github.com/eblot/pyftdi.git
#   cd pyftdi
#   python3 setup.py build
#   sudo python3 setup.py install
#
#
#
#>>>  DRIVERS
# It's necessary to manually unload these drivers:
#   sudo rmmod ftdi_sio
#   sudo rmmod usbserial
#
#

from pyftdi.ftdi import Ftdi
import codecs
import sys
import time

if __name__ == '__main__' :
    ft = Ftdi()
    ft.open(vendor=0x0403,product=0x6010,interface=2)
    i = 0
    rd = 0
    while True:
        #i = (i*2)%256
        time.sleep(0.2)
        i = (i + 1) % 256
        if (i==0): i=1
        print("write: " + str( ft.write_data(bytes([i])) ) , i )
        time.sleep(0.3)
        rd = ft.read_data(10)
        #num = int(rd.encode('hex'), 16)
        #num = struct.unpack(">L", rd)[0]
        for j in range(len(rd)):
            num = int(rd[j])
            print("read: " + repr(num) + "\t" + str(rd))
end
