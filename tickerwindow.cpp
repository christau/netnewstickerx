#include "tickerwindow.h"
#include "ui_tickerwindow.h"

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
    QMainWindow(parent),
    ui(new Ui::TickerWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
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
}

void TickerWindow::feedsUpdated()
{
}



