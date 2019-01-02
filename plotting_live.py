from influxdb import InfluxDBClient
import dbconfig

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style

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
    querys = client.query('SELECT "time", "temperature" FROM "iottest" GROUP BY "user"', epoch='ms')
    points = querys.get_points(tags={'user':'mega_1'})
    xs = []
    ys = []

    for point in points:
        if time_offset == 0:
            time_offset = int(point['time'])
        current = int(point['time'])
        xs.append((current - time_offset)/1000)
        ys.append(point['temperature'])

    if len(xs) == 0:
        time_offset = 0
    ax1.clear()
    current = float((current - time_offset)/1000)
    plt.xlim(current -5, current)
    plt.ylim(-2,2)
    ax1.plot(xs, ys)

ani = animation.FuncAnimation(fig, animate, interval = 100)
plt.show()
