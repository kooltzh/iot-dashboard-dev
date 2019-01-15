# IoT Dashboard Development
## Overview
This repository is setup for collaboration for IoT Dashboard project

This project is tested running on Ubuntu 18.04 LTS using anaconda as package manager

## Dependency
Here are the list of dependency of this project

For flask backend server,

* [InfluxDB 5.2.1](https://pypi.org/project/influxdb/) (installed using pip)  
* Flask 1.0.2

For live plotting of graph,  

* matplotlib 2.2.3

## Installation

* Install influxdb by [link](https://portal.influxdata.com/downloads/)
* Run the influxdb service by typing `influxd`
* Make sure that influxdb is running by entering the influx CLI by typing `influx`
* Clone or download this repository
* Update the dbconfig.py using your own influxdb username, password, database
* run `python main.py`
