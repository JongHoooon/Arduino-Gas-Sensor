from matplotlib.pylab import *
from mpl_toolkits.axes_grid1 import host_subplot
from matplotlib import animation
from matplotlib.animation import writers
import numpy as np
import serial

import pandas as pd
import openpyxl


###### configure arduino #####

serial_port = '/dev/tty.usbserial-130'
serial_baudrate = 9600

seri = serial.Serial(serial_port, serial_baudrate)

##### configure graph #####

# sent for figure

font = {'size': 9}
rc('font', **font)

# setup figure and subplots
f0 = figure(num=0, figsize=(8, 6))
f0.suptitle("Off Gas", fontsize=12)

ax01 = subplot2grid((1, 2), (0, 0))
ax02 = subplot2grid((1, 2), (0, 1))
# ax03 = subplot2grid((1, 4), (0, 2))
# ax04 = subplot2grid((1, 4), (0, 3))

# set title
ax01.set_title("first MQ9's CH4, CO")
ax02.set_title("second MQ9's CH4, CO")
# ax03.set_title("third MQ9's CH4, CO")
# ax04.set_title("Temperature")


# set limites
ax01.set_xlim(0, 60.0)
ax01.set_ylim(0, 50.0)

ax02.set_xlim(0, 60.0)
ax02.set_ylim(0, 50.0)

# ax03.set_xlim(0, 60.0)
# ax03.set_ylim(0, 50.0)

# ax04.set_xlim(0, 60.0)
# ax04.set_ylim(0, 300.0)

# Turn on grids
ax01.grid(True)
ax02.grid(True)
# ax03.grid(True)
# ax04.grid(True)


# set label names
ax01.set_xlabel("s")
ax01.set_ylabel("ppm")

ax02.set_xlabel("s")
ax02.set_ylabel("ppm")

# ax03.set_xlabel("s")
# ax03.set_ylabel("ppm")

# ax04.set_xlabel("s")
# ax04.set_ylabel("°C")

# Data Placeholders
ch4_1 = zeros(0)
co_1 = zeros(0)

ch4_2 = zeros(0)
co_2 = zeros(0)

# ch4_3 = zeros(0)
# co_3 = zeros(0)

# temp = zeros(0)

t = zeros(0)

# set plots
p011, = ax01.plot(t, ch4_1, "g-", label="CH4")
p012, = ax01.plot(t, co_1, "r-", label="CO")

p021, = ax02.plot(t, ch4_2, "g-", label="CH4")
p022, = ax02.plot(t, co_2, "r-", label="CO")

# p031, = ax03.plot(t, ch4_3, "g-", label="CH4")
# p032, = ax03.plot(t, co_3, "r-", label="CO")

# p04, = ax04.plot(t, temp, "-r", label="temp")

ax01.legend([p011, p012],
            [p011.get_label(), p012.get_label()])
ax02.legend([p021, p022],
            [p021.get_label(), p022.get_label()])
# ax03.legend([p031, p032], [p031.get_label(), p032.get_label()])
# ax04.legend([p04], [p04.get_label()])

# Data Update
xmin = 0.0
xmax = 60
x = 0.0

df = pd.DataFrame([[0, 0, 0, 0]],
                  index=[0],
                  columns=["CH4_1", "CO_1",
                           "CH4_2", "CO_2"])


def updateDate(self):
    global x
    global ch4_1
    global co_1
    global ch4_2
    global co_2
    # global ch4_3
    # global co_3
    # global temp
    global t

    global df

    try:
        if seri.readable():
            ret = seri.readline()
            ret = ret.decode()[: len(ret) - 1]
            if ret.find("gasData") > -1:
                ret = ret.replace(" ", "")
                ret = ret.replace("\r", "")
                res = ret.split(":")[1]
                res = res.split(",")
                # print(res)
                res = list(map(double, res))
                print(res)

                ch4_1 = append(ch4_1, res[0])
                co_1 = append(co_1, res[1])

                ch4_2 = append(ch4_2, res[2])
                co_2 = append(co_2, res[3])

                # ch4_3 = append(ch4_3, res[4])
                # co_3 = append(co_3, res[5])
                
                # temp = append(temp, res[6])

                t = append(t, x)

                x += 0.5

                p011.set_data(t, ch4_1)
                p012.set_data(t, co_1)

                p021.set_data(t, ch4_2)
                p022.set_data(t, co_2)

                # p031.set_data(t, ch4_3)
                # p032.set_data(t, co_3)
                
                # p04.set_data(t, temp)

                ax01.set_title(f"first MQ9\nCH4: {ch4_1[-1]}, CO: {co_1[-1]}")
                ax02.set_title(f"second MQ9\nCH4: {ch4_2[-1]}, CO: {co_2[-1]}")
                # ax03.set_title(f"third MQ9\nCH4: {ch4_3[-1]}, CO: {co_3[-1]}")
                # ax04.set_title(f"temperature\n{temp[-1]} °C")

                append_df = pd.DataFrame(data=[res],
                                         index=[x],
                                         columns=["CH4_1", "CO_1",
                                                  "CH4_2", "CO_2"])

                df = pd.concat([df, append_df])

                df.to_excel(
                    "/Users/bromp/Desktop/study/Arduino-Gas-Sensor/arduino_gas/Off-Gas.xlsx")

                if x >= xmax-10.00:
                    p011.axes.set_xlim(x-xmax+10.0, x+10.0)
                    p012.axes.set_xlim(x-xmax+10.0, x+10.0)

                    p021.axes.set_xlim(x-xmax+10.0, x+10.0)
                    p022.axes.set_xlim(x-xmax+10.0, x+10.0)

                    # p031.axes.set_xlim(x-xmax+10.0, x+10.0)
                    # p032.axes.set_xlim(x-xmax+10.0, x+10.0)
                    
                    # p04.axes.set_xlim(x-xmax+10.0, x+10.0)
                    
                # if co_1[-1] > ymax - 10.00:

                return p011, p012, p021, p022 #, p031, p032, p04

    except Exception as ex:
        print("error")
        print(ex)
        pass
    return -1


simulation = animation.FuncAnimation(f0,
                                     updateDate,
                                     blit=False,
                                     interval=500,
                                     repeat=False)

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

