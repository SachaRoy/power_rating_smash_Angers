#include <iostream>
#include <sqlite3.h>
#include <vector>


int main() {
    sqlite3* DB;
    const char* file = "data_base.db";

    int rc = sqlite3_open(file, &DB);

    if(rc) {
        std::cout << "BD non trouvée" << std::endl;
    }
    else {
        std::cerr << "BD connectée" << std::endl;

        int nb;
        std::cout << "Combien de tournois voulez-vous ajouter ? " << std::flush;
        std::cin >> nb;

        if(nb > 0) {
            std::string name, s, n, n_joueurs, date, link;
            sqlite3_stmt *stmt;
            for(int i{0}; i<nb; i++) {
                std::cout << "Nom du tournoi: " << std::flush;
                std::cin >> name;

                std::cout << "Saison du tournoi: " << std::flush;
                std::cin >> s;

                std::cout << "Numéro du tournoi: " << std::flush;
                std::cin >> n;

                std::cout << "Nombre de joueurs: " << std::flush;
                std::cin >> n_joueurs;

                std::cout << "Date du tournoi: " << std::flush;
                std::cin >> date;

                std::cout << "Lien du tournoi: " << std::flush;
                std::cin >> link;


                sqlite3_prepare(DB, "INSERT INTO Tournois(nom, saison, numéro, nb_joueurs, date, lien) VALUES(?,?,?,?,?,?)", -1, &stmt, 0);
                sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), NULL);
                sqlite3_bind_text(stmt, 2, s.c_str(), s.size(), NULL);
                sqlite3_bind_text(stmt, 3, n.c_str(), n.size(), NULL);
                sqlite3_bind_text(stmt, 4, n_joueurs.c_str(), n_joueurs.size(), NULL);
                sqlite3_bind_text(stmt, 5, date.c_str(), date.size(), NULL);
                sqlite3_bind_text(stmt, 6, link.c_str(), link.size(), NULL);


                if(sqlite3_step(stmt) == SQLITE_DONE) std::cout << name << " du " << date << " enregistré" << std::endl;
                else std::cout << "L'enregistrement a échoué." << std::endl;
                sqlite3_reset(stmt);
            }
        }


        std::cout << "Combien de joueurs voulez-vous ajouter ? " << std::flush;
        std::cin >> nb;

        if(nb > 0) {
            std::string p;
            sqlite3_stmt *stmt;
            for(int i{0}; i<nb; i++) {
                std::cout << "Pseudo du joueur: " << std::flush;
                std::cin >> p;

                sqlite3_prepare(DB, "INSERT INTO Joueurs(pseudo) VALUES(?)", -1, &stmt, 0);
                sqlite3_bind_text(stmt, 1, p.c_str(), p.size(), NULL);

                if(sqlite3_step(stmt) == SQLITE_DONE) std::cout << p << " enregistré" << std::endl;
                else std::cout << "L'enregistrement a échoué." << std::endl;
                sqlite3_reset(stmt);
            }
        }

        std::cout << "Combien de résultats voulez-vous ajouter ? " << std::flush;
        std::cin >> nb;

        if(nb > 0) {
            std::string p1, p2, id, s1, s2;
            sqlite3_stmt *stmt;
            for(int i{0}; i<nb; i++) {
                std::cout << "Pseudo du gagnant: " << std::flush;
                std::cin >> p1;

                std::cout << "Pseudo du perdant: " << std::flush;
                std::cin >> p2;
                
                std::cout << "ID du tournoi: " << std::flush;
                std::cin >> id;
                
                std::cout << "Score du gagnant: " << std::flush;
                std::cin >> s1;
                
                std::cout << "Score du perdant: " << std::flush;
                std::cin >> s2;

                sqlite3_prepare(DB, "INSERT INTO Résultats(joueur1, joueur2, ID_tournoi, score_j1, score_j2) VALUES(?,?,?,?,?)", -1, &stmt, 0);
                sqlite3_bind_text(stmt, 1, p1.c_str(), p1.size(), NULL);
                sqlite3_bind_text(stmt, 2, p2.c_str(), p2.size(), NULL);
                sqlite3_bind_text(stmt, 3, id.c_str(), id.size(), NULL);
                sqlite3_bind_text(stmt, 4, s1.c_str(), s1.size(), NULL);
                sqlite3_bind_text(stmt, 5, s2.c_str(), s2.size(), NULL);

                if(sqlite3_step(stmt) == SQLITE_DONE) std::cout << "Afrontement de " << p1 << " contre " << p2 << " enregistré." << std::endl;
                else std::cout << "L'enregistrement a échoué." << std::endl;
                sqlite3_reset(stmt);
            }
        }
        
        sqlite3_close(DB);
    }
    
    return 0;
}