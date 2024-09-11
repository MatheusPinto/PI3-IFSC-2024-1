from ingredients_crud import create_food
from config import connect
from time import sleep

FOOD_DATABASE = [
    ("banana", "prata", 0.01, 0.22, 0.0, 0.92),
    ("banana", "ouro", 0.015, 0.25, 0.0, 1.06),
    ("maca", "verde", 0.005, 0.138, 0.0, 0.572),
    ("maca", "fuji", 0.004, 0.14, 0.0, 0.576),
    ("maca", "gala", 0.006, 0.139, 0.0, 0.58),
    ("frango", "peito", 0.31, 0.0, 0.036, 1.524),
    ("frango", "sobrecoxa", 0.21, 0.0, 0.12, 2.13),
    ("frango", "coxa", 0.18, 0.0, 0.16, 2.22),
    ("alface", "crespa", 0.012, 0.029, 0.002, 0.186),
    ("alface", "americana", 0.009, 0.026, 0.002, 0.155),
    ("alface", "romana", 0.012, 0.033, 0.003, 0.213),
    ("arroz", "branco", 0.027, 0.28, 0.003, 1.525),
    ("arroz", "integral", 0.026, 0.23, 0.016, 1.654),
    ("arroz", "parboilizado", 0.029, 0.27, 0.002, 1.472),
    ("batata", "inglesa", 0.02, 0.17, 0.001, 0.771),
    ("batata", "doce", 0.016, 0.2, 0.001, 0.893),
    ("batata", "vermelha", 0.017, 0.17, 0.001, 0.771),
    ("carne", "contrafile", 0.2, 0.0, 0.1, 1.7),
    ("carne", "alcatra", 0.21, 0.0, 0.15, 2.31),
    ("carne", "fraldinha", 0.19, 0.0, 0.14, 2.04),
    ("carne", "coxao duro", 0.2, 0.0, 0.11, 1.79),
    ("cebola", "branca", 0.011, 0.093, 0.001, 0.433),
    ("cebola", "amarela", 0.011, 0.093, 0.001, 0.433),
    ("cebola", "roxa", 0.012, 0.076, 0.002, 0.394),
    ("feijao", "preto", 0.09, 0.2, 0.005, 1.345),
    ("feijao", "vermelho", 0.089, 0.225, 0.005, 1.471),
    ("feijao", "branco", 0.08, 0.2, 0.005, 1.325),
    ("feijao", "carioca", 0.078, 0.21, 0.005, 1.347),
    ("laranja", "bahia", 0.01, 0.083, 0.001, 0.399),
    ("laranja", "pera", 0.009, 0.082, 0.002, 0.404),
    ("laranja", "lima", 0.009, 0.083, 0.002, 0.408),
    ("laranja", "valencia", 0.01, 0.092, 0.001, 0.429),
    ("leite", "integral", 0.034, 0.047, 0.036, 0.677),
    ("leite", "desnatado", 0.034, 0.048, 0.002, 0.372),
    ("leite", "semidesnatado", 0.034, 0.048, 0.015, 0.523),
    ("melancia", "melancia", 0.006, 0.076, 0.002, 0.366),
    ("morango", "morango", 0.008, 0.077, 0.004, 0.428),
    ("ovo", "frito", 0.156, 0.012, 0.186, 2.4),
    ("ovo", "cozido", 0.133, 0.006, 0.095, 1.46),
    ("tomate", "salada", 0.009, 0.039, 0.002, 0.225),
    ("tomate", "italiano", 0.011, 0.032, 0.002, 0.226),
    ("tomate", "marmande", 0.009, 0.039, 0.002, 0.225),
]

conn, cursor = connect()

if not conn:
    print("não foi possível estabelecer uma conexão")
    exit()

print("Inicializando a criação dos alimentos no banco de dados")
sleep(2)

for food in FOOD_DATABASE:
    try:
        create_food(*food, conn=conn, cursor=cursor)
        print(f"O alimento {food[0]} {food[1]} foi criado")
    except Exception:
        print(
            f"Um erro ocorreu ao adicionar o elemento '{food[0]}' de variedade '{food[1]}'"
        )
        continue
