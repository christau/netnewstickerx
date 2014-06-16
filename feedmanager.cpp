#include "feedmanager.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

FeedManager* FeedManager::m_instance = 0;

FeedManager* FeedManager::getInstance()
{
    if(m_instance==0)
    {
        m_instance = new FeedManager();
    }
    return m_instance;
}

FeedManager::FeedManager(QObject *parent) :
    QObject(parent)
{
    FeedManager::m_instance=0;
   // m_http.setProxy("10.10.10.80", 3128);
   connect(&m_http, SIGNAL(finished (QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));

}

void FeedManager::setSubscriptions( const QStringList &urls )
{
    foreach(QString s, urls)
    {
        QUrl url(s);
        url.setHost(url.host());
        url.setPort( url.port() != -1 ? url.port() : 80);
        QNetworkRequest req(url);
        m_http.get(req);

        //m_scanIDs[id] = url;
/*
        Feed* feed;
        if(m_availableFeeds.contains(url))
        {
            feed = m_availableFeeds[url];
        }
        else
        {
            feed = new Feed(url);
            m_availableFeeds[url]=feed;
        }
        qDebug()<<"Ret:"<<id->readAll();
        if(!parseXml(feed, id->readAll()))
        {
            qDebug()<<"Parse error in rss:"<<url;
        }
*/
    }
     //emit feedsLoaded();

}

const QMap<QUrl, Feed*> &FeedManager::availableFeeds() const
{
    return m_availableFeeds;
}

void FeedManager::requestFinished(QNetworkReply* reply)
{
    QUrl url = reply->url();
    /*if(error)
    {
        qDebug() <<"Error";
    }*/
    /*else if(m_scanIDs.contains(id))*/
    {
        //QUrl url = m_scanIDs[id];
        Feed* feed;
        if(m_availableFeeds.contains(url))
        {
            feed = m_availableFeeds[url];
        }
        else
        {
            feed = new Feed(url);
            m_availableFeeds[url]=feed;
        }
        //qDebug()<<"URL:"<<url;
        //qDebug()<<"Ret:"<<reply->readAll();
        if(!parseXml(feed, reply->readAll()))
        {
            qDebug()<<"Parse error in rss:"<<url;
        }
        //remove id, since we parsed this feed
       // m_scanIDs.remove(id);
        if(m_scanIDs.count()==0)
        {
            emit feedsLoaded();
        }
    }
//    else
//        m_scanIDs.remove(id);

    return;
}

bool FeedManager::parseXml(Feed* feed, QByteArray content)
{
    QString currentTag;
    QString linkString;
    QString titleString;
    QString dateString;
    bool isItem = false;

    QXmlStreamReader xml;
    xml.addData(content);

    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            if (xml.name() == "item")
            {
                isItem=true;
                linkString.clear();
                titleString.clear();
                dateString.clear();
            }

            currentTag = xml.name().toString();
        }
        else if (xml.isEndElement())
        {
            if (xml.name() == "item")
            {
                Item* item = new Item(titleString,linkString,dateString);
                feed->addItem(item);
                isItem = false;
                titleString.clear();
                linkString.clear();
                dateString.clear();
            }

        } else if (xml.isCharacters() && !xml.isWhitespace() && isItem)
        {
            if (currentTag == "title")
            {
                titleString += xml.text().toString();
            }
            else if (currentTag == "link")
            {
                linkString += xml.text().toString();
            }
            else if (currentTag == "pubDate")
            {
                dateString += xml.text().toString();
            }
        }
    }
    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
        return false;
    }
    qDebug()<<"Items for "<<feed->link()<<" "<<feed->items().size();
    return true;
}

