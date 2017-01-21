# Example of low level interaction with a BLE UART device that has an RX and TX
# characteristic for receiving and sending data.  This doesn't use any service
# implementation and instead just manipulates the services and characteristics
# on a device.  See the uart_service.py example for a simpler UART service
# example that uses a high level service implementation.
# Author: Tony DiCola
import logging
import time
import uuid

from socketIO_client import SocketIO, LoggingNamespace

import airpen

import Adafruit_BluefruitLE


# Enable debug output.
#logging.basicConfig(level=logging.DEBUG)

# Define service and characteristic UUIDs used by the UART service.
ACCEL_SERVICE_UUID = uuid.UUID('87de0001-51b5-43c3-9ccb-993004dd54aa')
ACCEL_DATA_CHAR = uuid.UUID('87de0002-51b5-43c3-9ccb-993004dd54aa')

# Get the BLE provider for the current platform.
ble = Adafruit_BluefruitLE.get_provider()


# Main function implements the program logic so it can run in a background
# thread.  Most platforms require the main thread to handle GUI events and other
# asyncronous events like BLE actions.  All of the threading logic is taken care
# of automatically though and you just need to provide a main function that uses
# the BLE provider.
def main():
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
    device.discover([ACCEL_SERVICE_UUID], [ACCEL_DATA_CHAR])

    # Find the UART service and its characteristics.
    accel_service = device.find_service(ACCEL_SERVICE_UUID)
    data_char = accel_service.find_characteristic(ACCEL_DATA_CHAR)

    # Write a string to the TX characteristic.
    # print('Sending message to device...')
    # tx.write_value('Hello world!\r\n')

    # Function to receive RX characteristic changes.  Note that this will
    # be called on a different thread so be careful to make sure state that
    # the function changes is thread safe.  Use Queue or other thread-safe
    # primitives to send data to other threads.
    def received(data):
        x, y, z = airpen.airpen(data)
        with SocketIO('localhost', 5000) as socketIO:
            socketIO.emit('values', x)
            socketIO.wait(seconds=1)
        # with open('bytes.txt', 'a') as txt:
        #     txt.write(str(.getvalue()))
        # print('Received: {0}'.format(data))

    # Turn on notification of RX characteristics using the callback above.
    print('Subscribing to RX characteristic changes...')
    data_char.start_notify(received)

    # Now just wait for 30 seconds to receive data.
    print('Waiting 60 seconds to receive data from the device...')
    time.sleep(60)

    # Make sure device is disconnected on exit.
    # device.disconnect()



# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
