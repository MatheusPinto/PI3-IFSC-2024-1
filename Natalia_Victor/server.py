# Interface Includes
import tkinter as tk
from tkinter import filedialog, messagebox
import json

from threading import Thread
from flask import Flask, request, jsonify
import socket
import json

# Variáveis globais
udp_port = 4210
broadcast_message = "ESP32_HERE"
broadcast_address = "255.255.255.255"
appFlask = Flask(__name__)
endpoint = "/data"
selfp = ""
dataDirectory = 'data'

class Server:
    def __init__(self, root, pt):
        global selfp
        selfp = self

        self.APsData = {'SSID': "",
                        'APs': {},
                        'Latencia': {},
                        'Maximos': {}}
        self.points = pt # Int
        self.dataCount = 0
        self.flask_running = False
        self.sockets = []

        self.root = root    
        self.root.title("Conectar ao ESP32")
        self.root.protocol("WM_DELETE_WINDOW", self.interrupt_server)
        self.root.geometry("300x300")
        self.root.resizable(False, False)
        self.setup_ui()

        self.esp32_ip = ""
        self.server_closed_callback = None 
        self.server_data_callback = None

    def setup_ui(self):
       self.ip_entry_frame = tk.Frame(self.root)
       self.ip_entry_frame.pack(side='top', pady=5)

       self.label_status = tk.Label(self.ip_entry_frame, text="Aguardando mensagem Broadcast do ESP32")
       self.label_status.pack(side='top', pady=10)
       self.label_address = tk.Label(self.ip_entry_frame, text="")
       self.label_address.pack(side='top', pady=10)
       self.label_points = tk.Label(self.ip_entry_frame, text="")
       self.label_points.pack(side='top', pady=10)

       # Iniciar a thread de broadcast e conexão
       self.broadcast_thread = Thread(target=self.broadcast)
       self.broadcast_thread.start()

    def update_status(self, status):
        self.label_status.config(text=status)

    def update_ip(self, esp32_ip):
        self.label_address.config(text=f"IP da ESP32: {esp32_ip}")
    
    def update_counter(self, value):
        self.label_points.config(text=f"Pontos recebidos: {value} / {self.points}")

    def run_flask_app(self):
        print("Iniciando o servidor Flask...")
        appFlask.run(host="0.0.0.0", port=5000)
        self.flask_running = True
    
    def broadcast(self):
        print("Aguardando por mensagem de broadcast...")
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.bind(("", udp_port))
        self.sockets.append(sock)

        ip_received = False
        while not ip_received:
            # Espera por mensagens de broadcast
            data, addr = sock.recvfrom(1024)
            message = data.decode()
    
            if message == broadcast_message:
                self.esp32_ip = addr[0]
                response_message = f"PYTHON_IP:{self.get_ip()},POINTS:{self.points}"
                while True:
                    # Enviar a mensagem de IP e pontos
                    sock.sendto(response_message.encode(), (self.esp32_ip, udp_port))
                    print("Mensagem com IP e pontos enviada.")
                    
                    # Esperar por confirmação
                    sock.settimeout(1)  # Tempo de espera para confirmação
                    try:
                        data, _ = sock.recvfrom(1024)
                        confirmation_message = data.decode()
                        if confirmation_message == "CONFIRMATION_RECEIVED":
                            print("Confirmação recebida do ESP32.")
                            ip_received = True
                            break
                        else:
                            print("Mensagem de confirmação inválida.")
                    except socket.timeout:
                        print("Sem confirmação, reenviando a mensagem.")
                        # Enviar novamente se a confirmação não for recebida
    
        sock.close()

        flask_thread = Thread(target=self.run_flask_app)
        flask_thread.start()

        # Atualizar a interface após a conexão
        self.update_ip(self.esp32_ip)
        self.update_status("Conexão estabelecida")
        self.update_counter(0)

    def get_ip(self):
        sock_ip = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock_ip.settimeout(0)
        self.sockets.append(sock_ip)

        try:
            sock_ip.connect(('10.254.254.254', 1))
            IP = sock_ip.getsockname()[0]
        except Exception:
            IP = '127.0.0.1'
        finally:
            sock_ip.close()
        return IP
    
    @appFlask.route(endpoint, methods=['POST'])
    def receive_data():
        global selfp

        data = request.get_data(as_text=True)
        print(f"Dados recebidos:\n{data}")
        
        if data.startswith("SSID"):
            alldata = data.split("\n")

            ssidData = alldata[0].split(',')[0].split(':')[1]
            pointNum = alldata[0].split(',')[1]
            aps = alldata[1:-1]
            latency = alldata[-1]

            selfp.APsData['SSID'] = ssidData

            for AP in aps:
                APData = AP.split(",")
                BSSID = APData[0]
                CHANNEL = int(APData[1])
                RSSI = int(APData[2])

                if BSSID not in selfp.APsData['APs']:
                    selfp.APsData['APs'][BSSID] = {
                        'Channel': CHANNEL,
                        'RSSI': {}
                    }
                selfp.APsData['APs'][BSSID]['RSSI'][pointNum] = RSSI

            if 'Latencia' not in selfp.APsData:
                selfp.APsData['Latencia'] = {}
                
            selfp.APsData['Latencia'][pointNum] = latency
            selfp.dataCount = selfp.dataCount + 1

        elif data == "ALL_POINTS_SENT":
            print("Todos os pontos foram recebidos. Encerrando o servidor...")
            selfp.update_counter(selfp.dataCount)
            selfp.update_status("Conexão encerrada.")
            selfp.preencher_chaves_faltantes()

            if selfp.server_data_callback:
                selfp.server_data_callback(selfp.APsData)

            selfp.close_server()   
            return jsonify({"status": "success"}), 200    

        else:
            print("Error reading data.")

        selfp.update_counter(selfp.dataCount)

        return jsonify({"status": "success"}), 200
    
    def save_data(self):
        filename = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'w+') as file:
                json.dump(self.APsData, file)
  
    def preencher_chaves_faltantes(self, valor_nulo=-100):
        """
        Preenche as chaves faltantes de 0 a points em um dicionário de RSSI para cada BSSID com um valor fixo nulo.
        
        param valor_nulo: O valor a ser atribuído às chaves faltantes (default é None).
        return: O dicionário APs_Data atualizado com todas as chaves de 0 a points para cada BSSID.
        """
        for BSSID in self.APsData['APs']:
            for chave in range(self.points):
                if str(chave) not in self.APsData['APs'][BSSID]['RSSI']:
                    self.APsData['APs'][BSSID]['RSSI'][str(chave)] = valor_nulo

        for chave in range(self.points):
            if str(chave) not in self.APsData['Latencia']:
                self.APsData['Latencia'][str(chave)] = None

        max_rssi = {}
        for BSSID, data in self.APsData['APs'].items():
            for key, value in data['RSSI'].items():
                if value is not None:
                    if key not in max_rssi:
                        max_rssi[key] = value
                    else:
                        max_rssi[key] = max(max_rssi[key], value) if max_rssi[key] is not None else value
        self.APsData['Maximos'] = max_rssi

    def interrupt_server(self):
        ask = messagebox.askquestion('Server', 'Tem certeza que deseja interromper a conexão?')
        if ask == 'yes':
            self.shutdown_flask()
            self.data_count = 0
            if self.server_closed_callback:
                self.server_closed_callback()
            self.root.destroy()

    def shutdown_flask(self):
        for sock in self.sockets:
            sock.close()

        if self.flask_running:
            func = request.environ.get('werkzeug.server.shutdown')
            if func is None:
                raise RuntimeError("Não foi possível finalizar o servidor.")
            func()
      
    def close_server(self):
        ask = messagebox.askquestion('Server', "Dados recebidos com sucesso. Deseja salvar?")
        if ask == 'yes':
            self.save_data()

        if self.server_closed_callback:
            self.server_closed_callback()
        self.shutdown_flask()
        self.root.destroy()
