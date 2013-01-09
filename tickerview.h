#ifndef TICKERVIEW_H
#define TICKERVIEW_H

#include <QtGui>
#include <deque>

class TickerView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TickerView(QWidget* parent=NULL);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    
public slots:
    /**
     * This should get called, if the feeds are finished loading in the FeedManager
     */
    void feedsUpdated();

private:
    QPoint mLastMousePosition;
    bool mMoving;
    QMap<int, QPixmap> m_iconMap;
    QColor m_colFont;
    QColor m_colHoverFont;
    std::deque<QGraphicsTextItem*> m_items;
    QFont m_font;
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
    bool m_mousePressed;
    int m_mouseXPos;
    bool m_doInitWidth;
    bool m_feedsLoaded;
    float m_scrollingDistance;
    /*
     * Distance between the news items in vertical scrolling mode
     */
    int m_itemVSpacing;
};

#endif // TICKERVIEW_H
