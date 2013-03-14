#include <QApplication>
#include <QNetworkProxy>
#include <QtGui>
#include "tickerwindow.h"
#include "tickerview.h"

#include "configuration.h"

#include "faviconmanager.h"
#include "feedmanager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TickerWindow w;
    w.show();
    Configuration* cfg = Configuration::getInstance();
    cfg->loadConfiguration();
/*    QNetworkProxy proxy;
     proxy.setType(QNetworkProxy::HttpCachingProxy);
     proxy.setHostName("10.10.10.80");
     proxy.setPort(3128);
     QNetworkProxy::setApplicationProxy(proxy);*/
//     QNetworkProxyFactory::setUseSystemConfiguration(true);

    FavIconManager* m = FavIconManager::getInstance();
    m->cacheFavIcon("http://www.spiegel.de/favicon.ico");

    FeedManager* f=FeedManager::getInstance();
//    QStringList s;
////    s<<"http://www.spiegel.de/schlagzeilen/tops/index.rss";
//    s<<"http://www.pl-forum.de/backend/pro-linux.rdf";
//    s<<"http://slashdot.org/slashdot.rdf";
//    s<<"http://www.spiegel.de/schlagzeilen/rss/0\\,5291\\,\\,00.xml";
//    s<<"http://www.tagesschau.de/newsticker.rdf";
//    s<<"http://www.faz.net/s/Rub/Tpl~Epartner~SRss_.xml";
//    s<<"http://www.heise.de/newsticker/heise.rdf";
//    //s<<"http://www.heise.de/newsticker/heise-atom.xml";
//    //s<<"http://10.10.160.72:8090/rssLatest";
//    //s<<"http://10.10.160.72:8090/rssFailed";
    f->setSubscriptions(cfg->m_feeds);

    QObject::connect(cfg, SIGNAL(configChanged(Configuration*)), &w, SLOT(configChanged(Configuration*)));
    
    return a.exec();
}
