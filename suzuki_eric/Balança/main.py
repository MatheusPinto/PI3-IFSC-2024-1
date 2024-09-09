import network
import usocket as socket
from machine import Pin, SoftI2C
from hx711 import HX711
from lcd_api import LcdApi
from i2c_lcd import I2cLcd
from time import sleep

# WiFi credentials
WIFI_SSID = "WIFI_NAME"
WIFI_PASS = "PASSWORD"
# I2C address of the LCD
I2C_ADDR = 0x27
# Total number of rows and columns of the LCD
totalRows = 2
totalColumns = 16

# Define the data pin (DT) and serial clock pin (SCK) for the HX711
DATA_PIN = 2  # Pin connected to DT
SCK_PIN = 4  # Pin connected to SCK

global ESP32_IP_ADDRESS


# Connect to WiFi
def connect_to_wifi(ssid, password) -> bool:
    wlan = network.WLAN(network.STA_IF)

    wlan.active(True)
    if not wlan.isconnected():
        print("Connecting to WiFi...")
        try:
            wlan.connect(ssid, password)
        except Exception:
            display_wifi_error()
            return False

        while not wlan.isconnected():
            pass
    print("WiFi connected")

    global ESP32_IP_ADDRESS
    ESP32_IP_ADDRESS = wlan.ifconfig()[0]

    print("ESP32 IP address:", ESP32_IP_ADDRESS)
    return True


# Initialize I2C method for ESP32
i2c = SoftI2C(scl=Pin(22), sda=Pin(21), freq=10000)
# Initialize LCD object
lcd = I2cLcd(i2c, I2C_ADDR, totalRows, totalColumns)

# Create an instance of the HX711 object
hx = HX711(DATA_PIN, SCK_PIN)
hx.set_scale(10)  # Set the scale
tare_weight = 0  # Initialize variable to store the known tare weight


def generate_raw_data():
    weight = get_scale_reading()
    write_lcd_text(weight)
    return str(weight)  # Return the raw weight as a string


def get_scale_reading() -> float:
    global tare_weight
    hx.tare()  # Request known tare weight
    current_reading = hx.make_average()  # Read the weight

    if tare_weight == 0:  # If tare weight is not yet stored
        tare_weight = current_reading  # Store the tare weight

    reading = float(current_reading) - float(tare_weight)  # Calculate net weight
    print(f"{reading}g")  # Print weight in kilograms

    return reading


def display_wifi_error():
    while True:
        linha_superior = "Cred. invalidas"
        linha_inferior = "de wifi"

        lcd.clear()
        lcd.putstr(linha_superior)
        lcd.move_to(0, 1)
        lcd.putstr(linha_inferior)
        sleep(5)

        linha_superior = "Atualize as"
        linha_inferior = "credenciais"

        lcd.clear()
        lcd.putstr(linha_superior)
        lcd.move_to(0, 1)
        lcd.putstr(linha_inferior)
        sleep(5)


def display_esp32_ip_address():
    linha_superior = "Ender. IP ESP32"

    global ESP32_IP_ADDRESS
    linha_inferior = ESP32_IP_ADDRESS if ESP32_IP_ADDRESS else "IP n ident."

    lcd.clear()
    lcd.putstr(linha_superior)
    lcd.move_to(0, 1)
    lcd.putstr(linha_inferior)
    sleep(2)


def write_lcd_text(scale_reading):
    # Update LCD with weight information
    linha_superior = "Leitura atual"
    linha_inferior = "Peso: %.2f g" % scale_reading
    lcd.clear()
    lcd.putstr(linha_superior)
    lcd.move_to(0, 1)
    lcd.putstr(linha_inferior)


print(f"credentials are: {WIFI_SSID}, {WIFI_PASS}")
has_connected = connect_to_wifi(WIFI_SSID, WIFI_PASS)

if has_connected:
    # Create a socket and listen for connections
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("", 80))
    s.listen(5)
    display_esp32_ip_address()
    print("Web server running...")
    while True:
        weight = get_scale_reading()
        write_lcd_text(weight)

        conn, addr = s.accept()
        request = conn.recv(1024)
        response = generate_raw_data()  # Send raw weight data
        conn.send("HTTP/1.1 200 OK\n")
        conn.send("Content-Type: text/plain\n")
        conn.send("Connection: close\n\n")
        conn.sendall(response)
        conn.close()
