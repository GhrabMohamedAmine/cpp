#include "event.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Event::Event() {}

Event::Event(int id, QString name, QString date, int capacity, QString type, QString duration, float price)
    : id_event(id), name(name), date_event(date), capacity(capacity), type(type), duration(duration), price(price) {}

bool Event::addEvent() {
    QSqlQuery query;
    query.prepare("INSERT INTO EVENTS (ID_EVENT, NAME, DATE_EVENT, CAPACITY, TYPE, DURATION, PRICE) "
                  "VALUES (:id, :name, :date, :capacity, :type, :duration, :price)");
    query.bindValue(":id", id_event);
    query.bindValue(":name", name);
    query.bindValue(":date", date_event);
    query.bindValue(":capacity", capacity);
    query.bindValue(":type", type);
    query.bindValue(":duration", duration);
    query.bindValue(":price", price);

    if (query.exec()) {
        qDebug() << "Événement ajouté avec succès.";
        return true;
    } else {
        qDebug() << "Erreur lors de l'ajout de l'événement:" << query.lastError().text();
        return false;
    }
}
QSqlQueryModel* Event::displayEvents() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EVENTS");
    return model;
}


int Event::getId() const { return id_event; }
void Event::setId(int id) { id_event = id; }

QString Event::getName() const { return name; }
void Event::setName(const QString &name) { this->name = name; }

QString Event::getDate() const { return date_event; }
void Event::setDate(const QString &date) { date_event = date; }

int Event::getCapacity() const { return capacity; }
void Event::setCapacity(int capacity) { this->capacity = capacity; }

QString Event::getType() const { return type; }
void Event::setType(const QString &type) { this->type = type; }

QString Event::getDuration() const { return duration; }
void Event::setDuration(const QString &duration) { this->duration = duration; }

float Event::getPrice() const { return price; }
void Event::setPrice(float price) { this->price = price; }
