#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include "event.h"
#include "connection.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Essayer de se connecter à la base de données
    Connection connection;
    if (!connection.createconnect()) {
        QMessageBox::critical(this, "Erreur de connexion", "La connexion à la base de données a échoué.");
        return;
    }

    // Rafraîchir la table au démarrage
    on_refreshButton_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addButton_clicked() {
    // Récupérer les données saisies par l'utilisateur
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QString date = ui->dateLineEdit->text();
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->text();
    QString duration = ui->durationLineEdit->text();
    float price = ui->priceLineEdit->text().toFloat();

    // Valider les données saisies
    if (id <= 0) {
        QMessageBox::warning(this, "Entrée invalide", "L'ID ne peut pas être nul ou négatif.");
        return;
    }
    if (name.isEmpty() || date.isEmpty() || type.isEmpty() || duration.isEmpty()) {
        QMessageBox::warning(this, "Entrée invalide", "Tous les champs doivent être remplis.");
        return;
    }

    // Créer un objet Event et définir ses propriétés
    Event event;
    event.setId(id);
    event.setName(name);
    event.setDate(date);
    event.setCapacity(capacity);
    event.setType(type);
    event.setDuration(duration);
    event.setPrice(price);

    // Ajouter l'événement à la base de données
    if (event.addEvent()) {
        QMessageBox::information(this, "Succès", "Événement ajouté avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'événement. Consultez les logs pour plus de détails.");
    }

    // Rafraîchir l'interface après ajout
    on_refreshButton_clicked();
}

void MainWindow::on_refreshButton_clicked() {
    // Vérification de la connexion à la base de données
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "La base de données n'est pas valide.";
        return;
    }

    if (!db.isOpen()) {
        qDebug() << "La base de données n'est pas ouverte.";
        qDebug() << "Erreur :" << db.lastError().text();
        return;
    }

    qDebug() << "Connexion à la base de données établie et valide.";

    // Vous pouvez ici ajouter le code pour rafraîchir la table ou la vue des événements
}

void MainWindow::on_modifyButton_clicked() {
    // Implémentez la logique de modification
}

void MainWindow::on_deleteButton_clicked() {
    int id = ui->idDeleteLineEdit->text().toInt();

    QSqlQuery query;
    query.prepare("DELETE FROM EVENTS WHERE ID_EVENT = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        qDebug() << "Événement supprimé avec succès.";
    } else {
        qDebug() << "Erreur lors de la suppression de l'événement:" << query.lastError().text();
    }

    on_refreshButton_clicked();
}
