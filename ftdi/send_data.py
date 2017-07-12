#!/usr/bin/python3

from pyftdi.ftdi import Ftdi
import time

if __name__ == '__main__' :
    ft = Ftdi()
    ft.open(vendor=0x0403,product=0x6010,interface=2)
    i = 1
    while True:
        i = (i*2)%256
        if (i==0): i=1
        print(ft.write_data(bytes([i])),i)
        time.sleep(0.5)
end
