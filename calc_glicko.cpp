#include <iostream>
#include <sqlite3.h>
#include <cmath>
#include <fstream>
#include "datelib.hpp"
#define _USE_MATH_DEFINES


struct results {
    // Structure permettant de stocker et exploiter les données obtenues par une requête SQL
    int n;
    int c;
    std::string* data;
};

static int callback(void *NotUsed, int argc, char **argv, char **szColName) {
    // Fonction de callback qui va sotcker le résultat de la requête dans le fichier auxilaire
    std::ofstream file("query_results.txt", std::ios::app);
    if(file) {
        for(int i{0}; i<argc; i++) {
            file << argv[i] << " ";
        }
        file << std::endl;
    }
    else {
        std::cout << "Le fichier n'a pas pu être ouvert." << std::endl;
    }
    return 0;
}

int exec_query(sqlite3* DB, const std::string query, char* &ErrMsg) {
    // Execute la requête "query"
    return sqlite3_exec(DB, query.c_str(), callback, NULL, &ErrMsg);
}

void clear_file(std::string f) {
    // Vide le fichier auxiliaire
    std::ofstream file(f.c_str());
    if(file) {
        file << "";
    }
    else {
        std::cout << "Le fichier n'a pas pu être ouvert." << std::endl;
    }
}

results read_file(std::string f) {
    // Récupère les données du fichier auxiliaire
    std::ifstream file(f);
    results rep;
    if(file) {
        int n, c;
        file >> n;
        file >> c;
        std::string* temp = new std::string[n*c];
        for(int i{0}; i<n*c; i++) {
            file >> temp[i];
        }
        rep.data = temp;
        rep.n = n;
        rep.c = c;
    }
    else {
        std::cout << "Le fichier n'a pas pu être ouvert." << std::endl;
    }
    return rep;
}


const float Q = std::log(10)/400;
const float C = 34.6;

float min(float n1, float n2) {
    // Calcul le minimum en tre "n1" et "n2"
    if(n1 > n2) return n2;
    else return n1;
}

int diff_dates(results date) {
    // Calcul le nombre de jours depuis le dernier tournoi
    if(date.n>1) {
        int j1 = (date.data[0][0]-'0')*10 + date.data[0][1]-'0';
        int m1 = (date.data[0][3]-'0')*10 + date.data[0][4]-'0';
        int a1 = (date.data[0][6]-'0')*1000 + (date.data[0][7]-'0')*100 + (date.data[0][8]-'0')*10 + date.data[0][9]-'0';

        int j2 = (date.data[1][0]-'0')*10 + date.data[1][1]-'0';
        int m2 = (date.data[1][3]-'0')*10 + date.data[1][4]-'0';
        int a2 = (date.data[1][6]-'0')*1000 + (date.data[1][7]-'0')*100 + (date.data[1][8]-'0')*10 + date.data[1][9]-'0';

        Date d1 = {j1, m1, a1};
        Date d2 = {j2, m2, a2};
        
        return getDifference(d1, d2);
    }
    else {
        return 0;
    }
}

float g(float RD) {
    // Fonction g
    return 1/(std::sqrt(1+(3*Q*Q*RD*RD)/M_PI*M_PI));
}

float E(float r0, float r, float RD) {
    // Fonction E
    return 1/(1+std::pow(10, g(RD)*(r0-r)/(-400)));
}

float d2(results adv) {
    // Calcule d²
    float sum = 0;
    float temp, temp_g, temp_e;
    for(int i{0}; i<adv.n*adv.c; i+=adv.c) {
        temp = std::stof(adv.data[i+2]);
        temp_g = g(temp);
        temp_e = E(std::stof(adv.data[i+1]), std::stof(adv.data[i+1]), temp);

        sum += temp_g*temp_g*temp_e*(1-temp_e);
    }
    return 1/(Q*Q*sum);
}

float temp_RD(float RD, int t) {
    // Calcul un RD temporraire en fonction du temps écoulé depuis le dernier tournoi
    return min(std::sqrt(RD*RD + C*C*t), 350);
}

float new_RD(float RD, float d2) {
    // Calcul le nouveau RD qui sera stocké dans le BD
    return std::sqrt(1/(1/RD*RD + 1/d2));
}

float new_rating(results &adv, float r0, float RD, float d2) {
    // Calcul le nouveau rating d'un joueur
    float rep = r0;
    float sum = 0;
    for(int i{0}; i<adv.n*adv.c; i+=adv.c) {
        sum += g(std::stof(adv.data[i+2]))*(std::stoi(adv.data[i+3])-E(r0, std::stof(adv.data[i+1]), std::stof(adv.data[i+2])));
    }
    rep += (Q/(1/(RD*RD)+1/d2))*sum;
    return rep;
}


int main() {
    sqlite3* DB;
    char* ErrMsg = 0;
    const char* file = "data_base.db";

    int rc = sqlite3_open(file, &DB);

    if(rc) {
        std::cout << "BD non trouvée" << std::endl;
    }
    else {
        std::cout << "BD connectée" << std::endl;
        int id, t;
        std::cout << "Rentrer l'ID du dernier tournoi: " << std::flush;
        std::cin >> id;

        clear_file("query_results.txt");

        // Récupération des joueurs ayant participé au dernier tournoi
        std::string query;
        query = "SELECT COUNT(DISTINCT j.pseudo) FROM Joueurs AS j JOIN Résultats AS r ON r.joueur1 = j.pseudo OR r.joueur2 = j.pseudo WHERE r.ID_tournoi = "+std::to_string(id)+";";
        rc = exec_query(DB, query, ErrMsg);

        std::ofstream f1("query_results.txt", std::ios::app);
        f1 << 3 << std::endl;
        f1.close();

        query = "SELECT DISTINCT j.pseudo, j.rating, j.RD FROM Joueurs AS j JOIN Résultats AS r ON r.joueur1 = j.pseudo OR r.joueur2 = j.pseudo WHERE r.ID_tournoi = "+std::to_string(id)+";";
        rc = exec_query(DB, query, ErrMsg);

        results part = read_file("query_results.txt");
        clear_file("query_results.txt");


        // Récupération des dates des 2 derniers tournois
        query = "SELECT COUNT(date) FROM Tournois WHERE ID = "+std::to_string(id)+" OR ID = "+std::to_string(id-1)+";";
        rc = exec_query(DB, query, ErrMsg);

        std::ofstream f2("query_results.txt", std::ios::app);
        f2 << 1 << std::endl;
        f2.close();

        query = "SELECT date FROM Tournois WHERE ID = "+std::to_string(id)+" OR ID = "+std::to_string(id-1)+";";
        rc = exec_query(DB, query, ErrMsg);
        
        results date = read_file("query_results.txt");
        clear_file("query_results.txt");

        t = diff_dates(date);


        // Calcul des ratings et deviation ratings de chaque joueur du dernier tournoi
        std::string name;
        float RD{350}, rating{1000}, d_squared{1};
        for(int i{0}; i<part.n*part.c; i+=part.c) {
            name = part.data[i];

            query = "SELECT COUNT(*) FROM (SELECT id,joueur1 AS pseudo FROM Résultats WHERE ID_tournoi = "+std::to_string(id)+" AND joueur2 = '"+name+"' UNION SELECT id, joueur2 AS pseudo FROM Résultats WHERE ID_tournoi = "+std::to_string(id)+" AND joueur1 = '"+name+"');";
            rc = exec_query(DB, query, ErrMsg);

            std::ofstream f2("query_results.txt", std::ios::app);
            f2 << 4 << std::endl;
            f2.close();

            query = "SELECT j.pseudo, j.rating, j.RD, s FROM Joueurs AS j JOIN (SELECT id, joueur1 AS p, '0' AS s FROM Résultats WHERE ID_tournoi = "+std::to_string(id)+" AND joueur2 = '"+name+"' UNION SELECT id, joueur2 AS p, '1' AS s FROM Résultats WHERE ID_tournoi = "+std::to_string(id)+" AND joueur1 = '"+name+"') ON j.pseudo = p;";
            rc = exec_query(DB, query, ErrMsg);
            results adv = read_file("query_results.txt");
            clear_file("query_results.txt");           


            d_squared = d2(adv);
            RD = temp_RD(std::stof(part.data[i+2]), t);
            rating = new_rating(adv, std::stof(part.data[i+1]), RD, d_squared);
            RD = new_RD(std::stof(part.data[i+2]), d_squared);

            part.data[i+1] = std::to_string(rating);
            part.data[i+2] = std::to_string(RD);
        }

        // Mise à jour de la BD
        for(int i{0}; i<part.n*part.c; i+=part.c) {
            query = "UPDATE Joueurs SET rating = '"+part.data[i+1]+"', RD = '"+part.data[i+2]+"' WHERE pseudo = '"+part.data[i]+"'";
            exec_query(DB, query, ErrMsg);
        }
    }
    return 0;
}