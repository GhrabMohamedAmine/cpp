#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection {
public:
    Connection();           // Constructeur
    bool createconnect();   // Méthode pour établir la connexion
};

#endif // CONNECTION_H
