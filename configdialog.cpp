#include "configdialog.h"
#include "ui_configdialog.h"

#include "configuration.h"

#include <QInputDialog>
#include <QStringListModel>
#include <QStringList>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    Configuration* cfg = Configuration::getInstance();
    cfg->loadConfiguration();
    ui->m_spMaxItems->setValue(cfg->m_maxItems);
    ui->m_spTickerSpeed->setValue(cfg->m_tickerSpeed);
    ui->m_spRefreshInterval->setValue(cfg->m_refreshInterval);
    ui->m_btnBgkColor->setColor(cfg->m_bkgColor);
    ui->m_btnTickerColor->setColor(cfg->m_textColor);
    ui->m_spMaxItems->setValue(cfg->m_maxItems);
    ui->m_cboFont->setCurrentFont(cfg->m_font);
    ui->m_spFontSize->setValue(cfg->m_fontSize);

    ui->m_lstFeeds->setModel(new QStringListModel(cfg->m_feeds));
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}


void ConfigDialog::on_m_btnOk_clicked()
{
    //apply configuration
    Configuration* cfg = Configuration::getInstance();
    cfg->m_bkgColor = ui->m_btnBgkColor->color();
    cfg->m_textColor = ui->m_btnTickerColor->color();
    cfg->m_tickerSpeed = ui->m_spTickerSpeed->value();
    cfg->m_maxItems = ui->m_spMaxItems->value();
    cfg->m_refreshInterval = ui->m_spRefreshInterval->value();
    cfg->m_font = ui->m_cboFont->currentFont();
    cfg->m_fontSize = ui->m_spFontSize->value();
    cfg->saveConfiguration();
    this->close();
}

void ConfigDialog::on_m_btnCancel_clicked()
{
    this->close();
}

void ConfigDialog::on_m_btnAddFeed_clicked()
{
    QString url = QInputDialog::getText(this, "Add Feed","Please input the URL of your Feed");
    if(url==NULL || url.isEmpty())
    {
        return;
    }
    Configuration* cfg = Configuration::getInstance();
    cfg->m_feeds.append(url);
    cfg->saveConfiguration();
    ((QStringListModel*) ui->m_lstFeeds->model())->setStringList(cfg->m_feeds);
}

void ConfigDialog::on_m_btnDelFeed_clicked()
{

}
