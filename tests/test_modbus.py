import serial
import time

def send_data(port, baudrate, data):
    # 打开串口
    with serial.Serial(port, baudrate, timeout=1) as ser:
        # 发送数据
        time.sleep(0.1)
        ser.write(data)
        time.sleep(0.05)

        print(f"Sent data: {data}, recv:{ser.read_all()}")
        time.sleep(0.1)

if __name__ == "__main__":
    # 给定的数据
    #data = [0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x90, 0x67]
    data = [0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB]
    # 设置串口和波特率
    port = '/dev/ttyUSB0'  # 根据你的设备来更改
    baudrate = 115200  # 根据你的设备来更改

    for i in range(10):
        send_data(port, baudrate, bytearray(data))
