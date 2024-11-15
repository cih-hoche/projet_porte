# Bienvenue

Sur ce github, vous trouverez le code source du serveur web qui gère l'acces au club info.\
Vous êtes libres d'y contribuer ou de chercher des bugs pour vous crocheter un accès. 🔑

Quelques règles de contribution:
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
