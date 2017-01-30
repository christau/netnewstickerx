#include "configuration.h"

#include <QSettings>
#include <QDir>
#include <QDebug>
#include <qdesktopservices.h>

/*
 * Singleton
 */
Configuration* Configuration::INSTANCE = NULL;

Configuration::Configuration()
{
    INSTANCE = NULL;
}

QString Configuration::getDataLocation()
{
    QString loc = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);
    qDebug()<<"Loc:"<<loc;
    /*if(!QDir(loc + "/NetNewsTickerX").exists())
    {
        QDir(loc).mkdir("NetNewsTickerX");
    }
    return loc + "/NetNewsTickerX";
    */
    return loc;
}

void Configuration::loadConfiguration()
{

    QSettings settings(getDataLocation() + "/NetNewsTicker.cfg", QSettings::NativeFormat);
    Configuration * cfg = INSTANCE;
    cfg->m_refreshInterval = settings.value("RefreshInterval", "15").toInt();
    cfg->m_maxItems = settings.value("MaxItems", "0").toInt();
    cfg->m_tickerSpeed = settings.value("TickerSpeed", "100").toInt();
    cfg->m_bkgColor = settings.value("BkgColor").value<QColor>();
    cfg->m_textColor = settings.value("TextColor").value<QColor>();
    cfg->m_font = settings.value("Font").value<QFont>();
    cfg->m_fontSize = settings.value("FontSize").toInt();
    cfg->m_feeds = settings.value("Feeds").value<QStringList>();
    cfg->m_windowPos = settings.value("WindowPos").value<QRect>();
    cfg->m_fixWindowPos = settings.value("FixWindowPos").toBool();
}

void Configuration::saveConfiguration()
{
    //TODO:Get the correct directory
    QSettings settings(getDataLocation() + "/NetNewsTicker.cfg", QSettings::NativeFormat);
    Configuration * cfg = INSTANCE;
    settings.setValue("RefreshInterval", cfg->m_refreshInterval);
    settings.setValue("MaxItems", cfg->m_maxItems);
    settings.setValue("TickerSpeed", cfg->m_tickerSpeed);
    settings.setValue("BkgColor", cfg->m_bkgColor);
    settings.setValue("TextColor", cfg->m_textColor);
    settings.setValue("Font", cfg->m_font);
    settings.setValue("FontSize", cfg->m_fontSize);
    settings.setValue("Feeds", cfg->m_feeds);
    settings.setValue("WindowPos", cfg->m_windowPos);
    settings.setValue("FixWindowPos", cfg->m_fixWindowPos);
    settings.sync();
    emit configChanged(this);
}

Configuration* Configuration::getInstance()
{
    if(INSTANCE==NULL)
    {
        INSTANCE = new Configuration();
        //Load Config when instantiated
        INSTANCE->loadConfiguration();
    }
    return INSTANCE;
}

