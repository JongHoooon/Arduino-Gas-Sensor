from matplotlib.pylab import *
from mpl_toolkits.axes_grid1 import host_subplot
from matplotlib import animation
import numpy as np
import serial


###### configure arduino #####

serial_port = '/dev/tty.usbserial-1140'
serial_baudrate = 9600

seri = serial.Serial(serial_port, serial_baudrate)

##### configure graph #####

# sent for figure

font = {'size': 9}
rc('font', **font)

# setup figure and subplots
f0 = figure(num = 0, figsize = (12, 12))
f0.suptitle("Off Gas", fontsize = 12)

ax01 = subplot2grid((2, 2), (0, 0))
ax02 = subplot2grid((2, 2), (0, 1))
ax03 = subplot2grid((2, 2), (1, 0))

# set title
ax01.set_title("first MQ9's LPG, CH4, CO")
ax02.set_title("first MQ9's CO")
ax03.set_title("second MQ9's CO")

# set limites
ax01.set_xlim(0, 60.0)
ax01.set_ylim(0, 50.0)

ax02.set_xlim(0, 60.0)
ax02.set_ylim(0, 50.0)

ax03.set_xlim(0, 60.0)
ax03.set_ylim(0, 50.0)

# Turn on grids
ax01.grid(True)
ax02.grid(True)
ax03.grid(True)

# set label names
ax01.set_xlabel("s")
ax01.set_ylabel("ppm")

ax02.set_xlabel("s")
ax02.set_ylabel("ppm")

ax03.set_xlabel("s")
ax03.set_ylabel("ppm")

# Data Placeholders
lpg_1 = zeros(0)
ch4_1 = zeros(0)
co_1_1 = zeros(0)

co_1_2 = zeros(0)

co_2 = zeros(0)

t = zeros(0)

# set plots
p011, = ax01.plot(t, lpg_1, "b-", label = "LPG")
p012, = ax01.plot(t, ch4_1, "g-", label = "CH4")
p013, = ax01.plot(t, co_1_1, "r-", label = "CO")

p021, = ax02.plot(t, co_1_2, "r-", label = "CO")

p031, = ax03.plot(t, co_2, "r-", label = "CO")

ax01.legend([p011, p012, p013], 
            [p011.get_label(), p012.get_label(), p013.get_label()])
ax02.legend([p021], 
            [p021.get_label()])
ax03.legend([p031], 
            [p031.get_label()])

# Data Update
xmin = 0.0
xmax = 60.0
x = 0.0


def updateDate(self):
    global x
    global lpg_1
    global ch4_1
    global co_1_1
    global co_1_2
    global co_2
    global t

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

                lpg_1 = append(lpg_1, res[0])
                ch4_1 = append(ch4_1, res[1])
                co_1_1 = append(co_1_1, res[2])
                
                co_1_2 = append(co_1_2, res[2])
                co_2 = append(co_2, res[3])
                t = append(t, x)

                x += 0.5

                p011.set_data(t, lpg_1)
                p012.set_data(t, ch4_1)
                p013.set_data(t, co_1_1)

                p021.set_data(t, co_1_2)
                
                p031.set_data(t, co_2)
                
                ax01.set_title(f"first MQ9\nLPG: {lpg_1[-1]}, CH4: {ch4_1[-1]}, CO: {co_1_1[-1]}")
                ax02.set_title(f"first MQ9\nCO: {co_1_2[-1]}")
                ax03.set_title(f"second MQ9\nCO: {co_2[-1]}")
                
                
                if x >= xmax-10.00:
                    p011.axes.set_xlim(x-xmax+10.0, x+10.0)
                    p012.axes.set_xlim(x-xmax+10.0, x+10.0)
                    p013.axes.set_xlim(x-xmax+10.0, x+10.0)

                    p021.axes.set_xlim(x-xmax+10.0, x+10.0)
                    
                    p031.axes.set_xlim(x-xmax+10.0, x+10.0)
                
                return p011, p012, p013, p021, p031

    except Exception as ex:
        print("error")
        print(ex)
        pass
    return -1



simulation = animation.FuncAnimation(f0, 
                                     updateDate, 
                                     blit = False,
                                     interval = 500,
                                     repeat = False)

plt.show()





""""
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
        print("error")
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
"""
