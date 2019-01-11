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
import sqlite3
import datetime as dt
from time import time

from influxdb import InfluxDBClient
import dbconfig

client = InfluxDBClient(host=dbconfig.influx_host, port=dbconfig.influx_port)
client.switch_database(dbconfig.influx_db)

time_offset = 0

# dash by plotly component
external_css = ["https://cdnjs.cloudflare.com/ajax/libs/skeleton/2.0.4/skeleton.min.css",
                "https://fonts.googleapis.com/css?family=Raleway:400,400i,700,700i",
                "https://fonts.googleapis.com/css?family=Product+Sans:400,400i,700,700i"]
wanted = 'short'
if wanted == 'long':
    period_pair = {
        '1 hour': '1h',
        '12 hours': '12h',
        '1 day': '1d',
        '1 week': '7d',
        '1 month': '30d',
        '6 months': '180d',
        '1 year': '365d',
    }
else:
    period_pair = {
        '10 seconds': '10s',
        '30 seconds': '30s',
        '1 minute': '1m',
        '5 minute': '5m',
        '10 minute': '10m',
        '30 minute': '30m',
        '1 hour': '1h',
        '6 hour': '6h',
        '12 hour': '12h',
        '24 hour': '24h',
    }

mean_pair = {
    'No average': 'na',
    '10 minutes': '10m',
    '30 minutes': '30m',
    '1 hour': '1h',
}

save_pair = {
    'Yes': True,
    'No': False
}

app = dash.Dash(
    'streaming-wind-app',
    external_stylesheets=external_css
)
server = app.server

app.layout = html.Div([
    html.Div([
        html.H2("SPIRIT Lab monitoring")
        #html.Img(src="https://s3-us-west-1.amazonaws.com/plotly-tutorials/logo/new-branding/dash-logo-by-plotly-stripe-inverted.png"),
    ], className='banner'),
    html.Div([
        html.Div([
            html.H3("Live values from Arduino Mega 1")
        ], className='Title'),
        html.Div([
            "Period of interest:",
            dcc.RadioItems(
                id='period_rd',
                options=[{'label': i, 'value': i} for i in period_pair.keys()],
                value='30 seconds', #default value
                labelStyle={'display': 'inline-block'} 
            )
        ],
        style={'width': '48%', 'display': 'inline-block'}),
        html.Div([
            "Average value over:",
            dcc.RadioItems(
                id='mean_rd',
                options=[{'label': i, 'value': i} for i in mean_pair.keys()],
                value='No average', #default value
                labelStyle={'display': 'inline-block'} 
            )
        ],        
        style={'width': '48%', 'floating': 'right', 'display': 'inline-block'}),
        html.Div([
            "Save plotting: discarding not logical values",
            dcc.RadioItems(
                id='save_rd',
                options=[{'label': i, 'value': j} for i, j in save_pair.items()],
                value=True, #default value
                labelStyle={'display': 'inline-block'} 
            )
        ],        
        style={'width': '48%', 'display': 'inline-block'}),
        html.Div([
                html.Div([
                    dcc.Graph(id='humidity'),
                ], className='six columns'),
                html.Div([
                    dcc.Graph(id='temperature'),
                ], className='six columns')
            ], className="row"),
        html.Div([
            dcc.Interval(id='update', interval=1000, n_intervals=0)
            ], className='row')

    ])
])
@app.callback(Output('humidity', 'figure'), 
    [Input('update', 'n_intervals'),
    Input('period_rd', 'value'),
    Input('mean_rd', 'value'),
    Input('save_rd', 'value')])
def gen_humid_plot(interval, period, mean, save_plotting):
    return live_plot(interval, period, mean, save_plotting, 'humidity')

@app.callback(Output('temperature', 'figure'), 
    [Input('update', 'n_intervals'),
    Input('period_rd', 'value'),
    Input('mean_rd', 'value'),
    Input('save_rd', 'value')])
def gen_temp_plot(interval, period, mean, save_plotting):
    return live_plot(interval, period, mean, save_plotting, 'temperature')

def live_plot(interval, period, mean, save_plotting, interest):

    if period is None:
        plot_range = 'now() - 1d'
    else:
        plot_range = 'now() - {}'.format(period_pair.get(period))
    mean_range = mean_pair.get(mean, "1h")
    if mean_range == 'na':
        query_text = 'SELECT "time", "{}" FROM "megatest" WHERE "time" >= {} GROUP BY "user" tz(\'Singapore\')'.format(interest, plot_range)
    else:
        query_text = 'SELECT "time", mean("{}") as "{}" FROM "megatest" WHERE "time" >= {} GROUP BY "user", time({}) fill(none) tz(\'Singapore\')'.format(interest, interest, plot_range, mean_range)
    querys = client.query(query_text)
    #points = querys.get_points(tags={'user':'mega_1'})
    points = querys.get_points()
    xs = []
    ys = []

    for point in points:
        # if time_offset == 0:
        #     time_offset = int(point['time'])` 
        # current = int(point['time'])
        # xs.append((current - time_offset)/1000)

        # discarding not logical value
        xs.append(point['time'])
        ypoint = float(point[interest])
        if save_plotting:
            if ypoint < 0 or ypoint > 100:
                ys.append(None)
            else:
                ys.append(ypoint)
        else:
            ys.append(ypoint)
    #print("points are:"+points)
    #tp = np.array(points)
    trace = [
        Scatter(
            x=xs,
            y=ys,
            name=interest,
            line=Line(
                color='#42C4F7',
                shape='hvh'
            ),
##        error_y=ErrorY(
#            type='data',
#            #array=df['SpeedError'],
#            thickness=1.5,
#            width=2,
#            color='#B4E8FC'
#        ),
            mode='lines'
    )]

    layout = Layout(
        title='{} over {}'.format(interest, period)
    )

    return Figure(data=trace, layout=layout)

if __name__ == '__main__':
    # app.run_server(debug=True)
    app.run_server(host='0.0.0.0', port='8050')
