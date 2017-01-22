# Example of low level interaction with a BLE UART device that has an RX and TX
# characteristic for receiving and sending data.  This doesn't use any service
# implementation and instead just manipulates the services and characteristics
# on a device.  See the uart_service.py example for a simpler UART service
# example that uses a high level service implementation.
# Author: Tony DiCola
import logging
# logging.getLogger('requests').setLevel(logging.WARNING)
# logging.basicConfig(level=logging.DEBUG)

import time
import uuid

import airpen


import Adafruit_BluefruitLE


# Enable debug output.
#logging.basicConfig(level=logging.DEBUG)

# Define service and characteristic UUIDs used by the UART service.
ACCEL_SERVICE_UUID = uuid.UUID('87de0001-51b5-43c3-9ccb-993004dd54aa')
ACCEL_DATA_CHAR_UUID = uuid.UUID('87de0002-51b5-43c3-9ccb-993004dd54aa')
BTN_CHAR_UUID = uuid.UUID('87de0003-51b5-43c3-9ccb-993004dd54aa')
# Get the BLE provider for the current platform.
ble = Adafruit_BluefruitLE.get_provider()

data_interval_secs = [0.05, 0.05, 0.05]
current_v = [0, 0, 0]
current_pos = [0, 0, 0]

initialized = False

counter = 0
accel_buffer = []
current_pos_mm = [0, 0, 0]
current_pos_cm = [0, 0, 0]

def vec_add(v1, v2):
    return [x1 + x2 for (x1, x2) in zip(v1, v2)]

def vec_subtract(v1, v2):
    return [x1 - x2 for (x1, x2) in zip(v1, v2)]

def vec_multiply(v1, v2):
    return [x1 * x2 for (x1, x2) in zip(v1, v2)]

def vec_divide(v1, v2):
    return [x1 / x2 for (x1, x2) in zip(v1, v2)]




def integrate_accel_pos(accel_buffer):
    global current_v
    global current_pos
    global current_pos_mm
    global current_pos_cm
    da1 = vec_subtract(convert_accel_to_std_units(accel_buffer[1]), convert_accel_to_std_units(accel_buffer[0]))
    da2 = vec_subtract(convert_accel_to_std_units(accel_buffer[2]), convert_accel_to_std_units(accel_buffer[1]))
    v1 = vec_add(vec_multiply(da1, data_interval_secs), current_v)
    v2 = vec_add(vec_multiply(da2, data_interval_secs), v1)
    current_pos = vec_add(vec_multiply(vec_subtract(v2, v1), data_interval_secs), current_pos)
    current_v = v2
    # print "current_pos "+str(current_pos[0])
    # print current_pos
    current_pos_mm = vec_multiply(current_pos, [1000, 1000, 1000])
    # print current_pos_mm
    # print "current_pos_mm "+str(current_pos_mm[0])
    current_pos_cm = vec_multiply(current_pos, [100, 100, 100])
    # print "current_pos_cm "+str(current_pos_cm[0])

    

#convert to m/s^2
def convert_accel_to_std_units(accel_raw):
    v = vec_multiply(vec_multiply(accel_raw, [0.001, 0.001, 0.001]), [9.8, 9.8, 9.8])
    # print v
    return v


def correct_gravity(accel_std):
    a = [0, 0, 9.8]
    return vec_subtract(accel_raw, a)


# class Namespace(BaseNamespace):

#     def on_aaa_response(self, *args):
#         print('on_aaa_response', args)
#         self.emit('bbb')


# Main function implements the program logic so it can run in a background
# thread.  Most platforms require the main thread to handle GUI events and other
# asyncronous events like BLE actions.  All of the threading logic is taken care
# of automatically though and you just need to provide a main function that uses
# the BLE provider.
def main():
    print 'started...'
    # Clear any cached data because both bluez and CoreBluetooth have issues with
    # caching data and it going stale.
    ble.clear_cached_data()

    # Get the first available BLE network adapter and make sure it's powered on.
    adapter = ble.get_default_adapter()
    adapter.power_on()
    print('Using adapter: {0}'.format(adapter.name))

    # Disconnect any currently connected UART devices.  Good for cleaning up and
    # starting from a fresh state.
    print('Disconnecting any connected UART devices...')
    ble.disconnect_devices([ACCEL_SERVICE_UUID])

    # Scan for UART devices.
    print('Searching for AirPenn device...')
    try:
        adapter.start_scan()
        # Search for the first UART device found (will time out after 60 seconds
        # but you can specify an optional timeout_sec parameter to change it).
        device = ble.find_device(service_uuids=[ACCEL_SERVICE_UUID])
        if device is None:
            raise RuntimeError('Failed to find UART device!')
    finally:
        # Make sure scanning is stopped before exiting.
        adapter.stop_scan()

    print('Connecting to device...')
    device.connect()  # Will time out after 60 seconds, specify timeout_sec parameter
                      # to change the timeout.

    # Once connected do everything else in a try/finally to make sure the device
    # is disconnected when done.
    
    # Wait for service discovery to complete for at least the specified
    # service and characteristic UUID lists.  Will time out after 60 seconds
    # (specify timeout_sec parameter to override).
    print('Discovering services...')
    device.discover([ACCEL_SERVICE_UUID], [ACCEL_DATA_CHAR_UUID])

    # Find the UART service and its characteristics.
    accel_service = device.find_service(ACCEL_SERVICE_UUID)
    data_char = accel_service.find_characteristic(ACCEL_DATA_CHAR_UUID)
    btn_char = accel_service.find_characteristic(BTN_CHAR_UUID)

    # Write a string to the TX characteristic.
    # print('Sending message to device...')
    # tx.write_value('Hello world!\r\n')

    # Function to receive RX characteristic changes.  Note that this will
    # be called on a different thread so be careful to make sure state that
    # the function changes is thread safe.  Use Queue or other thread-safe
    # primitives to send data to other threads.
    def received_accel(data):
        x, y, z = airpen.airpen(data)
        global counter
        global accel_buffer
        global current_v
        global initialized
        global current_pos
        global current_pos_cm
        global current_pos_mm

        if initialized == False:
            print 'Not Initialized'
            if counter < 3:
                print 'counter < 3'
                accel_buffer.append([x, y, z])
                counter += 1
            else:
                print 'counter > 3 initialized'
                integrate_accel_pos(accel_buffer)
                initialized = True
        else:
            print 'initialized'
            # print accel_buffer
            accel_buffer.pop(0)
            accel_buffer.append([x, y, z])
            integrate_accel_pos(accel_buffer)

        with open('../../static/data.json', 'w') as some_file:
            print current_pos_mm[0]
            some_file.write('{"x":"'+str(current_pos[0]*1000)+'", "y":"'+str(current_pos[1]*1000)+'", "z":"'+str(current_pos[2]*1000)+'"}')

        # print str(accel_buffer[0][0])

        # print '{"x":"'+str(x)+'", "y":"'+str(y)+'", "z":"'+str(z)+'"} {"x":"'+str(current_v[0])+'", "y":"'+str(current_v[1])+'", "z":"'+str(current_v[2])+'"} {"x":"'+str(current_pos_cm[0])+'", "y":"'+str(current_pos_cm[1])+'", "z":"'+str(current_pos_cm[2])+'"}'

        

        # with SocketIO('localhost', 80, LoggingNamespace) as socketIO:
        #     print 'socket instance'
        #     # ss = socketIO.define(Namespace, '/')
        #     # print 'ss instance'
        #     socketIO.emit('values', x)
        
        # with open('bytes.txt', 'a') as txt:
        #     txt.write(str(.getvalue()))
        # print('Received: {0}'.format(data))

    def received_btn(data):
        btn_id, state = airpen.airpen(data)
        with open('../../static/btn.json', 'w') as some_file:
            some_file.write('{"id":"'+str(btn_id)+'", "state":"'+str(state)+'"}')
        # print '{"id":"'+str(btn_id)+'", "state":"'+str(state)+'"}'

    # Turn on notification of ACCEL characteristics using the callback above.
    print('Subscribing to ACCEL characteristic changes...')
    data_char.start_notify(received_accel)

    btn_char.start_notify(received_btn)

    # Now just wait for 30 seconds to receive data.
    print('Waiting 60 seconds to receive data from the device...')
    time.sleep(10000)

    # Make sure device is disconnected on exit.
    # device.disconnect()


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)

