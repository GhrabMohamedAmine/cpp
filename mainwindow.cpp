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
#include <QSerialPortInfo>
#include <QSerialPort>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
        ui->setupUi(this);

        arduino = new QSerialPort(this);
                connectToArduino();  // Connexion à l'Arduino
                connect(ui->gazDetect, &QPushButton::clicked, this, &MainWindow::on_gazDetect_clicked);  // Connexion du signal du bouton

                // Logique pour la détection des ports série
                qDebug() << "Nombre de ports:" << QSerialPortInfo::availablePorts().length();
                foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
                    qDebug() << "Nom du port:" << portInfo.portName();
                    qDebug() << "Description:" << portInfo.description();
                    qDebug() << "Fabricant:" << portInfo.manufacturer();
                    qDebug() << "Emplacement système:" << portInfo.systemLocation();
                }

        connect(ui->eventTableWidget, &QTableWidget::cellClicked, this, &MainWindow::on_eventTableWidget_cellClicked);
        Connection connection;
        if (!connection.createconnect()) {
            QMessageBox::critical(this, "Erreur de connexion", "La connexion à la base de données a échoué.");
            return;
        }
        QPieSeries *series = new QPieSeries();
        Event event;
        QVector<int> percents = Event::getPercent();
        if (percents[0]!=0)
        {
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
            for (int i = 0; i < series->slices().count(); ++i) {
                QPieSlice *slice = series->slices().at(i);
                QString name = slice->label();  // Nom du secteur (ex: "PRIVE", "PRO", etc.)
                double percentage = slice->percentage() * 100;  // Calcul du pourcentage
                slice->setLabel(QString("%1: %2%").arg(name).arg(percentage, 0, 'f', 1));  // Nom + Pourcentage
                slice->setLabelVisible(true);  // Rendre les labels visibles
            }

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("Répartition des événements");


            chart->legend()->hide();
            QChartView *chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
            layout->addWidget(chartView);
            chartView->setVisible(true);
        }






    on_refreshButton_clicked();
}

MainWindow::~MainWindow() {
    if (arduino->isOpen()) {
          arduino->close();
      }
    connect(ui->gazDetect, &QPushButton::pressed, this, &MainWindow::on_gazDetect_pressed);

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
    QString salle = ui->salleLineEdit->currentText();
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
    event.setSalle(salle);
    if (event.checkRentabilite(price, capacity, budget)) {
        event.setRentabilite("gagnant");
    }
    else
        event.setRentabilite("perdant");


    if(event.verifSalle(date,salle))
    {
        if (event.addEvent()) {
            QMessageBox::information(this, "Success", "Event added successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed.");
        }
    }
    else
        QMessageBox::critical(this, "Error", "Cette salle est deja reservee pour cette date, Veuillez choisir une autre s'il vous plait.");
    on_refreshButton_clicked();


}

void MainWindow::on_refreshButton_clicked() {

    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);
    Event event;
    QSqlQueryModel* model = event.displayEvents();

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
                    displayData = date.toString("dd/MM/yyyy");
                } else if (cellData.canConvert<QDateTime>()) {
                    QDateTime dateTime = cellData.toDateTime();
                    displayData = dateTime.date().toString("dd/MM/yyyy");
                }
            } else {
                displayData = cellData.toString();
            }

            ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
        }
    }


    ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    

    qDebug() << "Table des événements rafraîchie avec succès.";
}


void MainWindow::on_modifyButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QDate date = ui->dateLineEdit->date();
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
    if(row == 1)
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
                QVariant cellData = model->data(model->index(row, col));

                QString displayData;
                if (col == 2) {
                    if (cellData.canConvert<QDate>()) {
                        QDate date = cellData.toDate();
                        displayData = date.toString("dd/MM/yyyy");
                    } else if (cellData.canConvert<QDateTime>()) {
                        QDateTime dateTime = cellData.toDateTime();
                        displayData = dateTime.date().toString("dd/MM/yyyy");
                    }
                } else {
                    displayData = cellData.toString();
                }

                ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
            }
        }
        ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        qDebug() << "Table des événements rafraîchie avec succès.";

    }
}


QString serialBuffer;  // Tampon pour assembler les données série

void MainWindow::readArduinoData() {
    QByteArray data = arduino->readAll();  // Lire les données reçues de l'Arduino
    serialBuffer += QString(data);  // Ajouter les données au tampon

    // Vérification et traitement des données reçues
    while (serialBuffer.contains("\n")) {
        int index = serialBuffer.indexOf("\n");  // Trouver la fin d'une ligne
        QString completeLine = serialBuffer.left(index).trimmed();  // Extraire la ligne complète
        serialBuffer.remove(0, index + 1);  // Retirer la ligne traitée du tampon

        // Afficher la valeur reçue
        qDebug() << "Données complètes Arduino reçues :" << completeLine;
        // Si les données reçues sont un nombre (ex: valeur du gaz)
        bool ok;
        int gazValue = completeLine.toInt(&ok);  // Convertir la ligne en entier
        if (ok) {
             gazValueString = QString::number(gazValue);  // Stocker la valeur du gaz sous forme de chaîne
        }
    }
}
void MainWindow::connectToArduino() {
    // Détection des ports série disponibles
    foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
        if (portInfo.description().contains("Arduino")) {
            arduinoPortName = portInfo.portName();
            break;
        }
    }

    if (arduinoPortName.isEmpty()) {
        QMessageBox::warning(this, "Arduino non trouvé", "Aucun appareil Arduino détecté.");
        return;
    }

    arduino->setPortName(arduinoPortName);
    arduino->setBaudRate(QSerialPort::Baud9600);
    arduino->setDataBits(QSerialPort::Data8);
    arduino->setParity(QSerialPort::NoParity);
    arduino->setStopBits(QSerialPort::OneStop);
    arduino->setFlowControl(QSerialPort::NoFlowControl);

    if (arduino->open(QIODevice::ReadOnly)) {
        connect(arduino, &QSerialPort::readyRead, this, &MainWindow::readArduinoData);  // Connexion pour lire les données
        qDebug() << "Arduino connecté sur le port :" << arduinoPortName;
    } else {
        QMessageBox::critical(this, "Erreur de connexion", "Impossible d'ouvrir le port série.");
    }
}

    void MainWindow::on_gazDetect_pressed() {


    }

    void MainWindow::on_gazDetect_clicked() {
        if (! gazValueString.isEmpty()) {
            Event event;
            int id = ui->idRecherche->text().toInt();
            if(bool success = event.verifGaz(gazValueString.toInt(),id))
                QMessageBox::warning(this, "ça marche", "bien.");


            else
                QMessageBox::warning(this, "Erreur", "Aucune donnée de gaz reçue.");

        } else {
            QMessageBox::warning(this, "Erreur", "Aucune donnée de gaz reçue.");
        }
        on_refreshButton_clicked();

    }
