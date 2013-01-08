#ifndef FAVICONMANAGER_H
#define FAVICONMANAGER_H

#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>


class FavIconManager : public QObject
{
    Q_OBJECT
public:
    static FavIconManager* getInstance();
    void cacheFavIcon(QString url);
    QPixmap& getFavIcon(QString url);

private:
    FavIconManager();
    ~FavIconManager();

private:
    QNetworkAccessManager* m_manager;
    QNetworkReply* m_reply;
    QMap<QString, QPixmap> m_iconMap;
    static FavIconManager* m_instance;

private slots:
    void iconFinished(QNetworkReply *);
};

#endif // FAVICONMANAGER_H
