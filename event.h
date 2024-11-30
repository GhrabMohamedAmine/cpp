#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

class Event {
public:
    Event();
    Event(int id, QString name, QString date, int capacity, QString type, QString duration, float price);

    bool addEvent();
    QSqlQueryModel* displayEvents();
    bool modifyEvent(int id);
    bool deleteEvent(int id);

    // Getters et Setters
    int getId() const;
    void setId(int id);

    QString getName() const;
    void setName(const QString &name);

    QString getDate() const;
    void setDate(const QString &date);

    int getCapacity() const;
    void setCapacity(int capacity);

    QString getType() const;
    void setType(const QString &type);

    QString getDuration() const;
    void setDuration(const QString &duration);

    float getPrice() const;
    void setPrice(float price);

private:
    int id_event;
    QString name;
    QString date_event;
    int capacity;
    QString type;
    QString duration;
    int price;
};

#endif // EVENT_H
