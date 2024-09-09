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
    """Cria um usuário.

    Args:
        email: email do usuário.
        password: senha do usuário

    Returns:
        O id único do usuário, a conexão e cursor com o banco de dados em casos de sucesso ou
        None em todos parâmetros em caso de erro.
    """
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
    """Atualiza um usuário baseado em um email e uma senha

    Args:
        email: email do usuário.
        current_password: senha atual do usuário
        new_password1: senha do usuário a ser atualizada.
        new_password2: confirmação da senha do usuário a ser atualizada.
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        Mensagem em caso de sucesso ou em casos de erro retorna None
    """
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
    """Remove um usuário baseado em um email e senha.

    Args:
        email: email do usuário.
        password: senha do usuário
        conn: conexão utilizada para conectar no banco de dados.
        cursor: cursor utilizado para conectar no banco de dados.

    Returns:
        O usuário deletado em caso de sucesso ou None em caso de erro.
    """
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
    """Busca um usuário baseado no email e senha fornecidos

    Args:
        email: email do usuário.
        password: senha do usuário.

    Returns:
        O id único do usuário, a conexão e cursor com o banco de dados.
    """
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
