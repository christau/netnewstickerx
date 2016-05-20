#include "faviconmanager.h"
#include <QPixmap>

FavIconManager* FavIconManager::m_instance = 0;

FavIconManager* FavIconManager::getInstance()
{
    if(m_instance==0)
    {
        m_instance = new FavIconManager();
    }
    return m_instance;
}

FavIconManager::FavIconManager()
{
    m_manager = new QNetworkAccessManager( );
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(iconFinished(QNetworkReply *)));
}

FavIconManager::~FavIconManager()
{

}

void FavIconManager::cacheFavIcon(QString url)
{
    QNetworkRequest request(url);

    m_reply = m_manager->get(request);

}

void FavIconManager::iconFinished(QNetworkReply * reply)
{
    QByteArray content(reply->readAll());
    if( !reply->error( ) )
    {
        QPixmap pixmap;
        pixmap.loadFromData( content );
        if( !pixmap.isNull( ) )
        {
            m_iconMap[reply->url().toString()]=pixmap;
        }
    }

    return;
}

QPixmap& FavIconManager::getFavIcon(QString url)
{
    return m_iconMap[url];
}
