#ifndef TICKERWINDOW_H
#define TICKERWINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QMouseEvent>
#include <QTimer>


namespace Ui {
class TickerWindow;
}

class TickerWindow : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit TickerWindow(QWidget *parent = 0);
    ~TickerWindow();

public slots:
    void feedsUpdated();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
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
