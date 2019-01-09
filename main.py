# This is the main flask server script that used to listening for POST then save data into influxdb

from flask import Flask
from flask import render_template
from flask import request
from flask import Response


from influxdb import InfluxDBClient
import dbconfig

import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State, Event
import plotly.plotly as py
from plotly.graph_objs import *
from scipy.stats import rayleigh
from flask import Flask
import numpy as np
import pandas as pd
import os
import datetime as dt

external_css = ["https://cdnjs.cloudflare.com/ajax/libs/skeleton/2.0.4/skeleton.min.css",
                "https://fonts.googleapis.com/css?family=Raleway:400,400i,700,700i",
                "https://fonts.googleapis.com/css?family=Product+Sans:400,400i,700,700i"]


import time

#app = Flask(__name__)

app = dash.Dash(
    'streaming-wind-app',
    external_stylesheets=external_css
)

client = InfluxDBClient(host=dbconfig.influx_host, port=dbconfig.influx_port)



#check if database already exist, if not create it
list_databases = client.get_list_database()
databases = []
for database in list_databases:
    for value in database.values():
        databases.append(value)
if dbconfig.influx_db not in databases:
    client.create_database(dbconfig.influx_db)
client.switch_database(dbconfig.influx_db)

data_dict = {
    "measurement": "megatest",
    "tags": {
        "user": "mega_1",
        "serial": "somehash_sssss",
        "location": "SPIRIT_lab"
    },
    "fields": {
        "temperature": "",
        "humidity": ""
    }
}

@app.route("/")
def web_print():
    return 'hello'

@app.route("/api/post", methods=['GET', 'POST'])
def get_post():
    val_tem = request.form.get('temperature')
    val_hmd = request.form.get('humidity')
    print ("POST received temperature {}".format(val_tem))
    print ("POST received humidity {}".format(val_hmd))

    if val_tem is not None:
        data_dict['fields']['temperature'] = float(val_tem)
       # data_dict['fields']['humidity'] = float(val_hmd)
        client.write_points([data_dict])
        return_text = 'success'
        resp = Response(return_text, status=200, mimetype='text/plain')
        return resp

#    elif val_hmd is not None:
#        data_dict['fields']['humidity'] = float(val_hmd)
#        client.write_points([data_dict])
#        return_text = 'success'
#        resp = Response(return_text, status=200, mimetype='text/plain')
#        return resp

    else:
        return_text = 'fail'
        resp = Response(return_text, status=-10, mimetype='text/plain')
        return resp

#def get_post_hmd():
    #val_hmd = request.form.get('humidity')
    #print ("POST received humidity {}".format(val_hmd))
#    elif val_hmd is not None:
#        data_dict['fields']['humidity'] = float(val_hmd)
#        client.write_points([data_dict])
#        return_text = 'success'
#        resp = Response(return_text, status=200, mimetype='text/plain')
#        return resp
#    else:
#        return_text = 'fail'
#        resp = Response(return_text, status=-10, mimetype='text/plain')
#        return resp


@app.route("/api/hmd", methods=['GET', 'POST'])
def get_hmd():
    val_hmd = request.form.get('humidity')
    print ("POST received humidity {}".format(val_hmd))

    if val_hmd is not None:
        data_dict['fields']['humidity'] = float(val_hmd)
        client.write_points([data_dict])
        return_text = 'success'
        resp = Response(return_text, status=200, mimetype='text/plain')
        return resp

#    elif val_hmd is not None:
#        data_dict['fields']['humidity'] = float(val_hmd)
#        client.write_points([data_dict])
#        return_text = 'success'
#        resp = Response(return_text, status=200, mimetype='text/plain')
#        return resp

    else:
        return_text = 'fail'
        resp = Response(return_text, status=-10, mimetype='text/plain')
        return resp


if __name__ == '__main__':
    # app.run(port=5000, debug=True)
    app.run(host='0.0.0.0', port=5000)
