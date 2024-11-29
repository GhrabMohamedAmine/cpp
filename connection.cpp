#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection() {}

bool Connection::createconnect() {
    bool test = false;

    // Configurer la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("CPP_Projet");  // Nom de la source de données
    db.setUserName("amine");           // Nom d'utilisateur
    db.setPassword("esprit");          // Mot de passe

    // Ouvrir la connexion
    if (db.open()) {
        test = true;
        qDebug() << "Connexion à la base de données réussie.";
    } else {
        qDebug() << "Erreur de connexion à la base de données:" << db.lastError().text();
        qDebug() << "Message d'erreur complet : " << db.lastError().driverText();
    }

    return test;
}
