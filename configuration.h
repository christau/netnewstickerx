#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QColor>
#include <QRect>
#include <QFont>
#include <QStringList>

class Configuration : public QObject
{
    Q_OBJECT
private:
    Configuration();

public:
    void loadConfiguration();
    void saveConfiguration();
    static Configuration* getInstance();

public:
    int m_refreshInterval; //minutes
    int m_tickerSpeed; //px/s
    int m_maxItems;
    int m_fontSize;

    /**
     * @brief m_windowPos - Last poosition of the ticker window. Used to restore the position at restart
     */
    QRect m_windowPos;
    bool m_fixWindowPos;
    QColor m_textColor;
    QColor m_bkgColor;
    QFont m_font;
    QStringList m_feeds;

signals:
    void configChanged(Configuration* cfg);

private:
    QString getDataLocation();
    static Configuration* INSTANCE;

};

#endif // CONFIGURATION_H
