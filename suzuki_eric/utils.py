import cv2
import math
import supervision as sv
import requests
from requests.exceptions import ConnectionError
from requests.exceptions import ConnectTimeout
from ingredients_crud import get_foods_by_name
from food_diary_crud import create_diary_entry
from food_diary_crud import read_daily_totals
from users_crud import read_user
from users_crud import create_user
from http import HTTPStatus
from typing import Optional
from datetime import date
from getpass import getpass


annotator = sv.BoxAnnotator()

CLASS_NAMES_PTBR = [
    "alface",
    "arroz",
    "banana",
    "batata",
    "carne",
    "cebola",
    "feijao",
    "frango",
    "laranja",
    "leite",
    "maca",
    "melancia",
    "morango",
    "ovo",
    "tomate",
]

CLASS_NAMES_FOR_SEARCH = {
    "lettuce": "alface",
    "rice": "arroz",
    "banana": "banana",
    "potato": "batata",
    "meat": "carne",
    "onion": "cebola",
    "beans": "feijão",
    "chicken": "frango",
    "orange": "laranja",
    "milk": "leite",
    "apple": "maçã",
    "watermelon": "melancia",
    "strawberry": "morango",
    "egg": "ovo",
    "tomato": "tomate",
}

FOOD_TYPE_OPTIONS = {}

ESP32_IP_ADDRESS = "http://192.168.159.120"  # utilizar endereço IP obtido no ESP32
CONFIDENCE_THRESHOLD = (
    0.90  # nível mínimo de precisão de detecção dos alimentos pela IA
)

LAST_DETECTED_INGREDIENT = ""
IS_INGREDIENT_DETECTED = False
global TOTAL_DAILY_CALORIC_INTAKE
TOTAL_DAILY_CALORIC_INTAKE = 0
FONT = cv2.FONT_HERSHEY_SIMPLEX

SIGNED_UP = "1"
NOT_SIGNED_UP = "2"
EXIT_MENU = "9"

USER_MENU_OPTIONS = [SIGNED_UP, NOT_SIGNED_UP, EXIT_MENU]


def get_foodscale_reading() -> float:
    try:
        response = requests.get(ESP32_IP_ADDRESS, timeout=5)
    except (ConnectionError, ConnectTimeout):
        print(f"Erro ao conectar ao endereço IP '{ESP32_IP_ADDRESS}' da balança.")
        exit()

    if response.status_code == HTTPStatus.OK:
        weight = float(response.text)
        print(f"Peso mensurado: {weight} g")

        if weight > 0:
            return weight

        return 0

    print(
        f"Falha ao buscar os dados. Código de status da requisição: {response.status_code}"
    )

    return 0


def display_total_daily_calories_consumed(daily_totals) -> None:
    current_date = date.today().strftime("%d/%m/%Y")

    if not daily_totals["total_calories"]:
        print(f"Nenhum alimento foi consumido no dia {current_date}")
        return

    print(f"\nDetalhamento das calorias consumidas para o dia {current_date}:\n")

    print(
        f"Total de proteínas consumidas: {daily_totals['total_protein']:.2f}g\n"
        f"Total de carboidratos consumidos: {daily_totals['total_carbs']}g\n"
        f"Total de gorduras consumidas: {daily_totals['total_fats']:.2f}g\n"
        f"Total de calorias gastas: {daily_totals['total_calories']:.2f} kcal\n"
    )


def validate_foodscale_reading(weight) -> Optional[float]:
    pass


def draw_ingredient_options_text(image):
    text_Y_position = 270
    if not IS_INGREDIENT_DETECTED:
        return

    cv2.putText(
        image,
        "Selecione o tipo do alimento",
        (20, text_Y_position),
        FONT,
        1,
        (0, 255, 255),
        2,
        cv2.LINE_4,
    )

    text_Y_position = text_Y_position + 50

    global LAST_DETECTED_INGREDIENT
    for index, food_option in enumerate(FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT], 1):
        cv2.putText(
            image,
            f"{index}: {food_option.food_name} {food_option.variety}",
            (20, text_Y_position),
            FONT,
            1,
            (0, 255, 255),
            2,
            cv2.LINE_4,
        )

        text_Y_position = text_Y_position + 50


def draw_bounding_boxes(image, boxes, conn, cursor):
    global IS_INGREDIENT_DETECTED

    if IS_INGREDIENT_DETECTED:
        draw_ingredient_options_text(image)
        return

    for box in boxes:
        x1, y1, x2, y2 = box.xyxy[0]
        x1, y1, x2, y2 = (
            int(x1),
            int(y1),
            int(x2),
            int(y2),
        )
        food_id = int(box.cls[0])

        confidence = math.ceil((box.conf[0] * 100)) / 100

        if confidence >= CONFIDENCE_THRESHOLD and food_id != 9:
            global LAST_DETECTED_INGREDIENT

            food_options = get_foods_by_name(CLASS_NAMES_PTBR[food_id], conn, cursor)

            IS_INGREDIENT_DETECTED = True
            LAST_DETECTED_INGREDIENT = CLASS_NAMES_PTBR[food_id]
            FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT] = food_options

        confidence = math.ceil((box.conf[0] * 100)) / 100


def detect_webcam_video(user_id, model, conn, cursor):
    cap = cv2.VideoCapture(0)
    cap.set(3, 640)
    cap.set(4, 480)

    while True:
        _, image = cap.read()
        results = model(image, verbose=False, stream=True)
        global TOTAL_DAILY_CALORIC_INTAKE

        for result in results:
            draw_bounding_boxes(image, result.boxes, conn, cursor)
            cv2.putText(
                image,
                f"Total de Calorias: {TOTAL_DAILY_CALORIC_INTAKE}",
                (200, 460),
                FONT,
                1,
                (0, 255, 255),
                2,
                cv2.LINE_4,
            )

        cv2.imshow("Webcam", image)

        key = cv2.waitKey(1)
        global IS_INGREDIENT_DETECTED
        if key == ord(
            "c"
        ):  # Botão para optar por cancelar após um alimento ser identificado
            IS_INGREDIENT_DETECTED = False
            continue
        if key == ord("q"):  # Botão para optar por fechar o projeto
            print("Fechando a janela do projeto")
            break
        if key == ord("t"):  # Botão para obter o total de calorias consumidas no dia
            daily_totals = read_daily_totals(user_id, conn, cursor)

            display_total_daily_calories_consumed(daily_totals)
        elif (
            key == ord("1") and IS_INGREDIENT_DETECTED
        ):  # Botão para selecionar a opção 1 dos alimentos
            food_amount = get_foodscale_reading()

            selected_food = FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT][0]

            create_diary_entry(
                food_amount,
                selected_food.food_name,
                selected_food.variety,
                user_id,
                conn,
                cursor,
            )

            update_daily_totals(user_id, conn, cursor)
        elif (
            key == ord("2") and IS_INGREDIENT_DETECTED
        ):  # Botão para selecionar a opção 2 dos alimentos
            if len(FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT]) >= 2:
                food_amount = get_foodscale_reading()

                selected_food = FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT][1]

                create_diary_entry(
                    food_amount,
                    selected_food.food_name,
                    selected_food.variety,
                    user_id,
                    conn,
                    cursor,
                )

                update_daily_totals(user_id, conn, cursor)

        elif (
            key == ord("3") and IS_INGREDIENT_DETECTED
        ):  # Botão para selecionar a opção 3 dos alimentos
            if len(FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT]) >= 3:
                food_amount = get_foodscale_reading()

                if food_amount < 1:
                    print(
                        "não é possível salvar uma entrada no diário alimentício com um peso do alimento igual a 0"
                    )
                    IS_INGREDIENT_DETECTED = False
                    continue

                selected_food = FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT][2]

                create_diary_entry(
                    food_amount,
                    selected_food.food_name,
                    selected_food.variety,
                    user_id,
                    conn,
                    cursor,
                )

                update_daily_totals(user_id, conn, cursor)

        elif key == ord("4") and IS_INGREDIENT_DETECTED:
            if (
                len(FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT]) >= 4
            ):  # Botão para selecionar a opção 4 dos alimentos
                food_amount = get_foodscale_reading()

                selected_food = FOOD_TYPE_OPTIONS[LAST_DETECTED_INGREDIENT][3]

                create_diary_entry(
                    food_amount,
                    selected_food.food_name,
                    selected_food.variety,
                    user_id,
                    conn,
                    cursor,
                )

                update_daily_totals(user_id, conn, cursor)

    cap.release()
    cv2.destroyAllWindows()


def update_daily_totals(user_id, conn, cursor):
    daily_totals = read_daily_totals(user_id, conn=conn, cursor=cursor)

    display_total_daily_calories_consumed(daily_totals)

    if not (total_calories := daily_totals["total_calories"]):
        return

    global TOTAL_DAILY_CALORIC_INTAKE
    TOTAL_DAILY_CALORIC_INTAKE = total_calories if total_calories else 0

    global IS_INGREDIENT_DETECTED
    IS_INGREDIENT_DETECTED = False


def validate_user(email, password):
    if not (user := read_user(email, password)):
        print(
            f"Não foi encontrado nenhum usuário com o email '{email}' e a senha fornecida. Tente novamente.\n"
        )

        user_menu()

    return user


def user_menu():
    print("Você já possui cadastro? Digite a opção correspondente")
    print("1 - Sim")
    print("2 - Não")
    print("9 - Sair")

    selected_menu_option = input()

    if str(selected_menu_option) not in USER_MENU_OPTIONS:
        print("Opção inválida")
        exit()

    if selected_menu_option == EXIT_MENU:
        exit()

    if not (email := input("Insira seu email: ")):
        print("\nEmail não inserido! Tente novamente.\n")
        user_menu()

    if not (password := getpass("Insira sua senha:")):
        print("\nSenha não inserida! Tente novamente.\n")
        user_menu()

    if selected_menu_option == SIGNED_UP:
        user_id, conn, cursor = validate_user(email, password)
        if not user_id:
            user_menu()
    elif selected_menu_option == NOT_SIGNED_UP:
        user_id, conn, cursor = create_user(email, password)
        if not user_id:
            user_menu()

    return user_id, conn, cursor
