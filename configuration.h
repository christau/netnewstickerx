#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QColor>
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

    QColor m_textColor;
    QColor m_bkgColor;
    QFont m_font;
    QStringList m_feeds;

signals:
    void configChanged(Configuration* cfg);

private:
    static Configuration* INSTANCE;

};

#endif // CONFIGURATION_H
