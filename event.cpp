#include "event.h"

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
    return query.exec();
}

QSqlQueryModel* Event::displayEvents() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EVENTS");
    return model;
}

bool Event::modifyEvent(int id) {
    QSqlQuery query;
    query.prepare("UPDATE EVENTS SET NAME = :name, DATE_EVENT = :date, CAPACITY = :capacity, TYPE = :type, "
                  "DURATION = :duration, PRICE = :price WHERE ID_EVENT = :id");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":date", date_event);
    query.bindValue(":capacity", capacity);
    query.bindValue(":type", type);
    query.bindValue(":duration", duration);
    query.bindValue(":price", price);
    return query.exec();
}

bool Event::deleteEvent(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM EVENTS WHERE ID_EVENT = :id");
    query.bindValue(":id", id);
    return query.exec();
}
