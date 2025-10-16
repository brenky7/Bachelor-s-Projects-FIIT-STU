from fastapi import FastAPI, HTTPException, APIRouter
from dotenv import load_dotenv
import os
import psycopg2

router = APIRouter()


def connect_to_database():
    load_dotenv()
    db_host = os.getenv('DATABASE_HOST')
    db_port = os.getenv('DATABASE_PORT')
    db_name = os.getenv('DATABASE_NAME')
    db_user = os.getenv('DATABASE_USER')
    db_password = os.getenv('DATABASE_PASSWORD')

    conn = psycopg2.connect(
        host=db_host,
        port=db_port,
        database=db_name,
        user=db_user,
        password=db_password
    )
    return conn


@router.get('/v1/status')
def get_database_version():
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("SELECT version();")
        db_version = cursor.fetchone()[0]
        cursor.close()
        conn.close()
        response = {'version': db_version}
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))



