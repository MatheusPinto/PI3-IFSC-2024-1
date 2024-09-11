import config
from ultralytics import YOLO
from utils import detect_webcam_video
from utils import update_daily_totals
from utils import get_foodscale_reading
from time import sleep
from utils import user_menu

import os

HOME = os.getcwd()
print(HOME)

model = YOLO("best_training_n8.pt")

print("Bem vindo ao AI Food!")
sleep(1)

# NOTE (@eric_reis): Chamada necessária para identificar se a balança se encontra conectada e o
#                    endereço IP é o correto.
get_foodscale_reading()

user_id, conn, cursor = user_menu()

if user_id:
    update_daily_totals(user_id, conn, cursor)
    detect_webcam_video(user_id, model, conn, cursor)
