from flask import Flask, render_template, url_for, send_from_directory
from flask_socketio import SocketIO, emit
import logging
import time
import uuid
import airpen


app = Flask(__name__, static_url_path='')
app.config['SECRET_KEY'] = 'airpen'
socketio = SocketIO(app)
celery_thread = None

ACCEL_SERVICE_UUID = uuid.UUID('87de0001-51b5-43c3-9ccb-993004dd54aa')
ACCEL_DATA_CHAR = uuid.UUID('87de0002-51b5-43c3-9ccb-993004dd54aa')

# def main():
# 	print 'started...'
# 	ble.clear_cached_data()

# 	print 'cleared'

# 	adapter = ble.get_default_adapter()

# 	print 'adapter found'

# 	adapter.power_on()
# 	print 'Using adapter: {0}'.format(adapter.name)

# 	print 'Disconnecting any connected UART devices...' 

# 	ble.disconnect_devices([ACCEL_SERVICE_UUID])

# 	print 'Searching for AirPenn device...'
# 	try:
# 	    adapter.start_scan()
# 	    device = ble.find_device(service_uuids=[ACCEL_SERVICE_UUID])
# 	    if device is None:
# 	        raise RuntimeError('Failed to find UART device!')
# 	finally:
# 	    adapter.stop_scan()

# 	print 'Connecting to decvice...'
# 	device.connect() 

# 	print 'Discovering services...'
# 	device.discover([ACCEL_SERVICE_UUID], [ACCEL_DATA_CHAR])

# 	accel = device.find_service(ACCEL_SERVICE_UUID)
# 	data = accel.find_characteristic(ACCEL_DATA_CHAR)

# 	def received(data):
# 		x, y, z = airpen.airpen(data)
# 		print x
# 		emit('values', str(x))

# 	print 'Subscribing to ACCEL characteristic changes...'
# 	data.start_notify(received)

# 	print 'Waiting 60 seconds to receive data from the device...'
# 	time.sleep(60)

# 	# device.disconnect()


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
