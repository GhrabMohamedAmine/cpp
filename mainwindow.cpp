#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    // Rafraîchir la table au démarrage
    on_refreshButton_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QString date = ui->dateLineEdit->text();
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->text();
    QString duration = ui->durationLineEdit->text();
    float price = ui->priceLineEdit->text().toFloat();

    if (id <= 0) {
        qDebug() << "L'ID ne peut pas être nul ou négatif.";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO EVENTS (ID_EVENT, NAME, DATE_EVENT, CAPACITY, TYPE, DURATION, PRICE) "
                  "VALUES (:id, :name, :date, :capacity, :type, :duration, :price)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":date", date);
    query.bindValue(":capacity", capacity);
    query.bindValue(":type", type);
    query.bindValue(":duration", duration);
    query.bindValue(":price", price);

    if (query.exec()) {
        qDebug() << "Événement ajouté avec succès.";
    } else {
        qDebug() << "Erreur lors de l'ajout de l'événement:" << query.lastError().text();
    }

    on_refreshButton_clicked();
}

void MainWindow::on_refreshButton_clicked() {
    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);

    QSqlQuery query("SELECT * FROM EVENTS");
    if (!query.exec()) {
        qDebug() << "Erreur lors du rafraîchissement:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->eventTableWidget->insertRow(row);
        ui->eventTableWidget->setItem(row, 0, new QTableWidgetItem(query.value("ID_EVENT").toString()));
        ui->eventTableWidget->setItem(row, 1, new QTableWidgetItem(query.value("NAME").toString()));
        ui->eventTableWidget->setItem(row, 2, new QTableWidgetItem(query.value("DATE_EVENT").toString()));
        ui->eventTableWidget->setItem(row, 3, new QTableWidgetItem(query.value("CAPACITY").toString()));
        ui->eventTableWidget->setItem(row, 4, new QTableWidgetItem(query.value("TYPE").toString()));
        ui->eventTableWidget->setItem(row, 5, new QTableWidgetItem(query.value("DURATION").toString()));
        ui->eventTableWidget->setItem(row, 6, new QTableWidgetItem(query.value("PRICE").toString()));
        row++;
    }

    ui->eventTableWidget->resizeColumnsToContents();
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
