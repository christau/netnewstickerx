#ifndef ITEM_H
#define ITEM_H

#include <QString>

class Item
{
public:
    Item(QString link, QString title,QString date) : m_link(link), m_title(title),m_date(date)
    {
    }

    int	getWidth() const
    {
        return m_width;
    }
    void setWidth(int width)
    {
        m_width = width;
    }
    int iconId()
    {
        return m_iconId;
    }
    void setIconId(int iconId)
    {
        m_iconId = iconId;
    }

    const QString& title(){return m_title;}
    const QString& link(){return m_link;}
    const QString& date(){return m_date;}

private:
    QString m_link;
    QString m_title;
    QString m_date;
    int	m_width;
    int m_iconId;

};

#endif // ITEM_H
