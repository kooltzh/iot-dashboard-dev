from flask import Flask
from flask import render_template
from flask import request

from influxdb import InfluxDBClient
import dbconfig

import time

app = Flask(__name__)

client = InfluxDBClient(host=dbconfig.influx_host, port=dbconfig.influx_port)
client.switch_database(dbconfig.influx_db)

data_dict = {
    "measurement": "iottest",
    "tags": {
        "user": "mega_1",
        "serial": "somehash_sssss",
        "location": "SPIRIT_lab"
    },
    "fields": {
        "temperature": ""
    }
}

@app.route("/")
def web_print():
    print('hello')

@app.route("/api/post", methods=['GET', 'POST'])
def get_post():
    val = request.form.get('maindata')
    print ("POST received {}".format(val))
    if val is not None:
        data_dict['fields']['temperature'] = float(val)
        client.write_points([data_dict])
        return 'success'
    else:
        return 'fail'


if __name__ == '__main__':
    app.run(port=5000, debug=True)