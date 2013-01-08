#ifndef FEED_H
#define FEED_H

#include<QList>

#include "item.h"

class Feed
{
public:
    Feed();
    const QList<Item*> items()
    {
        return m_items;
    }

    const QString& title()
    {
        return m_title;
    }

    const QString& link()
    {
        return m_link;
    }
    void addItem(Item* item);

private:
    QString m_title;
    QString m_link;
    QList<Item*> m_items;

};

#endif // FEED_H
