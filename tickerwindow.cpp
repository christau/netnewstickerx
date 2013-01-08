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
#include <OpenGL/glu.h>


TickerWindow::TickerWindow(QWidget *parent) :
    QGLWidget(parent),
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

void TickerWindow::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void TickerWindow::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TickerWindow::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    glVertex2f(0,0);
    glVertex2f(100,500);
    glVertex2f(500,100);
    glEnd();

    QPainter p(this);
        p.drawText(200,10,"X");
        p.drawText(200,10,"Y");
        p.drawText(200,10,"Z");
        p.end();
}

/*
void TickerWindow::paintEvent(QPaintEvent *pe)
{
    QRect region = pe->rect();
    QPainter painter(this);
    QPen pen(Qt::red); //Note: set line colour like this

    //(Brush line removed; not necessary when drawing a line)
    int x = 0; //Note changed
    int y = height() / 2; //Note changed
    pen.setWidth(10);
    painter.setPen(pen);
    painter.drawLine(x,y,x+100,y);

}

*/


