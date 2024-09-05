import psycopg2
import config as creds

PG_ENDPOINT = "pi3-db.cvridvfsmzai.us-east-1.rds.amazonaws.com"
PG_DATABASE_NAME = "projeto_integrador_3_db"
PG_USERNAME = "postgres"
PG_PASSWORD = ""

# NOTE (@eric_reis): Comando para acessar o banco de dados utilizando a lib "psql":
# psql \
#    --host=pi3-db.cvridvfsmzai.us-east-1.rds.amazonaws.com \
#    --port=5432 \
#    --username=postgres \
#    --password \
#    --dbname=projeto_integrador_3_db


def connect():
    connection_config = (
        "host="
        + creds.PG_ENDPOINT
        + " port="
        + "5432"
        + " dbname="
        + creds.PG_DATABASE_NAME
        + " user="
        + creds.PG_USERNAME
        + " password="
        + creds.PG_PASSWORD
    )

    print("\nInitializing connection to:")
    print(connection_config)
    conn = psycopg2.connect(connection_config)
    print("connected")
    cursor = conn.cursor()

    return conn, cursor
