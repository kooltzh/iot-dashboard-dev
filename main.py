# This is the main flask server script that used to listening for POST then save data into influxdb

from flask import Flask
from flask import render_template
from flask import request
from flask import Response


from influxdb import InfluxDBClient
import dbconfig


app = Flask(__name__)

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
        "humidity": "",
        "pir":"",
        "soundDB":"",
        "co":""
    }
}

@app.route("/")
def web_print():
    return 'hello'

@app.route("/api/post", methods=['GET', 'POST'])
def get_post():
    raw_data = request.get_data().decode('utf8')
    val_tem = request.form.get('temperature')
    print("POST received raw \"{}\"".format(raw_data))
    print("POST received temperature {}".format(val_tem))
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
    raw_data = request.get_data().decode('utf8')
    val_hmd = request.form.get('humidity')
    print("POST received raw \"{}\"".format(raw_data))
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

@app.route("/api/pir", methods=['GET', 'POST'])
def get_pir():
    raw_data = request.get_data().decode('utf8')
    val_pir = request.form.get('pir')
    print("POST received raw \"{}\"".format(raw_data))
    print ("POST received pir_status {}".format(val_pir))

    if val_pir is not None:
        data_dict['fields']['pir'] = float(val_pir)
        client.write_points([data_dict])
        return_text = 'success'
        resp = Response(return_text, status=200, mimetype='text/plain')
        return resp

    else:
        return_text = 'fail'
        resp = Response(return_text, status=-10, mimetype='text/plain')
        return resp

@app.route("/api/sound", methods=['GET', 'POST'])
def get_sound():
    raw_data = request.get_data().decode('utf8')
    val_sound = request.form.get('soundDB')
    print("POST received raw \"{}\"".format(raw_data))
    print ("POST received soundDB {}".format(val_sound))

    if val_sound is not None:
        data_dict['fields']['soundDB'] = float(val_sound)
        client.write_points([data_dict])
        return_text = 'success'
        resp = Response(return_text, status=200, mimetype='text/plain')
        return resp

    else:
        return_text = 'fail'
        resp = Response(return_text, status=-10, mimetype='text/plain')
        return resp
        
@app.route("/api/co", methods=['GET', 'POST'])
def get_co():
    raw_data = request.get_data().decode('utf8')
    val_co = request.form.get('co')
    print("POST received raw \"{}\"".format(raw_data))
    print ("POST received co {}".format(val_co))

    if val_co is not None:
        data_dict['fields']['co'] = float(val_co)
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
