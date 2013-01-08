#ifndef FEEDMANAGER_H
#define FEEDMANAGER_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QXmlStreamReader>
#include <QLinkedList>

#include "feed.h"


class FeedManager : public QObject
{
    Q_OBJECT
public:
    explicit FeedManager(QObject *parent = 0);
    /**
     * @brief triggers to read the passed rss fedds. If finished, the signal feedsLoaded() is emitted
     * @param urls a list of RSS urls
     */
    void setSubscriptions( const QStringList &urls );
    const QMap<QUrl, Feed*> & availableFeeds() const;
    
signals:
    void feedsLoaded();
    
private slots:
 //   void readData(const QHttpResponseHeader &);
    void requestFinished( int id, bool error );
//    void responseHeaderReceived(const QHttpResponseHeader&);
    //void handleHeaderResponse ( const QHttpResponseHeader&);

private:
    QMap<QUrl, Feed*> m_availableFeeds;
    QMap<int, QUrl> m_scanIDs;
    QHttp m_http;
    void parseXml(Feed* feed, QByteArray content);
    
};

#endif // FEEDMANAGER_H
