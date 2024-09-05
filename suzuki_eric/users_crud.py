import psycopg2
import config
from collections import namedtuple
from psycopg2 import errors
from psycopg2 import sql

UserInfo = namedtuple(
    "FoodInfo",
    [
        "id",
        "email",
        "password",
    ],
)


def create_user(email, password):
    try:
        conn, cursor = config.connect()

        insert_query = sql.SQL(
            """
            INSERT INTO users (email, password)
            VALUES (%s, %s)
            RETURNING id, created_at;
        """
        )

        cursor.execute(insert_query, (email, password))

        result = cursor.fetchone()

        if not result:
            return None, None, None

        conn.commit()

        user_id = result[0]

        print(f"Usuário criado com email '{email}'.\n")
        return user_id, conn, cursor

    except errors.UniqueViolation as e:
        print(f"O email '{email}' já existe no sistema. Insira um novo e-mail.\n")
        conn.rollback()
    except psycopg2.Error as e:
        print(f"Error: {e}")
        conn.rollback()

    return None, None, None


def update_user(email, current_password, new_password1, new_password2, conn, cursor):
    if new_password1 != new_password2:
        return "As senhas fornecidas não coincidem. Tente novamente"

    try:
        select_query = """
            SELECT id, email, password, created_at
            FROM users
            WHERE email = %s AND password = %s;
        """

        cursor.execute(select_query, (email, current_password))

        user = cursor.fetchone()

        if not user:
            return f"Não foi encontrado nenhum usuário com o email '{email}' e a senha fornecida. Tente novamente."

        update_query = """
            UPDATE users
            SET password = %s, created_at = CURRENT_TIMESTAMP
            WHERE email = %s
            RETURNING id, email, created_at;
        """

        cursor.execute(update_query, (new_password1, email))

        updated_user = cursor.fetchone()

        conn.commit()

        if updated_user:
            return "Senha do usuário atualizada com sucesso!"

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        return None


import psycopg2
from psycopg2 import sql


def delete_user(email, password, conn, cursor):
    try:
        delete_query = """
            DELETE FROM users
            WHERE email = %s AND password = %s
            RETURNING id, email, created_at;
        """

        cursor.execute(delete_query, (email, password))

        deleted_user = cursor.fetchone()

        conn.commit()

        if deleted_user:
            print(f"Usuário com email {email} foi removido com sucesso.")
            return deleted_user
        else:
            print(f"Não foi encontrado um usuário com o email {email}")
            return None

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        if conn:
            conn.rollback()
        return None


def read_user(email, password):
    try:
        conn, cursor = config.connect()

        select_query = """
            SELECT id
            FROM users
            WHERE email = %s AND password = %s;
        """

        cursor.execute(select_query, (email, password))

        result = cursor.fetchone()

        if not result:
            print(
                f"\nNão foi encontrado nenhum usuário com o email '{email}' e a senha fornecida. Tente novamente.\n"
            )
            return None, None, None

        user_id = result[0]

        return user_id, conn, cursor

    except psycopg2.Error as e:
        print(f"Database error: {e}")
        return None
