#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include "event.h"
#include "connection.h"
#include <QTableView>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QPainter>
#include <event.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
        connect(ui->eventTableWidget, &QTableWidget::cellClicked, this, &MainWindow::on_eventTableWidget_cellClicked);
        Connection connection;
        if (!connection.createconnect()) {
            QMessageBox::critical(this, "Erreur de connexion", "La connexion à la base de données a échoué.");
            return;
        }
        QPieSeries *series = new QPieSeries();

        // Obtenez les pourcentages des événements
        Event event;
        QVector<int> percents = Event::getPercent();

        // Ajoutez les sections du graphique avec leurs noms et pourcentages
        series->append("PRIVE", percents[1] * 100 / percents[0]);
        series->append("PROFESSIONNEL", percents[2] * 100 / percents[0]);
        series->append("SAISIONNIER", percents[3] * 100 / percents[0]);
        series->append("CULTUREL", percents[4] * 100 / percents[0]);
        series->append("SPORTIF", percents[5] * 100 / percents[0]);

        // Personnaliser les couleurs des sections
        series->slices().at(0)->setBrush(QColor(255, 99, 132));  // PRIVE (rose)
        series->slices().at(1)->setBrush(QColor(54, 162, 235)); // PRO (bleu)
        series->slices().at(2)->setBrush(QColor(75, 192, 192)); // SPORT (vert)
        series->slices().at(3)->setBrush(QColor(153, 102, 255)); // CULTUREL (violet)
        series->slices().at(4)->setBrush(QColor(255, 159, 64)); // AUTRE SPORT (orange)

        // Ajouter le nom et les pourcentages dans les secteurs
        for (int i = 0; i < series->slices().count(); ++i) {
            QPieSlice *slice = series->slices().at(i);
            QString name = slice->label();  // Nom du secteur (ex: "PRIVE", "PRO", etc.)
            double percentage = slice->percentage() * 100;  // Calcul du pourcentage
            slice->setLabel(QString("%1: %2%").arg(name).arg(percentage, 0, 'f', 1));  // Nom + Pourcentage
            slice->setLabelVisible(true);  // Rendre les labels visibles
        }

        // Créez le graphique et ajoutez la série
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Répartition des événements");

        // Cachez la légende
        chart->legend()->hide();

        // Créez le QChartView et définissez les rendus
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        // Créer un layout pour afficher le graphique
        QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
        layout->addWidget(chartView);

        // Mettre à jour l'affichage
        chartView->setVisible(true);





    on_refreshButton_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QDate date = ui->dateLineEdit->date();
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->currentText();
    int budget = ui->durationLineEdit->text().toInt();
    float price = ui->priceLineEdit->text().toFloat();

    if (id <= 0) {
        QMessageBox::warning(this, "Donnez un entier" ,"positif pour l'id");
        return;
    }
    if (name.isEmpty() || !date.isValid() || type.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "All fields must be filled.");
        return;
    }
    if (date <= QDate::currentDate()) {
        QMessageBox::warning(this, "Invalid Date", "The date must be in the future.");
        return;
    }
    Event event;
    event.setId(id);
    event.setName(name);
    event.setDate(date);
    event.setCapacity(capacity);
    event.setType(type);
    event.setBudget(budget);
    event.setPrice(price);
    if (event.checkRentabilite(price, capacity, budget)) {
        event.setRentabilite("gagnant");
    }
    else
        event.setRentabilite("perdant");



    if (event.addEvent()) {
        QMessageBox::information(this, "Success", "Event added successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add the event. Check logs for details.");
    }

    // Rafraîchir la table après l'ajout
    on_refreshButton_clicked();
}

void MainWindow::on_refreshButton_clicked() {

    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);

    // Créer un objet Event et récupérer les données
    Event event;
    QSqlQueryModel* model = event.displayEvents();

    if (!model) {
        qDebug() << "Erreur : le modèle retourné par displayEvents() est null.";
        return;
    }

    // Récupérer les données du modèle et les insérer dans le QTableWidget
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->eventTableWidget->setRowCount(rowCount);
    ui->eventTableWidget->setColumnCount(columnCount);

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QVariant cellData = model->data(model->index(row, col)); // Obtenir la donnée directement

            QString displayData;
            if (col == 2) {
                if (cellData.canConvert<QDate>()) {
                    QDate date = cellData.toDate();
                    displayData = date.toString("dd/MM/yyyy"); // Formater uniquement la date
                } else if (cellData.canConvert<QDateTime>()) {
                    QDateTime dateTime = cellData.toDateTime();
                    displayData = dateTime.date().toString("dd/MM/yyyy"); // Extraire la date de QDateTime
                }
            } else {
                displayData = cellData.toString(); // Pour les autres colonnes
            }

            ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
        }
    }

    // Ajuster la taille des colonnes pour remplir l'espace
    ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "Table des événements rafraîchie avec succès.";
}


void MainWindow::on_modifyButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QDate date = ui->dateLineEdit->date();  // Utilisation de date() pour obtenir un QDate
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->currentText();
    int budget = ui->durationLineEdit->text().toInt();
    float price = ui->priceLineEdit->text().toFloat();

    if (id <= 0) {
        QMessageBox::warning(this, "Donnez un entier", "positif pour l'id");
        return;
    }
    if (name.isEmpty() || !date.isValid() || type.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "All fields must be filled.");
        return;
    }
    if (date <= QDate::currentDate()) {
        QMessageBox::warning(this, "Invalid Date", "The date must be in the future.");
        return;
    }
    Event event;
    event.setId(id);
    event.setName(name);
    event.setDate(date);
    event.setCapacity(capacity);
    event.setType(type);
    event.setBudget(budget);
    event.setPrice(price);

    if (event.modifyEvent(id)) {
        QMessageBox::information(this, "Success", "Event modified successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to modify the event. Check logs for details.");
    }

    // Rafraîchir la table après modification
    on_refreshButton_clicked();
}


void MainWindow::on_deleteButton_clicked() {
    QTableWidgetItem* currentItem = ui->eventTableWidget->currentItem();

    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select a cell to delete the event.");
        return;
    }

    int row = currentItem->row();
    int id = ui->eventTableWidget->item(row, 0)->text().toInt();  // ID dans la première colonne

    if (id <= 0) {
        QMessageBox::warning(this, "Invalid ID", "Please select a valid event to delete.");
        return;
    }

    // Créer un objet Event et appeler la méthode deleteEvent()
    Event event;
    bool success = event.deleteEvent(id);  // Supposons que cette méthode gère la requête SQL et renvoie true si réussi

    if (success) {
        QMessageBox::information(this, "Success", "Event deleted successfully.");
        qDebug() << "Événement supprimé avec succès.";

        // Supprimer la ligne du QTableWidget
        ui->eventTableWidget->removeRow(row);
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete the event. Check logs for details.");

    }

    // Rafraîchir la table après suppression
    on_refreshButton_clicked();
}



void MainWindow::on_Recherche_clicked()
{
    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);


    int id = ui->idRecherche->text().toInt();

    if (id <= 0) {
        QMessageBox::warning(this, "Invalid ID", "Please select a valid event to delete.");
        return;
    }

    // Créer un objet Event et appeler la méthode deleteEvent()
    Event event;
    QSqlQueryModel* model = event.Recherche(id);
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->eventTableWidget->setRowCount(rowCount);
    ui->eventTableWidget->setColumnCount(columnCount);

    // Remplir le QTableWidget avec les données du modèle
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(cellData));
        }
    }

    // Ajuster la taille des colonnes pour remplir l'espace
    ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "Table des événements rafraîchie avec succès.";



}

void MainWindow::on_eventTableWidget_cellClicked(int row, int column)
{


}

void MainWindow::on_eventTableWidget_cellDoubleClicked(int row, int column)
{
    qDebug() << row,column;
    if(row == 0)
    {
        ui->eventTableWidget->clearContents();
        ui->eventTableWidget->setRowCount(0);
        Event event;
        QSqlQueryModel* model = event.triEvents(column);

        if (!model) {
            qDebug() << "Erreur : le modèle retourné par displayEvents() est null.";
            return;
        }
        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->eventTableWidget->setRowCount(rowCount);
        ui->eventTableWidget->setColumnCount(columnCount);
        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < columnCount; ++col) {
                QVariant cellData = model->data(model->index(row, col)); // Obtenir la donnée directement

                QString displayData;
                if (col == 2) {
                    if (cellData.canConvert<QDate>()) {
                        QDate date = cellData.toDate();
                        displayData = date.toString("dd/MM/yyyy"); // Formater uniquement la date
                    } else if (cellData.canConvert<QDateTime>()) {
                        QDateTime dateTime = cellData.toDateTime();
                        displayData = dateTime.date().toString("dd/MM/yyyy"); // Extraire la date de QDateTime
                    }
                } else {
                    displayData = cellData.toString(); // Pour les autres colonnes
                }

                ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
            }
        }
        ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        qDebug() << "Table des événements rafraîchie avec succès.";

    }
}
