
# Interface Includes
import tkinter as tk
from tkinter import filedialog, messagebox
import json

# Heat Map Includes
import matplotlib.pyplot as plt
import matplotlib.cm as cm  # Para acessar diferentes colormaps
import numpy as np
from scipy.interpolate import griddata
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class HeatMap:
    def __init__(self, root, sketch=None, data=None):
        self.sketch_data = sketch
        self.apsData = data
        self.heatmap_close_callback = None

        self.root = root
        self.root.title("Mapa de Calor")
        self.root.geometry("800x600")
        
        self.root.protocol("WM_DELETE_WINDOW", self.close_heatMap)
        self.setup_ui()
        
    def setup_ui(self):
      
        # Frame principal
        self.frame = tk.Frame(self.root)
        self.frame.pack(fill="both", expand=True)
    
        # Label
        self.label = tk.Label(self.frame, text="Selecione o conjunto de dados para gerar o mapa de calor:")
        self.label.pack(pady=10)
    
        # Variável para armazenar a seleção do dado
        self.selection_var = tk.StringVar(self.frame)
    
        # Dropdown menu para selecionar o AP
        self.ap_selection = ttk.Combobox(self.frame, textvariable=self.selection_var)
        self.ap_selection.pack(pady=10)
        
        # Atualizar as opções do combobox
        self.update_ap_selection()

        # Botão para gerar o mapa de calor
        self.generate_button = tk.Button(self.frame, text="Gerar Mapa de Calor", command=self.generate_heatmap)
        self.generate_button.pack(pady=10)

        # Área onde o gráfico será exibido
        self.canvas_frame = tk.Frame(self.frame)
        self.canvas_frame.pack(fill="both", expand=True)
    
        # Adicionando o menu
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        # Menu de Arquivo
        filemenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Arquivo", menu=filemenu)
        filemenu.add_command(label="Carregar esboço externo", command=self.load_external_sketch)
        filemenu.add_command(label="Carregar dados externos", command=self.load_external_ap_data)

    def update_ap_selection(self):
        if self.apsData:
            ap_keys = list(self.apsData['APs'].keys()) + ['Latencia','Maximos']
            self.ap_selection['values'] = ap_keys

    def generate_heatmap(self):
        selection = self.selection_var.get()
        data = None

        if selection == 'Latencia': 
            data = self.apsData['Latencia']
        elif selection == 'Maximos': 
            data = self.apsData['Maximos']
        elif selection and selection in self.apsData['APs']: 
            data = self.apsData['APs'][selection]["RSSI"]
        else: messagebox.showerror("Erro", "Selecione um conjunto de dados válido válido.")

        if self.sketch_data and data != None:
            self.plot_heatmap(data, selection)
        else: messagebox.showerror("Erro", "Carregue um esboço antes de gerar o mapa de calor.")

    def plot_heatmap(self, data, selection):
   
        # Definindo limites dos eixos
        min_x = float('inf')
        max_x = float('-inf')
        min_y = float('inf')
        max_y = float('-inf')
    
        # Calculando os limites com base nas paredes externas
        for wall in self.sketch_data['external_walls']:
            wall = np.array(wall)
            min_x = min(min_x, wall[:, 0].min())
            max_x = max(max_x, wall[:, 0].max())
            min_y = min(min_y, wall[:, 1].min())
            max_y = max(max_y, wall[:, 1].max())
    
        # Adicionando margens para os eixos x e y
        margin_x = (max_x - min_x) * 0.1
        margin_y = (max_y - min_y) * 0.1
    
        # Ajustando os limites com margens
        min_x -= margin_x
        max_x += margin_x
        min_y -= margin_y
        max_y += margin_y
    
        fig, ax = plt.subplots()
        ax.set_xlim(min_x, max_x)
        ax.set_ylim(max_y, min_y)  # Inversão do eixo y
        ax.set_aspect('equal')

        if selection == 'Latencia':
            ax.set_title("Mapa de calor da Latência")
        else:
            ax.set_title("Mapa de calor do RSSI")
    
        # Desenhar paredes externas
        for wall in self.sketch_data['external_walls']:
            wall = np.array(wall)
            ax.plot(wall[:, 0], wall[:, 1], color='blue')
    
        # Desenhar portas
        for door in self.sketch_data['doors']:
            door = np.array(door)
            ax.plot(door[:, 0], door[:, 1], color='green')
    
        # Desenhar pontos numerados
        points = np.array(self.sketch_data['points'])
        scatter = ax.scatter(points[:, 0], points[:, 1], c='red')
    
        # Obter as medições para o conjunto de dados selecionado
        if selection in ('Latencia','Maximos'):
            data = self.apsData[selection]
        else:
            data = self.apsData['APs'][selection]["RSSI"]

        z = np.array([data[str(i)] for i in range(len(points))])
    
        # Criar grade para o mapa de calor
        grid_x, grid_y = np.mgrid[min_x:max_x:100j, min_y:max_y:100j]
    
        # Interpolação
        grid_z = griddata((points[:, 0], points[:, 1]), z, (grid_x, grid_y), method='cubic')
    
        # Ajustar a escala do mapa de calor
        vmin = min(z)
        vmax = max(z)
    
        # Plotar mapa de calor com escala de cores quentes e frias
        heatmap = ax.imshow(grid_z.T, extent=(min_x, max_x, min_y, max_y), origin='lower', cmap=cm.magma, alpha=0.5, vmin=vmin, vmax=vmax)
        if selection == 'Latencia':        
            fig.colorbar(heatmap, ax=ax, label='Latência')
        else:    
            fig.colorbar(heatmap, ax=ax, label='Intensidade de Sinal')
    
        # Remover a legenda
        scatter.set_label(None)
    
        # Exibir valor ao passar o mouse
        def on_move(event):
            if event.inaxes == ax:
                closest_point = np.argmin(np.sqrt((points[:, 0] - event.xdata)**2 + (points[:, 1] - event.ydata)**2))
                value = z[closest_point]
                if selection == 'Latencia':
                    ax.set_xlabel(f"Valor: {value} ms")
                else:
                    ax.set_xlabel(f"Valor: {value} dBm")
                fig.canvas.draw_idle()
        fig.canvas.mpl_connect('motion_notify_event', on_move)
    
        # Embed Matplotlib figure in Tkinter
        for widget in self.canvas_frame.winfo_children():
            widget.destroy()
    
        canvas = FigureCanvasTkAgg(fig, master=self.canvas_frame)
        canvas.draw()
        canvas.get_tk_widget().pack(fill="both", expand=True)
        plt.close(fig)
    
    def load_external_ap_data(self):
        filename = filedialog.askopenfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'r') as file:
                self.apsData = json.load(file)
            self.update_ap_selection()
            messagebox.showinfo("Sucesso", "Dados carregados com sucesso.")
        else:
            messagebox.showerror("Erro", "Falha ao carregar dados.")

    def load_external_sketch(self):
        filename = filedialog.askopenfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'r') as file:
                self.sketch_data = json.load(file)
            messagebox.showinfo("Sucesso", "Esboço carregado com sucesso.")
        else:
            messagebox.showerror("Erro", "Falha ao carregar o esboço.")

    def close_heatMap(self):
        if self.heatmap_close_callback:
            self.heatmap_close_callback() 
        self.root.destroy()