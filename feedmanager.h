#ifndef FEEDMANAGER_H
#define FEEDMANAGER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QXmlStreamReader>

#include "feed.h"


class FeedManager : public QObject
{
    Q_OBJECT
public:
    static FeedManager* getInstance();

private:
    explicit FeedManager(QObject *parent = 0);
    /**
     * @brief triggers to read the passed rss fedds. If finished, the signal feedsLoaded() is emitted
     * @param urls a list of RSS urls
     */
public:
    void setSubscriptions( const QStringList &urls );
    const QMap<QUrl, Feed*> & availableFeeds() const;
    
signals:
    /**
     * This method gets emitted if all feeds are loaded
     */
    void feedsLoaded();
    
private slots:
    void requestFinished( int id, bool error );

private:
    QMap<QUrl, Feed*> m_availableFeeds;
    QMap<int, QUrl> m_scanIDs;
    QHttp m_http;
    bool parseXml(Feed* feed, QByteArray content);
    static FeedManager* m_instance;
    
};

#endif // FEEDMANAGER_H
