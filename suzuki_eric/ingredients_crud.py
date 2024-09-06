import psycopg2
import config
import json

from collections import namedtuple

FoodInfo = namedtuple(
    "FoodInfo",
    [
        "id",
        "food_name",
        "variety",
        "protein_per_gram",
        "carbs_per_gram",
        "fats_per_gram",
        "calories_per_gram",
        "last_updated",
        "additional_info",
    ],
)


def create_foods_table():
    """Cria a tabela `foods` no banco de dados da AWS"""
    try:
        conn, cursor = config.connect()

        create_table_query = """
        CREATE TABLE IF NOT EXISTS foods (
            id SERIAL PRIMARY KEY,
            food_name VARCHAR(255) NOT NULL,
            variety VARCHAR(255) NOT NULL,
            protein_per_gram NUMERIC(10, 2),
            carbs_per_gram NUMERIC(10, 2),
            fats_per_gram NUMERIC(10, 2),
            calories_per_gram NUMERIC(10, 2),
            last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            additional_info JSONB,
            CONSTRAINT unique_food_variety UNIQUE (food_name, variety)
        );
        """

        cursor.execute(create_table_query)
        conn.commit()

        print("Tabela 'foods' criada com sucesso!")

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        if conn:
            conn.rollback()
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()


def create_food(
    food_name: str,
    variety: str,
    protein_per_gram,
    carbs_per_gram,
    fats_per_gram: float,
    calories_per_gram: float,
    additional_info={},
    conn=None,
    cursor=None,
):
    """Cria um alimento no banco de dados.

    Args:
        food_name: nome da comida.
        variety: variedade do alimento.
        protein_per_gram: quantidade de proteína por grama.
        carbs_per_gram: quantidade de carboidratos por grama.
        fats_per_gram: quantidade de gorduras por grama.
        calories_per_gram: quantidade total de calorias por grama.
        additional_info: Informações adicionais como por exemplo micronutrientes.
                         Retorna por default {}.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        A comida adicionada no banco de dados em caso de sucesso ou None em casos de erro.
    """
    try:
        insert_query = """
        INSERT INTO foods (food_name, variety, protein_per_gram, carbs_per_gram, fats_per_gram, calories_per_gram, additional_info)
        VALUES (%s, %s, %s, %s, %s, %s, %s)
        RETURNING id, food_name, variety, protein_per_gram, carbs_per_gram, fats_per_gram, calories_per_gram, last_updated;
        """

        additional_info_json = json.dumps(additional_info)

        cursor.execute(
            insert_query,
            (
                food_name,
                variety,
                protein_per_gram,
                carbs_per_gram,
                fats_per_gram,
                calories_per_gram,
                additional_info_json,
            ),
        )

        inserted_food = cursor.fetchone()
        conn.commit()
        return inserted_food

    except psycopg2.IntegrityError as e:
        print(f"Integrity error: {e}")
        conn.rollback()
        return None
    except psycopg2.Error as e:
        print(f"Database error: {e}")
        conn.rollback()
        return None


def delete_food(food_name, variety, conn, cursor):
    """Deleta um alimento do banco de dados.

    Args:
        food_name: nome da comida.
        variety: variedade do alimento.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        A comida deletada do banco de dados em caso de sucesso ou None em casos de erro.
    """
    try:
        delete_query = """
        DELETE FROM foods
        WHERE food_name = %s AND variety = %s
        RETURNING id, food_name, variety;
        """

        cursor.execute(delete_query, (food_name, variety))

        deleted_food = cursor.fetchone()
        conn.commit()

        if deleted_food:
            return deleted_food
        else:
            print(
                f"Não foi encontrada uma comida com o nome {food_name} e variedade {variety}"
            )
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        conn.rollback()
        return None


def update_food(
    food_name,
    variety,
    protein_per_gram=None,
    carbs_per_gram=None,
    fats_per_gram=None,
    calories_per_gram=None,
    additional_info=None,
    conn=None,
    cursor=None,
):
    """Atualiza um alimento no banco de dados.

    Args:
        food_name: nome da comida.
        variety: variedade do alimento.
        protein_per_gram: quantidade de proteína por grama.
        carbs_per_gram: quantidade de carboidratos por grama.
        fats_per_gram: quantidade de gorduras por grama.
        calories_per_gram: quantidade total de calorias por grama.
        additional_info: Informações adicionais como por exemplo micronutrientes.
                         Retorna por default {}.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        A comida atualizada no banco de dados em caso de sucesso ou None em casos de erro.
    """
    try:
        update_query = """
        UPDATE foods
        SET
            protein_per_gram = COALESCE(%s, protein_per_gram),
            carbs_per_gram = COALESCE(%s, carbs_per_gram),
            fats_per_gram = COALESCE(%s, fats_per_gram),
            calories_per_gram = COALESCE(%s, calories_per_gram),
            additional_info = COALESCE(%s, additional_info),
            last_updated = CURRENT_TIMESTAMP
        WHERE food_name = %s AND variety = %s
        RETURNING id, food_name, variety, protein_per_gram, carbs_per_gram, fats_per_gram, calories_per_gram, last_updated;
        """

        cursor.execute(
            update_query,
            (
                protein_per_gram,
                carbs_per_gram,
                fats_per_gram,
                calories_per_gram,
                additional_info,
                food_name,
                variety,
            ),
        )

        updated_food = cursor.fetchone()

        conn.commit()

        if updated_food:
            return updated_food
        else:
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        conn.rollback()
        return None


def read_food(food_name, variety, conn, cursor):
    """Busca um alimento específico no banco de dados baseado no nome e variedade.

    Args:
        food_name: nome da comida.
        variety: variedade do alimento.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        O alimento encontrado em caso de sucesso ou None em caso de erro.
    """
    try:
        select_query = """
        SELECT id, food_name, variety, protein_per_gram, carbs_per_gram, fats_per_gram, calories_per_gram, last_updated, additional_info
        FROM foods
        WHERE food_name = %s AND variety = %s;
        """

        cursor.execute(select_query, (food_name, variety))

        food = cursor.fetchone()

        if food:
            return FoodInfo(*food)
        else:
            print("No food found with the provided name and variety.")
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        return None


def get_foods_by_name(food_name, conn, cursor):
    """Busca alimentos no banco de dados baseado no nome.

    Args:
        food_name: nome da comida.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        Os alimentos encontrados em caso de sucesso ou None em caso de erro.
    """
    try:
        query = """
        SELECT * FROM foods
        WHERE food_name = %s;
        """
        cursor.execute(query, (food_name,))
        foods = cursor.fetchall()

        foods_found = []
        if foods:
            for food in foods:
                foods_found.append(FoodInfo(*food))
            return foods_found
        else:
            print(f"No foods found with name '{food_name}'.")
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        if cursor:
            cursor.close()
        if conn:
            conn.close()
        return None
