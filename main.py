# This is the main flask server script that used to listening for POST then save data into influxdb

from flask import Flask
from flask import render_template
from flask import request
from flask import Response

from influxdb import InfluxDBClient
import dbconfig

import time

app = Flask(__name__)

client = InfluxDBClient(host=dbconfig.influx_host, port=dbconfig.influx_port)
client.switch_database(dbconfig.influx_db)

data_dict = {
    "measurement": "megatest",
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
    return 'hello'

@app.route("/api/post", methods=['GET', 'POST'])
def get_post():
    val = request.form.get('maindata')
    print ("POST received {}".format(val))
    if val is not None:
        data_dict['fields']['temperature'] = float(val)
        client.write_points([data_dict])
        return_text = 'success'
        resp = Response(return_text, status=200, mimetype='text/plain')
        return resp
    else:
        return_text = 'fail'
        resp = Response(return_text, status=-10, mimetype='text/plain')
        return resp


if __name__ == '__main__':
    # app.run(port=5000, debug=True)
    app.run(host='0.0.0.0', port=5000)