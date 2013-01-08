#ifndef TICKERWINDOW_H
#define TICKERWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QTimer>


namespace Ui {
class TickerWindow;
}

class TickerWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TickerWindow(QWidget *parent = 0);
    ~TickerWindow();

public slots:
    void feedsUpdated();

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private:
    void initScrollTimer();
    QPoint mLastMousePosition;
    bool mMoving;
    Ui::TickerWindow *ui;
    QTimer* m_pTimer;

private slots:
    void animate();
};

#endif // TICKERWINDOW_H
