#ifndef TICKERWINDOW_H
#define TICKERWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include <deque>
#include <item.h>

#include "configuration.h"


namespace Ui {
class TickerWindow;
}

class TickerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit TickerWindow(QWidget *parent = 0);
    ~TickerWindow();

public slots:
    /**
     * This should get called, if the feeds are finished loading in the FeedManager
     */
    void feedsUpdated();
    void ShowContextMenu(const QPoint& pos);
    void configChanged(Configuration* cfg);

protected:
    void paintEvent(QPaintEvent *pe);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    virtual void dropEvent ( QDropEvent * event );
    virtual void wheelEvent(QWheelEvent *event);
    virtual void moveEvent(QMoveEvent *event);
    virtual void leaveEvent(QEvent *);

private:
    void initScrollTimer();
    QPoint mLastMousePosition;
    bool mMoving;
    Ui::TickerWindow *ui;
    QTimer* m_pTimer;
    QTimer* m_pMoveElapsedTimer;

private slots:
    void animate();

private:
    QMap<int, QPixmap> m_iconMap;
    QColor m_colFont;
    QColor m_colHoverFont;
    std::deque<Item> m_items;
    QFont* m_font;
    bool m_horizontalScrolling;
    int m_mouseXOffs;
    int m_mouseDelta;
    float m_decay;
    float m_stepSize;
    int m_position;
    int m_padding;
    int m_height;
    int m_iconWidth;
    int m_hotItem;
    int m_mouseXPos;
    bool m_doInitWidth;
    bool m_feedsLoaded;
    float m_scrollingDistance;
    /*
     * Distance between the news items in vertical scrolling mode
     */
    int m_itemVSpacing;

    enum Corner
    {
        Content,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
        TopLeft
    };
    Corner m_border;

private:
    Corner getBorder(const QPoint& pos);


};

#endif // TICKERWINDOW_H
