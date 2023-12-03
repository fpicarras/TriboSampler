import asyncio
import matplotlib.pyplot as plt
from bleak import BleakScanner, BleakClient

class TriboSampler:
    def __init__(self, deviceName, serviceUUID, charUUID):
        self.deviceName = deviceName
        self.serviceUUID = serviceUUID
        self.charUUID = charUUID
        self.device = None

        self.time = []
        self.data = []

    async def connect(self):
        devices = await BleakScanner.discover()
        for d in devices:
            try:
                if d.name == self.deviceName:
                    print("Found it!")
                    self.device = d
                    return True
            except KeyError:
                pass
        if self.device == None:
            print("Unable to locate device!")
            return False

    async def readValue(self, graphWidth=100):
        def on_close(event):
            print("Program Ended!")
            exit()

        async with BleakClient(str(self.device.address)) as client:
            plt.ion()  # Turn on interactive mode for real-time plotting
            plt.figure()
            plt.gcf().canvas.mpl_connect('close_event', on_close)
            plt.show()
            while(1):
                try:
                    message = (await client.read_gatt_char(self.charUUID)).decode('utf-8')
                    message = message.split("|")

                    self.data.append(float(message[0])*(3.3/4095.0)-2.83)
                    self.time.append(float(message[1])/1000)

                    self.data = self.data[-graphWidth:]
                    self.time = self.time[-graphWidth:]
                    
                    # Update the plot
                    plt.clf()
                    plt.plot(self.time, self.data, label='Sensor Data')
                    plt.ylim(-1, 1)
                    plt.xlabel('Time')
                    plt.ylabel('Sensor Value')
                    plt.title('Real-Time Sensor Data')
                    plt.legend()
                    plt.pause(0.01)  # Adjust the pause time for a suitable refresh rate
                except KeyboardInterrupt:
                    pass


async def main():
    TbSampler = TriboSampler("TriboSamplerV1", "4fafc201-1fb5-459e-8fcc-c5c9c331914b", "beb5483e-36e1-4688-b7f5-ea07361b26a8")
    if(await TbSampler.connect()):
        await TbSampler.readValue()
    

asyncio.run(main())