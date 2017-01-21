from flask import Flask, render_template, url_for, send_from_directory
from flask_socketio import SocketIO, emit
import logging
import time
import uuid
import airpen


app = Flask(__name__, static_url_path='')
app.config['SECRET_KEY'] = 'airpen'
socketio = SocketIO(app)


@app.route('/')
def index():
	return render_template('index.html')


@app.route('/<path:path>')
def jsfile(path):
    return url_for('static', filename=path)


@socketio.on('shouldStart')
def requestBluetooth(shouldStart):
	if shouldStart:
		print 'Drawing Started'
		emit('start')
		print 'running...'
	else:
		print 'Drawing Stopped'


if __name__ == '__main__':
    socketio.run(app)
