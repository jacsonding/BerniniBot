import socketio
import eventlet
from flask import Flask, render_template, url_for, send_from_directory
import airpen
from subprocess import call

sio = socketio.Server()
app = Flask(__name__, static_url_path='')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/<path:path>')
def jsfile(path):
    return url_for('static', filename=path)

@sio.on('connect')
def connect(sid, environ):
    print('connect ', sid)

@sio.on('my message')
def message(sid, data):
    print('message ', data)

@sio.on('disconnect')
def disconnect(sid):
    print('disconnect ', sid)

@sio.on('shouldStart')
def shouldStart(sid, shouldStart):
	if shouldStart:
		print 'starting'
		call(["python", "Adafruit_Python_BluefruitLE/examples/low_level.py"])
	else:
		print 'disconnected'

if __name__ == '__main__':
    # wrap Flask application with socketio's middleware
    app = socketio.Middleware(sio, app)
    # deploy as an eventlet WSGI server
    eventlet.wsgi.server(eventlet.listen(('', 80)), app)
