#ifndef FEED_H
#define FEED_H

#include<QList>
#include<QUrl>

#include "item.h"

class Feed
{
public:
    Feed(QUrl url)
    {
        m_link=url;
    }

    const QList<Item*> items()
    {
        return m_items;
    }

    const QString& title()
    {
        return m_title;
    }

    const QUrl& link()
    {
        return m_link;
    }
    void addItem(Item* item);

private:
    QString m_title;
    QUrl m_link;
    QList<Item*> m_items;

};

#endif // FEED_H
