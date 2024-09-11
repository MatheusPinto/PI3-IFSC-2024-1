# Main App Includes
import tkinter as tk
from tkinter import filedialog, messagebox
import json
from shapely.geometry import Polygon, Point
import math
from threading import Thread

# Connect Includes
from server import Server

# Heat Map Includes
from heatmap import HeatMap

# Variáveis globais para armazenar dados do esboço
external_walls = []
internal_walls = []
doors = []
windows = []
points = []
points_distance = 50
dislocation = 25

APsData = None

class MainApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Analisador de Sinal Wi-FI")
        self.root.protocol("WM_DELETE_WINDOW", self.close_mainApp)
        self.root.geometry("600x400")
        self.root.minsize(600,400)
        self.root.maxsize(600,400)
        self.setup_ui()

        self.sketch = None
        self.server = None
        self.heatmap = None

        # Controle de estado dos menus e da conexão
        self.sketch_open = False
        self.server_open = False
        self.heatmap_open = False

    def setup_ui(self):
        mainframe = tk.Frame(self.root)
        mainframe.pack(fill="both", expand=True)

        label = tk.Label(mainframe, text="Projeto Integrador 3 - Analisador de rede Wi-FI\nVictor Lompa e Natália Besen")
        label.pack(pady=20)

        create_sketch_button = tk.Button(mainframe, text="Editar Esboço", width=30, height=3, borderwidth=2, command=self.open_sketch)
        create_sketch_button.pack(pady=20)
        connect_to_ESP32 = tk.Button(mainframe, text="Conectar ao ESP", width=30, height=3, borderwidth=2, command=self.open_connect)
        connect_to_ESP32.pack(pady=20)
        create_heat_map = tk.Button(mainframe, text="Mapa de Calor", width=30, height=3, borderwidth=2, command=self.open_heatmap)
        create_heat_map.pack(pady=20)
 
    def open_sketch(self):
        if self.server_open == True: 
            messagebox.showwarning("Aviso", "Conexão com a ESP está em andamento. Não é possível editar o esboço agora.")
        elif self.sketch_open == False:
            sketch_window = tk.Toplevel(self.root)
            self.sketch = HouseSketch(sketch_window)
            self.sketch_open = True
            self.sketch.sketch_closed_callback = self.sketch_closed
            
    def open_connect(self):
        global points
        if self.server_open == False:
            connect_window = tk.Toplevel(self.root)
            self.server = Server(connect_window, len(points))
            self.server_open = True
            self.server.server_closed_callback = self.server_closed
            self.server.server_data_callback = self.server_data

    def open_heatmap(self):
        if self.heatmap_open == False:
            heatmap_window = tk.Toplevel(self.root)
            self.heatmap = HeatMap(heatmap_window,   {'external_walls': external_walls,
                                                    'internal_walls': internal_walls,
                                                    'doors': doors,
                                                    'windows': windows,
                                                    'points': points}, APsData)
            self.heatmap_open = True
            self.heatmap.heatmap_close_callback = self.heatmap_closed

    def close_mainApp(self):
        ask = messagebox.askquestion(title="Analisador de Sinal Wi-Fi", message="Deseja encerrar a aplicação?", icon='warning')
        if ask == 'yes':
            self.root.destroy()

    def sketch_closed(self):
        self.sketch_open = False
    
    def server_closed(self):
        self.server_open = False
    
    def server_data(self, data):
        global APsData
        APsData = data

    def heatmap_closed(self):
        self.heatmap_open = False
    
class HouseSketch:
    def __init__(self, root):
        self.root = root
        self.root.protocol("WM_DELETE_WINDOW", self.exit_sketch)
        self.root.title("Editor de esboço")
        self.canvas = tk.Canvas(root, width=800, height=600, bg='white')
        self.canvas.pack(fill='both', expand=True)

        self.grid_size = 100
        self.snap_distance = 10
        self.snap_to_grid_enabled = True

        self.drawing = False
        self.current_element = []
        self.element_type = 'wall'
        self.wall_type = 'external'
        self.history = []
        self.redo_stack = []
        self.show_points = True
        self.show_point_numbers = True
        self.show_shortest_path = True

        self.sketch_closed_callback = None

        self.setup_ui()
        self.draw_grid()

    def setup_ui(self):

        # Create and pack Frames
        config_frame = tk.Frame(self.root)
        config_frame.pack(side='top', fill='both')

        itens_frame = tk.Frame(config_frame)  # Frame de Item 
        itens_frame.pack(side='left')
        self.options_frame = tk.Frame(itens_frame)  # Tipo de item
        self.options_frame.pack(side='top') 
        self.wall_options_frame = tk.Frame(itens_frame)  # Tipo de parede
        self.wall_options_frame.pack(side='top')
        
        tools_frame = tk.Frame(config_frame)  # Ferramentas
        tools_frame.pack(side='left')

        grid_frame = tk.Frame(config_frame)  # Frame da Grid
        grid_frame.pack(side='left')
        grid_size_frame = tk.Frame(grid_frame)  # Tamanho da Grid
        grid_size_frame.pack(side='bottom')
        
        generate_frame = tk.Frame(config_frame)  # Geração de Pontos
        generate_frame.pack(side='left')
        generate_distance_frame = tk.Frame(generate_frame)  # Distância de geração
        generate_distance_frame.pack(side='left') 
        
        points_frame = tk.Frame(config_frame)  # Visualização de pontos
        points_frame.pack(side='left')
        
        # Adicionando o menu
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        # Menu de Arquivo
        filemenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Arquivo", menu=filemenu)
        filemenu.add_command(label="Novo", command=self.new_sketch)
        filemenu.add_command(label="Salvar", command=self.save_sketch)
        filemenu.add_command(label="Carregar", command=self.load_sketch)
        filemenu.add_separator()
        filemenu.add_command(label="Sair", command=self.exit_sketch)

        # Menu de Edição
        editmenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Editar", menu=editmenu)
        editmenu.add_command(label="Desfazer", command=self.undo)
        editmenu.add_command(label="Refazer", command=self.redo)

        # Seleção de Item 
        tk.Button(self.options_frame, text="Parede", command=lambda: self.set_element_type('wall')).pack(side='left')
        tk.Button(self.options_frame, text="Porta", command=lambda: self.set_element_type('door')).pack(side='left')
        tk.Button(self.options_frame, text="Janela", command=lambda: self.set_element_type('window')).pack(side='left')

        # Seleção de tipo de Parede
        self.wall_type_var = tk.StringVar(value=self.wall_type)
        tk.Radiobutton(self.wall_options_frame, text="Externa", variable=self.wall_type_var, value='external', command=self.update_wall_type).pack(side='left')
        tk.Radiobutton(self.wall_options_frame, text="Interna", variable=self.wall_type_var, value='internal', command=self.update_wall_type).pack(side='left')

        # Edição do Grid
        self.snap_var = tk.BooleanVar(value=self.snap_to_grid_enabled)
        tk.Checkbutton(grid_frame, text="Grudar ao Grid", variable=self.snap_var, command=self.update_snap).pack(side='top')
        tk.Label(grid_size_frame, text="Tamanho:").pack(side='left')
        self.grid_size_entry = tk.Entry(grid_size_frame, width=5)
        self.grid_size_entry.insert(0, str(self.grid_size))
        self.grid_size_entry.pack(side='left')
        self.grid_size_entry.bind("<Return>", self.apply_grid_size)

        # Geração de Pontos
        tk.Label(generate_distance_frame, text="Ajuste de Pontos").pack(side='top')
        self.distance_slider = tk.Scale(generate_distance_frame, from_=20, to_=100, orient='horizontal', command=self.update_point_distance)
        self.distance_slider.set(points_distance)
        self.distance_slider.pack(side='left')
        dislocation_slider = tk.Scale(generate_distance_frame, from_=1, to_=50, orient='horizontal', command=self.update_dislocation_distance)
        dislocation_slider.set(dislocation)
        dislocation_slider.pack(side='left')
        tk.Button(points_frame, text="Calcular", command=self.calculate_points).pack(side='left')

        # Opções para pontos
        self.show_points_var = tk.BooleanVar(value=self.show_points)
        tk.Checkbutton(points_frame, text="Mostrar Pontos", variable=self.show_points_var, command=self.toggle_points).pack(side='top',anchor='w')
        self.show_shortest_path_var = tk.BooleanVar(value=self.show_shortest_path)
        tk.Checkbutton(points_frame, text="Mostrar Trajeto", variable=self.show_shortest_path_var, command=self.toggle_shortest_path).pack(side='top',anchor='w')
        self.show_point_numbers_var = tk.BooleanVar(value=self.show_point_numbers)
        tk.Checkbutton(points_frame, text="Mostrar Númeração", variable=self.show_point_numbers_var, command=self.toggle_point_numbers).pack(side='top',anchor='w')
        
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind_all("<Control-z>", self.undo)
        self.canvas.bind_all("<Control-y>", self.redo)

    def set_element_type(self, element_type):
        self.current_element.clear()
        self.element_type = element_type
        if element_type == 'wall':
            self.toggle_wall_options()
        else:
            self.wall_options_frame.pack_forget()

    def toggle_wall_options(self):
        if self.element_type == 'wall':
            self.wall_options_frame.pack(side='top', fill='x')
        else:
            self.wall_options_frame.pack_forget()

    def update_wall_type(self):
        self.wall_type = self.wall_type_var.get()

    def draw_grid(self):
        self.canvas.delete("grid")
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()

        for x in range(0, width, self.grid_size):
            self.canvas.create_line(x, 0, x, height, fill='lightgray', dash=(2, 2), tags="grid")

        for y in range(0, height, self.grid_size):
            self.canvas.create_line(0, y, width, y, fill='lightgray', dash=(2, 2), tags="grid")

    def snap_to_grid(self, x, y):
        if self.snap_to_grid_enabled:
            x = round(x / self.grid_size) * self.grid_size
            y = round(y / self.grid_size) * self.grid_size
        return x, y

    def update_snap(self):
        self.snap_to_grid_enabled = self.snap_var.get()

    def apply_grid_size(self, event=None):
        try:
            new_size = int(self.grid_size_entry.get())
            if new_size < 10:
                new_size = 10
            elif new_size > 100:
                new_size = 100
            self.grid_size = new_size
            self.grid_size_entry.delete(0, tk.END)
            self.grid_size_entry.insert(0, str(self.grid_size))
            self.draw_grid()
            self.update_canvas()
        except ValueError:
            messagebox.showerror("Erro", "Por favor, insira um número válido.")

    def update_point_distance(self, val):
        global points_distance
        points_distance = int(val)
        self.calculate_points()

    def update_dislocation_distance(self, val):
        global dislocation
        dislocation = int(val)
        self.calculate_points()

    def toggle_points(self):
        self.show_points = self.show_points_var.get()
        self.update_canvas()

    def toggle_point_numbers(self):
        self.show_point_numbers = self.show_point_numbers_var.get()
        self.update_canvas()

    def toggle_shortest_path(self):
        self.show_shortest_path = self.show_shortest_path_var.get()
        self.update_canvas()

    def on_click(self, event):
        x, y = self.snap_to_grid(event.x, event.y)

        if self.element_type == 'wall':
            if self.wall_type == 'external':
                if not self.drawing:
                    self.current_element = [(x, y)]
                    self.drawing = True
                else:
                    if len(self.current_element) > 0 and (x, y) == self.current_element[0]:
                        # Termina o polígono
                        self.current_element.append(self.current_element[0])
                        self.finish_element()
                        self.toggle_wall_options()
                    else:
                        self.current_element.append((x, y))
            elif self.wall_type == 'internal':
                if len(self.current_element) == 0:
                    self.current_element = [(x, y)]
                elif len(self.current_element) == 1:
                    self.current_element.append((x, y))
                    self.finish_element()
        elif self.element_type in ['door', 'window']:
            if len(self.current_element) == 0:
                self.current_element = [(x, y)]
            elif len(self.current_element) == 1:
                self.current_element.append((x, y))
                self.finish_element()

        self.update_canvas()

    def finish_element(self):
        if self.element_type == 'wall':
            if self.wall_type == 'external':
                if len(self.current_element) > 2:
                    external_walls.append(self.current_element)
            elif self.wall_type == 'internal':
                if len(self.current_element) == 2:
                    internal_walls.append(self.current_element)
        elif self.element_type == 'door':
            if len(self.current_element) == 2:
                doors.append(self.current_element)
        elif self.element_type == 'window':
            if len(self.current_element) == 2:
                windows.append(self.current_element)

        self.history.append((self.element_type, self.wall_type, self.current_element))
        self.redo_stack.clear()
        self.current_element = []
        self.drawing = False
        self.update_canvas()

    def update_canvas(self):
        self.canvas.delete("all")
        self.draw_grid()
        for wall in external_walls:
            self.canvas.create_polygon(wall, outline='black', fill='', width=2)
        for wall in internal_walls:
            self.canvas.create_line(wall, fill='gray', width=2, dash=(4, 4))
        for door in doors:
            self.canvas.create_line(door, fill='brown', width=5)
        for window in windows:
            self.canvas.create_line(window, fill='blue', width=3)

        if self.show_shortest_path and points:
            for i in range(len(points) - 1):
                self.canvas.create_line(points[i], points[i + 1], fill='green', width=2)

        # Visualização de pontos
        if self.show_points and self.show_point_numbers:
            for idx, point in enumerate(points):
                self.canvas.create_oval(point[0] - 3, point[1] - 3, point[0] + 3, point[1] + 3, fill='red')
                self.canvas.create_text(point[0], point[1]-10, text=str(idx), fill='black')
        elif self.show_points and not self.show_point_numbers:
            for point in points:
                self.canvas.create_oval(point[0] - 3, point[1] - 3, point[0] + 3, point[1] + 3, fill='red')
        elif not self.show_points and self.show_point_numbers:
            for idx, point in enumerate(points):
                self.canvas.create_text(point[0], point[1], text=str(idx), fill='black')

        if self.drawing and self.element_type == 'wall' and self.wall_type == 'external':
            self.canvas.create_line(self.current_element, fill='blue', dash=(2, 2))
        elif self.drawing and self.element_type == 'wall' and self.wall_type == 'internal':
            if len(self.current_element) == 2:
                self.canvas.create_line(self.current_element, fill='green', width=2)

    def calculate_points(self):
        global points
        points.clear()
        for wall in external_walls:
            polygon = Polygon(wall)
            if not polygon.is_valid or polygon.exterior is None:
                continue
            min_x, min_y, max_x, max_y = polygon.bounds
            x = min_x + dislocation
            while x <= max_x:
                y = min_y + dislocation
                while y <= max_y:
                    point = Point(x, y)
                    if polygon.contains(point):
                        points.append((x, y))
                    y += points_distance
                x += points_distance

        # Encontrar o caminho mais curto e atualizar os pontos
        points = list(find_shortest_path(points)[0])
        
        self.update_canvas()

    def new_sketch(self):
        global external_walls, internal_walls, doors, windows, points_distance, dislocation, points

        external_walls = []
        internal_walls = []
        doors = []
        windows = []
        points = []
        points_distance = 50
        dislocation = 25
        self.grid_size = 100
        self.snap_distance = 10
        self.snap_to_grid_enabled = True
        self.drawing = False
        self.current_element = []
        self.element_type = 'wall'
        self.wall_type = 'external'
        self.history = []
        self.redo_stack = []
        self.show_points = True
        self.show_point_numbers = True
        self.show_shortest_path = True
        self.update_canvas()

    def save_sketch(self):
    # Salva um arquivo JSON com as informações do esboço
        filename = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'w') as file:
                json.dump({
                    'external_walls': external_walls,
                    'internal_walls': internal_walls,
                    'doors': doors,
                    'windows': windows,
                    'grid_size': self.grid_size,
                    'snap_to_grid': self.snap_to_grid_enabled,
                    'points': points,
                    'show_points': self.show_points,
                    'show_point_numbers': self.show_point_numbers,
                    'show_shortest_path': self.show_shortest_path
                }, file)

    def load_sketch(self):
    # Carrega um arquivo JSON com as informações do esboço
        global external_walls, internal_walls, windows, doors, points
        filename = filedialog.askopenfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'r') as file:
                data = json.load(file)
                external_walls = data.get('external_walls', [])
                internal_walls = data.get('internal_walls', [])
                windows = data.get('windows', [])
                points = data.get('points', [])
                doors = data.get('doors', [])
                self.grid_size = data.get('grid_size', 100)
                self.snap_to_grid_enabled = data.get('snap_to_grid', True)
                self.show_points = data.get('show_points', True)
                self.show_point_numbers = data.get('show_point_numbers', True)
                self.show_shortest_path = data.get('show_shortest_path', True)
                self.update_canvas()

    def exit_sketch(self):
        if self.sketch_closed_callback:
            self.sketch_closed_callback()
        self.root.destroy()

    def undo(self, event=None):
    # Undo (Control + Z)
        if not self.history:
            return
        last_action = self.history.pop()

        # Verifica se last_action tem a estrutura esperada
        if len(last_action) == 3:
            element_type, wall_type, element = last_action
            if element_type == 'wall':
                if wall_type == 'external':
                    external_walls.pop()
                elif wall_type == 'internal':
                    internal_walls.pop()
            elif element_type == 'door':
                doors.pop()
            elif element_type == 'window':
                windows.pop()

            # Adiciona o elemento removido na pilha de refazer
            self.redo_stack.append(last_action)
            self.update_canvas()
        else:
            print(f"Erro: last_action não tem 3 elementos, tem {len(last_action)}")

    def redo(self, event=None):
    # Redo (Control + Y)
        if not self.redo_stack:
            return

        last_undo = self.redo_stack.pop()
        self.history.append(last_undo)

        element_type, wall_type, element = last_undo
        if element_type == 'wall':
            if wall_type == 'external':
                external_walls.append(element)
            elif wall_type == 'internal':
                internal_walls.append(element)
        elif element_type == 'door':
            doors.append(element)
        elif element_type == 'window':
            windows.append(element)

        self.update_canvas()

# Funções auxiliares para TSP

def distance(p1, p2):
# Calcula distância entre dois pontos
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def find_shortest_path(points):
# Encontra um caminho aproximado passando por todos os pontos usando o algoritmo Nearest Neighbor
    if not points:
        return [], 0

    start_point = points[0]
    unvisited = points[1:]
    path = [start_point]
    total_distance = 0

    current_point = start_point
    while unvisited:
        nearest = min(unvisited, key=lambda p: distance(current_point, p))
        total_distance += distance(current_point, nearest)
        path.append(nearest)
        unvisited.remove(nearest)
        current_point = nearest

    return path, total_distance

# Código principal para inicializar a interface
root = tk.Tk()
app = Thread(target= MainApp(root))
root.mainloop()
