#include "feedmanager.h"

#include <QDebug>

FeedManager::FeedManager(QObject *parent) :
    QObject(parent)
{
    m_http.setProxy("10.10.10.80", 3128);
    //connect(&m_http, SIGNAL(readyRead(const QHttpResponseHeader &)), this, SLOT(readData(const QHttpResponseHeader &)));
    //connect(&m_http, SIGNAL(requestStarted(int)), this, SLOT(requestStarted(int)));
   connect(&m_http, SIGNAL(requestFinished (int, bool)), this, SLOT(requestFinished( int, bool)));
   //connect(&m_http, SIGNAL(responseHeaderReceived (const QHttpResponseHeader&)), this, SLOT(handleHeaderResponse (const QHttpResponseHeader&)));

}

/*
void FeedManager::responseHeaderReceived(const QHttpResponseHeader &resp)
{
    qDebug() << "Size : " << resp.contentLength();
    qDebug() << "Type : " << resp.contentType();
    qDebug() << "Status Code : " << resp.statusCode();
}
*/

void FeedManager::setSubscriptions( const QStringList &urls )
{
    foreach(QString s, urls)
    {
        QUrl url(s);
        //qDebug()<<"Scanning:"<<url.toString();
        //m_http.setHost(url.host());
        m_http.setHost(url.host(), url.port() != -1 ? url.port() : 80);
        int id = m_http.get(url.path());
        m_scanIDs[id] = url;
    }
}

const QMap<QUrl, Feed*> &FeedManager::availableFeeds() const
{
    return m_availableFeeds;
}

/*
void FeedManager::readData(const QHttpResponseHeader &resp)
{
    qDebug()<<"Ready id:"<<m_http.currentId();
    qDebug()<<"Ready req:"<<m_http.currentRequest().path();
    if (resp.statusCode() != 200)
        m_http.abort();
    else
    {
        m_xml.addData(m_http.readAll());
        parseXml();
    }
}
*/

void FeedManager::requestFinished( int id, bool error)
{
    if(error)
    {
        qDebug() <<"Error";
    }
    else if(m_scanIDs.contains(id))
    {
        QUrl url = m_scanIDs[id];
        Feed* feed;
        if(m_availableFeeds.contains(url))
        {
            feed = m_availableFeeds[url];
        }
        else
        {
            feed = new Feed();
            m_availableFeeds[url]=feed;
        }
        parseXml(feed, m_http.readAll());
        //remove id, since we parsed this feed
        m_scanIDs.remove(id);

    }
    return;
}

void FeedManager::parseXml(Feed* feed, QByteArray content)
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
                Item* item = new Item(linkString,titleString,dateString);
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
    qDebug()<<"Got "<<feed->items().count() <<" Items for this feed";
    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
        //m_http.abort();
    }
}

