import network, time
from machine import Pin, reset
import usocket

def connect_wifi(ssid, password, led, timeout=15):
    wlan = network.WLAN(network.STA_IF)

    try:
        wlan.active(False)
        time.sleep(1)
        wlan.active(True)

        print("connecting to network:", ssid)
        wlan.connect(ssid, password)

        start = time.time()
        while not wlan.isconnected():
            led.value(not led.value())
            time.sleep(0.3)

            if time.time() - start > timeout:
                print("wifi connect failed")
                print("status:", wlan.status())
                wlan.disconnect()
                wlan.active(False)
                return False

        led.value(0)
        print("Connected:", wlan.ifconfig())
        return True

    except OSError as e:
        print("WiFi error:", e)
        time.sleep(2)
        reset()

if __name__ == '__main__':
    led1 = Pin(15, Pin.OUT)
    # ssid = "xtaiuiu_Pura_70_pro"        # 确保是 2.4G
    # password = "787816998"
    
    ssid = "MIFI-E33A"
    password = "1234567890"
    
    dest_ip = "192.168.100.200"
    dest_port = 10001
    # ssid = "Nxszstsg"
    # password = "qwertyuiop"
    if connect_wifi(ssid, password, led1):
        socket = usocket.socket()
        addr = (dest_ip, dest_port)
        socket.connect(addr)
        socket.send("Hello PRECHIN")
        print("send success")
        
        buffer = b""
        while True:
            data = socket.recv(128)
            if not data:
                continue

            buffer += data

            try:
                msg = buffer.decode("utf-8")
                print("received: " + msg)

                socket.send(("I get " + msg).encode("utf-8"))
                buffer = b""   # 解码成功才清空

            except UnicodeError as e:
                print(e)
                pass

    time.sleep_ms(300)
    
