from influxdb import InfluxDBClient
import dbconfig

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style

import numpy as np

import datetime

client = InfluxDBClient(host=dbconfig.influx_host, port=dbconfig.influx_port)
client.switch_database(dbconfig.influx_db)

style.use('fivethirtyeight')

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

time_offset = 0

def animate(i):
    global time_offset
    current = 0
    # querys = client.query('SELECT "time", "temperature" FROM "iottest" GROUP BY "user"', epoch='ms')
    querys = client.query('SELECT "time", "temperature" FROM "megatest" GROUP BY "user"', epoch='ms')
    points = querys.get_points(tags={'user':'mega_1'})
    del querys
    points_list = list(points)
    del points
    numberPoint = len(points_list)
    xs = np.zeros((1,numberPoint))
    ys = np.zeros((1,numberPoint))


    ii = 0
    for point in points_list:
        if time_offset == 0:
            time_offset = int(point['time'])
        current = int(point['time'])
        xs[0,ii] = (current - time_offset)/1000
        ys[0,ii] = point['temperature']
        ii+=1
    if len(xs[0,:]) == 0:
        time_offset = 0
    ax1.clear()
    current = float((current - time_offset)/1000)

    # discarding value outside out 5 second earlier than current
    acpt_Idx = (xs > (current - 5))
    xs = xs[acpt_Idx]
    ys = ys[acpt_Idx]
    ax1.plot(xs, ys)

ani = animation.FuncAnimation(fig, animate, interval = 100)
plt.show()
