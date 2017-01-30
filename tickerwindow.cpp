#include "tickerwindow.h"
#include "ui_tickerwindow.h"
#include "feedmanager.h"
#include "faviconmanager.h"
#include "configdialog.h"

#include <QDebug>
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTimer>
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
#include <QMimeData>



TickerWindow::TickerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TickerWindow)
{
    //    setStyleSheet("background:transparent;");
    //    setAttribute(Qt::WA_TranslucentBackground);
    //    setWindowFlags(Qt::FramelessWindowHint);
    setAcceptDrops(true);
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setMouseTracking(true);
    m_pTimer=NULL;
    m_scrollingDistance = 1.;
    m_stepSize=1;
    m_horizontalScrolling=true;
    m_feedsLoaded = false;
    m_mouseXOffs = 0;
    m_stepSize = -1.;
    m_decay = 0;
    m_doInitWidth = true;
    m_height = 0;
    m_iconWidth = 0;
    m_hotItem = -1;
    m_position = 0;
    m_itemVSpacing = 0;
    int fontSize = Configuration::getInstance()->m_fontSize;
    m_font = new QFont("Arial", fontSize>0?fontSize:25, QFont::Bold, false);
    m_colFont = Configuration::getInstance()->m_textColor;
    m_colBkg = Configuration::getInstance()->m_bkgColor;
    connect(FeedManager::getInstance(), SIGNAL( feedsLoaded() ), this, SLOT( feedsUpdated() ));
    initScrollTimer();
    m_border=Content;
    mMoving = false;

    m_colHoverFont = QColor::fromRgb(255, 0, 0);

    // myWidget is any QWidget-derived class
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));

    QRect pos = Configuration::getInstance()->m_windowPos;
    if(pos.width()==0 || pos.height()==0)
    {
        this->move(0,0);
        this->resize(200,50);
    }
    else
    {
        this->move(pos.topLeft());
        this->resize(pos.size());
    }
}

TickerWindow::~TickerWindow()
{
    delete ui;
}

void TickerWindow::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Reload Feeds");
    myMenu.addAction("Configuration");
    QAction* a = myMenu.addAction("Fix Window Position");
    a->setCheckable(true);
    a->setChecked(Configuration::getInstance()->m_fixWindowPos);
    myMenu.addSeparator();
    myMenu.addAction("Exit");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if(selectedItem->text()=="Configuration")
        {
            ConfigDialog* dlg = new ConfigDialog();
            dlg->show();
        }
        else if(selectedItem->text()=="Exit")
        {
            Configuration::getInstance()->m_windowPos = this->geometry();
            Configuration::getInstance()->saveConfiguration();
            exit(0);
        }
        else if(selectedItem->text()=="Fix Window Position")
        {
            bool fwp = selectedItem->isChecked();
            selectedItem->setChecked(!fwp);
            printf("fwp:%i\n", fwp);
            Configuration::getInstance()->m_fixWindowPos = fwp;
            Configuration::getInstance()->saveConfiguration();
        }
    }
    else
    {
        // nothing was chosen
    }
}



void TickerWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mMoving = true;
        if (m_horizontalScrolling)
        {
            m_mouseXPos = event->pos().x();
        }
        else
        {
            m_mouseXPos = event->pos().y();
        }

        m_mouseXOffs = m_mouseXPos;
        m_mouseDelta = 0;
        mLastMousePosition = event->globalPos();
    }
}

void TickerWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(!mMoving)
    {
        m_border = getBorder(event->globalPos());
        Qt::CursorShape s = Qt::ArrowCursor;
        if(m_border!=Content)
        {
            switch(m_border)
            {
            case Top:
                s=Qt::SizeVerCursor;
                break;
            case Right:
                s=Qt::SizeHorCursor;
                break;
            case Bottom:
                s=Qt::SizeVerCursor;
                break;
            case Left:
                s=Qt::SizeHorCursor;
                break;

            }
        }
        setCursor(s);
    }

    if( event->buttons().testFlag(Qt::LeftButton) && mMoving && m_border==Content && !Configuration::getInstance()->m_fixWindowPos)
    {
        this->move(this->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
    else if(mMoving && m_border!=Content)
    {
        QRect r = this->geometry();
        switch(m_border)
        {
        case Top:
            r.setTop(event->globalPos().y());
            break;
        case Right:
            r.setRight(event->globalPos().x());
            break;
        case Bottom:
            r.setBottom(event->globalPos().y());
            break;
        case Left:
            r.setLeft(event->globalPos().x());
            break;
        }

        this->setGeometry(r);
    }
    else
    {
        if (m_horizontalScrolling)
        {
            int w = m_position + m_iconWidth + 10;
            int x = event->pos().x();
            int y = event->pos().y();
            qDebug()<<"X:"<<x<<" Y:"<<y;
            qDebug()<<"Geom:"<<geometry();
            m_hotItem = -1;
            int item = 0;
            int yMin = geometry().height() / 2 - m_height / 2;
            int yMax = geometry().height() / 2 + m_height / 2;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                int tmp = it->getWidth();

                if (x > w && x < (w + tmp) && (y > yMin) && (y < yMax))
                {
                    m_hotItem = item;
                    setCursor(Qt::PointingHandCursor);
                    break;
                }
                w += tmp + m_padding + m_iconWidth;
                if (w > this->size().width())
                    break;
                ++item;
            }
            if (m_hotItem == -1 && m_border ==-1)
                setCursor(Qt::ArrowCursor);
        }
        else
        {
            int y = event->pos().y();
            int item = 0;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                int h = item * 2 * m_height + item * m_itemVSpacing /*+ Y_OFFSET*/+ m_position;
                if ((y > h - m_height) && (y < h))
                {
                    m_hotItem = item;
                    setCursor(Qt::PointingHandCursor);
                    break;
                }
                if (h > this->size().height())
                {
                    setCursor(Qt::ArrowCursor);
                    m_hotItem = -1;
                    break;
                }
                ++item;
            }
        }
    }
}

void TickerWindow::mouseReleaseEvent(QMouseEvent* event)
{
    mMoving = false;
    //if the user moved the mouse too much
    //do not check for opening the url
    if (abs(m_mouseDelta) < 20)
    {
        if (m_hotItem == -1)
            return;
        int item = 0;
        for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            if (m_hotItem == item)
                QDesktopServices::openUrl(it->link());
            ++item;
        }
    }
    else
    {
        if (m_mouseDelta < -20)
        {
            m_decay = 1.05;
            m_stepSize = m_mouseDelta / 10;
            if (m_stepSize > -(float) m_scrollingDistance)
                m_stepSize = -(float) m_scrollingDistance;
        }
        else if (m_mouseDelta > 20)
        {
            m_decay = 1.03;
            m_stepSize = m_mouseDelta / 10;
            if (m_stepSize < (float) m_scrollingDistance)
                m_stepSize = (float) m_scrollingDistance;
        }
    }

    m_mouseXOffs = 0;
}

void TickerWindow::initScrollTimer()
{
    if (m_pTimer == 0)
    {
        m_pTimer = new QTimer(this);
        m_pTimer->setTimerType(Qt::PreciseTimer);
        connect(m_pTimer, SIGNAL(timeout()), SLOT(animate()));
        m_pTimer->start(25);
    }
    //int msec = 25;//(1. / Settings::scrollingSpeed()) * 1000;
   // m_pTimer->setInterval(msec);
}

void TickerWindow::animate()
{
    if (m_items.empty() || !m_feedsLoaded)
    {
        return;
    }
    if (!m_horizontalScrolling)
    {
        if (!mMoving)
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
        if (!mMoving)
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
   //     const QString favIcon = feed->link().toString();//KMimeType::favIconForUrl(feed->link());
        QUrl tmp = QUrl(feed->link());
        tmp.setPath("/favicon.ico");
        QPixmap pm = FavIconManager::getInstance()->getFavIcon(tmp.toString());
        iIdx = -1;
        if(!pm.isNull())
        {
            m_iconMap[iconIdx] = pm;
            iIdx = iconIdx;
            ++iconIdx;
        }
        foreach (Item* item, feed->items())
        {
            Item it(item->title(),item->link(),item->date());
            if(iIdx>-1)
            {
                it.setIconId(iIdx);
            }
            m_items.push_back(it);
        }

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

void TickerWindow::wheelEvent(QWheelEvent *event)
{
    animate();
    m_position += event->delta() / 3;
}


void TickerWindow::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);

    p.save();
    p.setFont(*m_font);
    p.setClipRect(contentsRect());
    //p.fillRect((contentsRect()), m_colBkg);
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

void TickerWindow::dragEnterEvent(QDragEnterEvent *e)
{
        e->acceptProposedAction();
}

void TickerWindow::dropEvent(QDropEvent * event)
{
    QList<QUrl> list = event->mimeData()->urls();
    if (list.count() > 0)
    {
        //        connect(NewsFeedManager::self(), SIGNAL( feedLoaded( const QUrl & ) ), this, SLOT( feedLoaded( const QUrl & ) ));
        //        NewsFeedManager::self()->updateFeed(list.at(0));
    }
}

void TickerWindow::leaveEvent(QEvent *)
{
    m_hotItem = -1;
    setCursor(Qt::ArrowCursor);
}

TickerWindow::Corner TickerWindow::getBorder(const QPoint& pos)
{
    QRect r = this->geometry();
    int tolerance = 5;
    if(pos.x() < (r.x()+tolerance))
    {
        return Left;//West
    }
    else if(pos.x()>r.x()+r.width()-tolerance)
    {
        return Right;//East
    }
    else if(pos.y()<r.y()+tolerance)
    {
        return Top;//North
    }
    else if(pos.y()>r.y()+r.height()-tolerance)
    {
        return Bottom;//South
    }

    return Content;
}

void TickerWindow::moveEvent(QMoveEvent *event)
{
    if (mMoving)
    {

    }
    else
    {
        if (m_horizontalScrolling)
        {
            int w = m_position + m_iconWidth + 10;
            int x = event->pos().x();
            int y = event->pos().y();
            m_hotItem = -1;
            int item = 0;
            int yMin = this->geometry().top() + this->geometry().height() / 2 - m_height / 2;
            int yMax = this->geometry().top() + this->geometry().height() / 2 + m_height / 2;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                int tmp = it->getWidth();

                if (x > w && x < (w + tmp) && (y > yMin) && (y < yMax))
                {
                    m_hotItem = item;
                    setCursor(Qt::PointingHandCursor);
                    break;
                }
                w += tmp + m_padding + m_iconWidth;
                if (w > this->size().width())
                    break;
                ++item;
            }
            if (m_hotItem == -1)
                setCursor(Qt::ArrowCursor);
        }
        else
        {
            int y = event->pos().y();
            int item = 0;
            for (std::deque<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it)
            {
                int h = item * 2 * m_height + item * m_itemVSpacing /*+ Y_OFFSET*/+ m_position;
                if ((y > h - m_height) && (y < h))
                {
                    m_hotItem = item;
                    setCursor(Qt::PointingHandCursor);
                    break;
                }
                if (h > this->size().height())
                {
                    setCursor(Qt::ArrowCursor);
                    m_hotItem = -1;
                    break;
                }
                ++item;
            }

            update();
        }
    }

}

void TickerWindow::configChanged(Configuration* cfg)
{
    return;
}
