import sqlite3

# Connect to the database (creates a new file if it doesn't exist)
conn = sqlite3.connect('porte.db')

# Create a cursor object to execute SQL commands
cursor = conn.cursor()

# Create a table to hold usernames, user NFC card IDs, and status being a number (0 for root, 1 for user, 2 for guest), expiration date
#also create a table to hold logs with date, time, userid
cursor.execute('''
    CREATE TABLE users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL,
        nfc_card_id TEXT NOT NULL,
        status INTEGER NOT NULL,
        expiration_date TEXT NOT NULL
    )
''')

cursor.execute('''
    CREATE TABLE logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        datetime TEXT NOT NULL,
        card_id TEXT NOT NULL
    )
''')

cursor.execute('''
INSERT INTO users (username, nfc_card_id, status, expiration_date) VALUES ('root', 'root_id', 0, "-2")
''')
# Commit the changes and close the connection
conn.commit()
conn.close()