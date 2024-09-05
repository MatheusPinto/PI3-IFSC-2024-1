import config
import psycopg2
from ingredients_crud import read_food
from datetime import datetime
from datetime import date


def create_food_diary_table():
    """Cria a tabela de diário de alimentos consumidos no banco de dados"""
    try:
        conn, cursor = config.connect()

        create_table_query = """
        CREATE TABLE IF NOT EXISTS food_diary (
            id SERIAL PRIMARY KEY,
            user_id INT NOT NULL,
            food_id INT NOT NULL,
            amount_grams NUMERIC(10, 2) NOT NULL,
            entry_date DATE NOT NULL,
            total_protein NUMERIC(10, 2) NOT NULL,
            total_carbs NUMERIC(10, 2) NOT NULL,
            total_fats NUMERIC(10, 2) NOT NULL,
            total_calories NUMERIC(10, 2) NOT NULL,
            FOREIGN KEY (food_id) REFERENCES foods(id)
        );
        """

        cursor.execute(create_table_query)
        conn.commit()

        print("Tabela 'food diary' criada com sucesso!")

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        if conn:
            conn.rollback()
    # finally:
    #     if cursor:
    #         cursor.close()
    #     if conn:
    #         conn.close()


def create_diary_entry(amount_grams, food_name, variety, user_id, conn, cursor):
    """Cria uma entrada do alimento na tabela de alimentos consumidos.

    Args:
        amount_grams: total de gramas do alimento consumido.
        food_name: nome da comida.
        variety: variedade do alimento.
        user_id: id do usuário.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        Total de calorias consumidas ou None em caso de erro.
    """
    food_info = read_food(food_name, variety, conn, cursor)

    if not food_info:
        "O alimento buscado não foi encontrado no banco de dados."

    try:
        total_protein = float(food_info.protein_per_gram) * amount_grams
        total_carbs = float(food_info.carbs_per_gram) * amount_grams
        total_fats = float(food_info.fats_per_gram) * amount_grams
        total_calories = float(food_info.calories_per_gram) * amount_grams

        entry_date = datetime.now().strftime("%Y-%m-%d")

        insert_query = """
        INSERT INTO food_diary (user_id, food_id, amount_grams, entry_date, total_protein, total_carbs, total_fats, total_calories)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
        RETURNING id, user_id, food_id, amount_grams, entry_date, total_protein, total_carbs, total_fats, total_calories;
        """
        cursor.execute(
            insert_query,
            (
                user_id,
                food_info.id,
                amount_grams,
                entry_date,
                total_protein,
                total_carbs,
                total_fats,
                total_calories,
            ),
        )

        new_entry = cursor.fetchone()
        conn.commit()

        print(
            "\nAlimento adicionado:\n"
            f"Quantia: {new_entry[3]}g,\n"
            f"Proteína: {new_entry[5]}g\n"
            f"Carboidratos: {new_entry[6]}\n"
            f"Gorduras: {new_entry[7]}\n"
            f"Calorias totais: {new_entry[8]}\n"
        )
        return total_calories

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        conn.rollback()
        return None


def read_daily_totals(user_id, conn, cursor):
    """Lê o total de calorias e macro-nutrientes consumidos no dia.

    Args:
        user_id: id do usuário
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
       Total de calorias consumidas ou None em caso de erros.
    """
    current_date = date.today().isoformat()

    try:
        # SQL query to get the totals for the current day
        query = """
        SELECT
        SUM(total_protein) AS total_protein,
        SUM(total_carbs) AS total_carbs,
        SUM(total_fats) AS total_fats,
        SUM(total_calories) AS total_calories
        FROM food_diary
        WHERE DATE(entry_date) = %s AND user_id = %s;
        """

        cursor.execute(query, (current_date, user_id))
        result = cursor.fetchone()
        conn.commit()

        if result:
            total_protein, total_carbs, total_fats, total_calories = result

            return {
                "total_protein": total_protein,
                "total_carbs": total_carbs,
                "total_fats": total_fats,
                "total_calories": total_calories,
            }
        else:
            print("No entries for today.")
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        return None


def read_daily_totals_by_date(user_id, specific_date, conn, cursor):
    """Lê o total de calorias e macro-nutrientes para um dia específico.

    Args:
        user_id: id do usuário.
        specific_date (_type_): data específica para ser utilizada como filtro.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
       Os totais de calorias e macro nutrientes consumidos para um dia específico
    """
    try:
        query = """
        SELECT
        SUM(total_protein) AS total_protein,
        SUM(total_carbs) AS total_carbs,
        SUM(total_fats) AS total_fats,
        SUM(total_calories) AS total_calories
        FROM food_diary
        WHERE DATE(entry_date) = %s AND user_id = %s;
        """
        cursor.execute(query, (specific_date, user_id))
        result = cursor.fetchone()
        conn.commit()

        if result:
            total_protein, total_carbs, total_fats, total_calories = result
            return {
                "total_protein": total_protein if total_protein else 0,
                "total_carbs": total_carbs if total_carbs else 0,
                "total_fats": total_fats if total_fats else 0,
                "total_calories": total_calories if total_calories else 0,
            }
        else:
            print(f"No entries for {specific_date}.")
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        return None


def delete_today_entries(user_id):
    """Delete todas os alimentos consumidos para o dia atual para um usuário.

    Args:
        user_id: id do usuário.
    """
    try:
        conn, cursor = config.connect()

        current_date = datetime.now().strftime("%Y-%m-%d")

        delete_query = """
        DELETE FROM food_diary
        WHERE DATE(entry_date) = %s AND user_id = %s;
        """
        cursor.execute(delete_query, (current_date, user_id))
        conn.commit()

        print(
            f"Deletado todas entradas no diario nutricional para o usuário de id {user_id} na data {current_date}."
        )

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        if conn:
            conn.rollback()
        return None
