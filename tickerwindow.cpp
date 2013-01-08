#include "tickerwindow.h"
#include "ui_tickerwindow.h"
#include "feedmanager.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTimer>
#include <QtNetwork/QHttp>
#include <QFile>
#include <QList>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneResizeEvent>
#include <QFontMetrics>
#include <QDesktopServices>
#include <QAction>
#include <QMessageBox>
#include <QMenu>



TickerWindow::TickerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TickerWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    m_scrollingDistance = 1.;
    m_stepSize=1;
    m_horizontalScrolling=true;
    m_position=1;
    connect(FeedManager::getInstance(), SIGNAL( feedsLoaded() ), this, SLOT( feedsUpdated() ));
    initScrollTimer();
}

TickerWindow::~TickerWindow()
{
    delete ui;
}

void TickerWindow::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = true;
        mLastMousePosition = event->globalPos();
    }
}

void TickerWindow::mouseMoveEvent(QMouseEvent* event)
{
    if( event->buttons().testFlag(Qt::LeftButton) && mMoving)
    {
        this->move(this->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
}

void TickerWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = false;
    }
}

void TickerWindow::initScrollTimer()
{
    if (m_pTimer == 0)
    {
        m_pTimer = new QTimer(this);
        connect(m_pTimer, SIGNAL(timeout()), SLOT(animate()));
        m_pTimer->start(0);
    }
    int msec = 16;//(1. / Settings::scrollingSpeed()) * 1000;
    m_pTimer->setInterval(msec);
}

void TickerWindow::animate()
{
    if (m_items.empty() || !m_feedsLoaded)
    {
        return;
    }
    if (!m_horizontalScrolling)
    {
        if (!m_mousePressed)
        {
            if (m_decay != 0)
            {
                m_position += m_stepSize;
                m_stepSize /= m_decay;

                if (m_stepSize < 1.0 && m_stepSize > -1.0)
                {
                    if (m_stepSize < 0)
                    {
                        m_stepSize = -(float) m_scrollingDistance;
                    }
                    else
                    {
                        m_stepSize = (float) m_scrollingDistance;
                    }
                    m_decay = 0;
                }
            }
            else
            {
                m_position += m_stepSize;
            }

        }
        //		printf("m_dist:%f\n", m_stepSize);

        if (m_height == -1)
        {
            return;
        }
        if (abs(m_position) > (2 * m_height + m_itemVSpacing))
        {
            m_items.push_back(*m_items.begin());
            m_items.pop_front();
            m_position = 0;
        }
        else if (m_position > 0)
        {
            m_items.push_front(*(m_items.end() - 1));
            m_items.pop_back();
            m_position -= 2 * m_height + m_itemVSpacing;
        }
    }
    else
    {
        if (!m_mousePressed)
        {
            if (m_decay != 0)
            {
                m_position += m_stepSize;
                m_stepSize /= m_decay;
                if (m_stepSize < 1.0 && m_stepSize > -1.0)
                {
                    if (m_stepSize < 0)
                    {
                        m_stepSize = -m_scrollingDistance;
                    }
                    else
                    {
                        m_stepSize = m_scrollingDistance;
                    }
                    m_decay = 0;
                }
            }
            else
            {
                m_position += m_stepSize;
            }

        }
        //		printf("m_dist:%f\n", m_stepSize);

        int w = m_items.begin()->getWidth();
        if (w == -1)
        {
            return;
        }
        if (abs(m_position) > (w + m_padding + m_iconWidth + 10))
        {
            m_items.push_back(*m_items.begin());
            m_items.pop_front();
            m_position = 0;
        }
        else if (m_position > 0)
        {
            m_items.push_front(*(m_items.end() - 1));
            m_items.pop_back();
            m_position -= m_items.begin()->getWidth() + (m_padding + m_iconWidth + 10);
        }
    }

    update();
}

void TickerWindow::feedsUpdated()
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
            Item it(item->title(),item->link(),item->date());
            m_items.push_back(it);
        }

//        if (!favIcon.isEmpty())
//        {
//            QPixmap pm = SmallIcon(favIcon);
//            m_iconMap[iconIdx] = pm;
//            iIdx = iconIdx;
//            ++iconIdx;
//        }
//        else
//        {
//            //download the icon
//            KTemporaryFile* tmpFile = new KTemporaryFile();
//            if (tmpFile->open())
//            {
//                QUrl l(feed->link());
//                QString url = "http://" + l.host() + "/favicon.ico";
//                KIO::Job* getJob = KIO::file_copy(url, KUrl(tmpFile->fileName()), -1, KIO::Overwrite
//                        | KIO::HideProgressInfo);
//                getJob->ui()->setWindow(0);
//                if (KIO::NetAccess::synchronousRun(getJob, 0))
//                {
//                    QPixmap p(tmpFile->fileName());
//                    if (p.width() > 0)
//                    {
//                        iIdx = iconIdx;
//                        m_iconMap[iIdx] = p;
//                        ++iconIdx;
//                    }
//                }
//                else
//                {
//                    iIdx = 0;
//                }
//            }
//            tmpFile->close();
//            delete tmpFile;
//        }
        int itemCnt = 0;
        /**
         * Get max item count for this feed
         */
        int maxItems = 10;//default
        int pos = -1;
        /**
         * find the correct feed position, since they are not the same order in NewsFeedManager
         */
//        for (int i = 0; i < Settings::feedUrls().count(); ++i)
//        {
//            if(Settings::feedUrls().at(i) == currentFeedUrl)
//            {
//                pos = i;
//                break;
//            }
//        }

//        if(Settings::feedMaxItems().count() > feedPos)
//        {
//            maxItems = Settings::feedMaxItems().at(pos);
//        }
    }
    m_iconWidth = 0;
    m_height = 0;
    m_feedsLoaded = true;
    m_doInitWidth = true;
    update();

}

void TickerWindow::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);

    p.save();
    p.setFont(m_font);
    p.setClipRect(contentsRect());

    if (!m_items.empty() && m_feedsLoaded)
    {
        /*
         * Init height of the font
         */
        if (m_height == 0)
        {
            m_height = contentsRect().top() + p.fontMetrics().ascent() - p.fontMetrics().descent() + 2;
            m_iconWidth = m_height / 2;
            if (m_iconWidth < 16)
                m_iconWidth = 16;
        }

        if (m_doInitWidth)
        {
            m_padding = p.fontMetrics().width(" +++ ");
            //get the real width for all items
            for (std::deque<Item>::iterator itTmp = m_items.begin(); itTmp != m_items.end(); ++itTmp)
            {
                itTmp->setWidth(p.fontMetrics().width(itTmp->title()));
            }
            m_doInitWidth = false;
        }
        if (!m_horizontalScrolling)
        {
            int itemIdx = 0;
            int yPos = m_position;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                int x = contentsRect().width() / 2 - it->getWidth() / 2;
                if (x < 20 + m_iconWidth)
                    x = 20 + m_iconWidth;
                if (it->iconId() > 0 && m_iconMap.contains(it->iconId()))
                {
                    QPixmap icon = m_iconMap[it->iconId()];
                    p.drawPixmap(QRect(x - 5 - m_iconWidth, yPos - m_height / 2, m_iconWidth, m_iconWidth), icon, icon.rect());
                }

                if (itemIdx == m_hotItem)
                {
                    p.setPen(m_colHoverFont);
                }
                else
                {
                    p.setPen(m_colFont);
                }

                ++itemIdx;
                p.drawText(x, yPos, it->title());
                yPos += m_height + m_itemVSpacing;
                p.setPen(m_colFont);
                p.drawText(contentsRect().width() / 2, yPos, "+++");
                yPos += m_height + m_itemVSpacing;
                if (yPos > contentsRect().height() + m_height)
                    break;
            }

        }
        else
        {
            int w = m_position;
            int itemIdx = 0;
            int yPos = contentsRect().top() + contentsRect().height() / 2 + m_height / 2;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                if (it->iconId() > 0 && m_iconMap.contains(it->iconId()))
                {
                    QPixmap icon = m_iconMap[it->iconId()];
                    p.drawPixmap(QRect(w + 5, contentsRect().top() + contentsRect().height() / 2 - m_iconWidth / 2, m_iconWidth, m_iconWidth), icon, icon.rect());
                }
                w += m_iconWidth + 10;
                if (itemIdx == m_hotItem)
                {
                    p.setPen(m_colHoverFont);
                }
                else
                {
                    p.setPen(m_colFont);
                }

                ++itemIdx;
                p.drawText(w, yPos, it->title());
                //			printf("drawn:%d w_:%d w:%d\n", itemIdx,w, it->getWidth());
                w += it->getWidth();
                p.setPen(m_colFont);
                p.drawText(w, yPos, " +++ ");
                w += m_padding;
                if (w > contentsRect().width())
                    break;
            }

        }
    }
    else
    {
        int yPos = contentsRect().top() + contentsRect().height() / 2 + m_height / 2;
        p.setPen(m_colFont);
        p.drawText(10, yPos, "Loading Feeds...");
    }
    p.restore();
}



