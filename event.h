#ifndef EVENT_H
#define EVENT_H


#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Event {
public:
    Event();
    Event(int id, QString name, QString date, int capacity, QString type, QString duration, float price);

    bool addEvent();
    QSqlQueryModel* displayEvents();
    bool modifyEvent(int id);
    bool deleteEvent(int id);

private:
    int id_event;
    QString name;
    QString date_event;
    int capacity;
    QString type;
    QString duration;
    float price;
};

#endif // EVENT_H
