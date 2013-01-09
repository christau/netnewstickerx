#include "tickerview.h"
#include "feedmanager.h"

TickerView::TickerView(QWidget* parent) :
    QGraphicsView(new QGraphicsScene(), parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    m_scrollingDistance = 1.;
    m_stepSize=1;
    m_horizontalScrolling=true;
    m_feedsLoaded = false;
    m_mouseXOffs = 0;
    m_stepSize = -1.;
    m_decay = 0;
    m_doInitWidth = true;
    m_mousePressed = false;
    m_height = 0;
    m_iconWidth = 0;
    m_hotItem = -1;
    m_position = 0;
    m_itemVSpacing = 0;
    connect(FeedManager::getInstance(), SIGNAL( feedsLoaded() ), this, SLOT( feedsUpdated() ));
}

void TickerView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = true;
        mLastMousePosition = event->globalPos();
    }
}

void TickerView::mouseMoveEvent(QMouseEvent* event)
{
    if( event->buttons().testFlag(Qt::LeftButton) && mMoving)
    {
        this->move(this->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
}

void TickerView::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = false;
    }
}

void TickerView::feedsUpdated()
{
    m_items.clear();
    m_iconMap.clear();
    m_doInitWidth = true;
    m_position = 1;
    int iconIdx = 1;
    QList<QUrl> avFeeds = FeedManager::getInstance()->availableFeeds().keys();

    QList<Feed*> availableFeeds = FeedManager::getInstance()->availableFeeds().values();
    for (int feedPos = 0; feedPos < availableFeeds.count(); ++feedPos)
    {
        QString currentFeedUrl = avFeeds.at(feedPos).toString();
        Feed* feed = availableFeeds.at(feedPos);
        int iIdx = 0;
        const QString favIcon = "";//KMimeType::favIconForUrl(feed->link());

        foreach (Item* item, feed->items())
        {
            QGraphicsTextItem* it = new QGraphicsTextItem(item->title());
            m_items.push_back(it);
        }
     }
    m_iconWidth = 0;
    m_height = 0;
    m_feedsLoaded = true;
    m_doInitWidth = true;
}

