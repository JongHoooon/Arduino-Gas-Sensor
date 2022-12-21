from matplotlib import pyplot as plt
from matplotlib import animation
import numpy as np
import serial


###########################################################
serial_port = '/dev/tty.usbserial-1130'
serial_baudrate = 9600

seri = serial.Serial(serial_port, serial_baudrate)

fig = plt.figure()
ax = plt.axes(xlim = (0, 1000), ylim = (0, 50))

plt.errorbar(x, y + 1, yerr=yerr, label = "co 1")
plt.errorbar(x, y, yerr=yerr, label = "co 2") 

plt.errorbar()


max_points = 1000
line, = ax.plot(np.arange(max_points),
                np.ones(max_points, dtype = np.float64) * np.nan,
                lw = 2)

def getData():
    try:
        if seri.readable():
            ret = seri.readline()
            ret = ret.decode()[: len(ret) - 1]
            if ret.find("gasData") > -1:
                ret = ret.replace(" ", "")
                ret = ret.replace("\r", "")
                res = ret.split(":")[1]
                res = res.split(",")
                res = list(map(float, res))
                print(res)
                return float(res[2])
    except Exception as ex:
        print(ex)
        pass
    return -1


def animate(i):
    y = float(getData())
    if(y > -1): 
        old_y = line.get_ydata()
        new_y = np.r_[old_y[1:], y]
        line.set_ydata(new_y)
    return line,
    
anim = animation.FuncAnimation(fig,
                               animate,
                               init_func=lambda : line, 
                               interval = 500)

plt.show()