Modèle relationnel de la base de donnée:
Joueurs (pseudo, rating, RD)
Résultats (ID, joueur1, joueur2, ID_tournoi, score_j1, score_j2)
Tournois (ID, nom, saison, numéro, nb_joueurs, date, lien)

SGBD utilisé: SQLite

"enregistrement.cpp" sert à rajouter des joueurs, résultats et tournois dans la BD.
"calc_glicko.cpp" sert à update le rating de chaque joueur afin de créer un classement des joueurs, il est nécessaire de compiler le fichier avec "datelib.cpp".
