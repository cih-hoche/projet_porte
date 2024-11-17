const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const bodyParser = require('body-parser');
const app = express();
const DB_PATH = 'porte.db';

app.use(bodyParser.json());

let db = new sqlite3.Database(DB_PATH, (err) => {
    if (err) {
        console.error(err.message);
    }
    console.log('Connected to the porte database.');
});

app.get('/', (req, res) => {
    res.send('/docs');
});

app.get('/api/users/get', (req, res) => {
    db.all('SELECT * FROM users', [], (err, rows) => {
        if (err) {
            throw err;
        }
        res.set('Content-Type', 'text/plain');
        res.send(rows.map(formatUserMinimal).join('\n'));
    });
});

app.get('/api/doorallowed/:nfc_card_id', (req, res) => {
    const nfc_card_id = req.params['nfc_card_id'];
    res.set('Content-Type', 'text/plain');
    db.get('SELECT * FROM users WHERE nfc_card_id = ?', [nfc_card_id], (err, row) => {
        if (err) {
            throw err;
        }
        if (!row) {
            return res.send('0#not found');
        }
        const now = new Date();
        const expiry = new Date(row['expiration_date'].toString());
        if (row['expiration_date'] === '-1' || row['expiration_date'] === '-2' || now < expiry) {
            return res.send('1#ok');
        }
        res.send('0#expired');
    });
});

app.get('/api/doorlogs/add', (req, res) => {
    const { date_time, card_id } = req.query;
    db.run('INSERT INTO logs (datetime, card_id) VALUES (?, ?)', [date_time, card_id], (err) => {
        if (err) {
            return res.status(400).send('Error inserting log');
        }
        res.send('ok');
    });
});

// app.get('/users/get/:username', (req, res) => {
//     const username = req.params.username;
//     db.get('SELECT * FROM users WHERE username = ?', [username], (err, row) => {
//         if (err) {
//             throw err;
//         }
//         res.send(formatUser(row));
//     });
// });
//
// app.get('/users/add', (req, res) => {
//     const { username, nfc_card_id, status, expiration_date } = req.query;
//     let expiry = expiration_date;
//     if (expiration_date !== '-1' && expiration_date !== '-2') {
//         try {
//             expiry = new Date(expiration_date).toISOString();
//         } catch (e) {
//             return res.status(400).send('Invalid date format');
//         }
//     }
//     db.run('INSERT INTO users (username, nfc_card_id, status, expiration_date) VALUES (?, ?, ?, ?)', [username, nfc_card_id, status, expiry], (err) => {
//         if (err) {
//             return res.status(400).send('Error adding user');
//         }
//         res.send('ok');
//     });
// });
//
// app.get('/users/isactive/:username', (req, res) => {
//     const username = req.params.username;
//     db.get('SELECT * FROM users WHERE username = ?', [username], (err, row) => {
//         if (err) {
//             throw err;
//         }
//         if (!row) {
//             return res.send('0#not found');
//         }
//         const now = new Date();
//         const expiry = new Date(row.expiration_date);
//         if (row.expiration_date === '-1' || row.expiration_date === '-2' || now < expiry) {
//             return res.send('1#ok');
//         }
//         res.send('0#expired');
//     });
// });
//
//
//
// app.get('/logs', (req, res) => {
//     db.all('SELECT * FROM logs', [], (err, rows) => {
//         if (err) {
//             throw err;
//         }
//         res.send(rows.map(formatLog).join('\n'));
//     });
// });

function formatUser(user) {
    return `${user.id} # ${user.username} # ${user['nfc_card_id']} # ${user.status} # ${user['expiration_date']}`;
}

function formatUserMinimal(user) {
    return `${user['nfc_card_id']};${user['expiration_date']}`;
}

function formatLog(log) {
    return `${log['datetime']} # ${log['card_id']}`;
}

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});