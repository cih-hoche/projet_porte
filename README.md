# Bienvenue

Sur ce github, vous trouverez le code source du serveur web et du contrôleur du moteur qui gèrent l'accès au club info.\
Vous êtes libres d'y contribuer ou de chercher des bugs pour vous crocheter un accès. 🔑

Quelques règles de contribution :
 - pas plus de fichiers annexes svp, embellissez le tas de nouilles qu'est le ptit main.py ❤
 - ... règles à suivre ...

<img src="https://github.com/user-attachments/assets/3b9e41fe-9b56-447d-b10e-31e326cdf7d1" width=50%>

## Commandes utiles

Installation des modules:\
`pip install fastapi[standard]`\
sqlite3 si vous l'avez pas

Demarrer en mode dev:
`fastapi dev .\main.py`

Demarrer en prod?
`fastapi run .\main.py`

## A faire:

Ajouter une clé secrète dans un .env pour que la porte puisse communiquer des logs +/- safe (header X-Secret-Key = ...)

## Esp32
*Programme à faire avec un esp32*

### Boucle principale:
#### Si le capteur nfc capte quelque chose
- Verification connexion au wifi
- Envoi de la requête pour savoir si l'utilisateur est autorisé
- Si oui, ouverture de la porte
- Envoi d'un log

Si pas connecté au wifi, vérification dans le fichier local + log dans le fichier local

#### Vérifie sa connexion au wifi toutes les 10 mins
- Si connecté, téléchargement du fichier d'autorisations + envoi des logs stockés + vidange du fichier de logs

#### Vérifie s'il n'y a pas de logs à envoyer toutes les 5 mins
- Si oui, connexion au wifi puis envoi des logs + suppressions des logs stockés.
  