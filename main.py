DB_PATH = "porte.db"

if __name__ == "__main__":
    input("Being started manually, press Enter to continue...")
import os

if not os.path.exists("porte.db"):
    import db_setup

import sqlite3
from fastapi import FastAPI, Header, Query
from fastapi.responses import PlainTextResponse
from contextlib import asynccontextmanager
from typing import Annotated
import datetime

@asynccontextmanager
async def lifespan(app: FastAPI):
    print("Startup")
    yield
    print("Shutdown...")
    print("Shutdown complete")

app = FastAPI(lifespan=lifespan)

def add_user(username, nfc_card_id, status, expiration_date):
    #do this safely
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''INSERT INTO users (username, nfc_card_id, status, expiration_date) VALUES (?, ?, ?, ?)''', (username, nfc_card_id, status, expiration_date))
        conn.commit()

def remove_user(username):
    #actually just set expiration date to -1
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute('''UPDATE users SET expiration_date = -1 WHERE username = ?''', (username,))
    conn.commit()

def add_log(date_time, card_id):
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''INSERT INTO logs (datetime, card_id) VALUES (?, ?)''', (date_time, card_id))
        conn.commit()

def get_logs():
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''SELECT * FROM logs''')
        return cur.fetchall()

def get_all():
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''SELECT * FROM users''')
        return cur.fetchall()

def get_one(username):
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''SELECT * FROM users WHERE username = ?''', (username,))
        return cur.fetchall()

def nfc2user(nfc_card_id):
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute('''SELECT * FROM users WHERE nfc_card_id = ?''', (nfc_card_id,))
        return cur.fetchall()


# @app.get("/logs")
# def get_logs():
#     with sqlite3.connect(DB_PATH) as conn:
#         cur = conn.cursor()
#         cur.execute('''SELECT * FROM logs''')
#         print(cur.fetchall())
#         return "eee"

@app.get("/", response_class=PlainTextResponse)
def route_index():
    return "Bienvenue sur l'API de gestion porte\nAccedez a la docu interactive sur la page /docs"

def format_user(usr):
    return " # ".join(map(str,usr))

@app.get("/users/get", tags=["users"], response_class=PlainTextResponse)
def route_user_list():
    """
    Recup la liste des utilisateurs

    *format*: `id # username # nfc_card_id # status # expiration_date`\\
    *expiration_date*: est au format `YYYY-MM-DD HH:MM:SS`, sinon -1 pour un utilisateur permanent, -2 pour un utilisateur temporaire\\
    *status*: est 0 pour admin 😎 et 1 pour user 🤤\\
    *nfc_card_id*: est l'identifiant de la carte NFC (string)
    """
    string = "\n".join(map(format_user, get_all()))
    return string

@app.get("/logs/add", response_class=PlainTextResponse)
def route_log(date_time, card_id):
    #TODO: FAIRE FONCTIONNER LE HEADER X-SECRET-KEY
    """
    Ajoute une entree dans les logs
    
    *date_time*: est au format `YYYY-MM-DD HH:MM:SS`\\
    *card_id*: est l'identifiant de la carte NFC (string)
    """
    add_log(date_time, card_id)
    return "ok"

@app.get("/users/get/{username}", tags=["users"], response_class=PlainTextResponse)
def route_user_one(username: str):
    """
    Recup un utilisateur
    
    *format*: `id # username # nfc_card_id # status # expiration_date`\\
    *expiration_date*: est au format `YYYY-MM-DD HH:MM:SS`, sinon -1 pour un utilisateur permanent, -2 pour un utilisateur temporaire\\
    *status*: est 0 pour admin 😎 et 1 pour user 🤤\\
    *nfc_card_id*: est l'identifiant de la carte NFC (string)
    """
    return format_user(get_one(username))

@app.get("/users/add", tags=["users"]) #/!\ this was conflicting with the other route to get users
def route_user_add(
    username: str = Query(..., min_length=1),
    nfc_card_id: str = Query(..., min_length=1),
    status: int = Query(..., ge=0, le=1),
    expiration_date: str = Query(..., min_length=1)
):
    """
    Ajoute un utilisateur

    *username*: le nom d'utilisateur (string)\\
    *nfc_card_id*: l'identifiant de la carte NFC (string)\\
    *status*: 0 pour admin 😎 et 1 pour user 🤤\\
    *expiration_date*: est au format `DD/MM/YYYY HH:MM:SS`, sinon -1 pour un utilisateur permanent, -2 pour un utilisateur temporaire
    """
    if expiration_date == "-1" or expiration_date == "-2":
        pass
    else:
        try:
            expiry = datetime.datetime.strptime(expiration_date, "%d/%m/%Y %H:%M:%S")
        except ValueError:
            return "Invalid date format"
    add_user(username, nfc_card_id, status, expiry)
    return "ok"

def is_active_user(username):
    u = get_one(username)
    print(u)
    if u == []:
        return False, "not found"
    u = u[0]
    id, name, card_id, status, dtstring = u
    #check for expiry date (-1 being never)
    print(dtstring)
    if dtstring in ["-1", "-2"]:
        return True, "ok"
    #get current datetime
    now = datetime.datetime.now()
    #get expiry date
    expiry = datetime.datetime.strptime(dtstring, "%Y-%m-%d %H:%M:%S")
    #check if current date is before expiry date
    if now > expiry:
        return False, "expired"
    return True, "ok"

@app.get("/users/isactive/{username}", tags=["users"], response_class=PlainTextResponse)
def route_user_active(username: str):
    """
    Verifie si un utilisateur est actif

    reponse: `0#not found` si l'utilisateur n'existe pas, `0#expired` si l'utilisateur est expiré, `1#ok` si l'utilisateur est actif
    """
    s, r = is_active_user(username)
    return str(int(s)) + "#" + r

def is_allowed(nfc_card_id):
    u = nfc2user(nfc_card_id)
    if u == []:
        return False, "not found"
    u = u[0]
    id, name, card_id, status, dtstring = u
    #check for expiry date (-1 being never)
    if dtstring in ["-1", "-2"]:
        return True, "ok"
    #get current datetime
    now = datetime.datetime.now()
    #get expiry date
    expiry = datetime.datetime.strptime(dtstring, "%Y-%m-%d %H:%M:%S")
    #check if current date is before expiry date
    if now > expiry:
        return False, "expired"
    return True, "ok"

@app.get("/users/doorallowed/{nfc_card_id}", tags=["users"])
def route_user_door(nfc_card_id: str):
    """
    Verifie si un utilisateur est autorisé a ouvrir la porte

    reponse: `0#not found` si l'utilisateur n'existe pas, `0#expired` si l'utilisateur est expiré, `1#ok` si l'utilisateur est actif
    """
    s, r = is_allowed(nfc_card_id)
    return str(int(s)) + "#" + r

def format_log(log):
    return " # ".join(map(str,log))

@app.get("/logs", response_class=PlainTextResponse)
def route_logs(x_secret_key: Annotated[str | None, Header()] = None):
    #set X-Secret-Key to the secret key to add stuff to the logs
    return "\n".join(map(format_log, get_logs()))

    return x_secret_key
