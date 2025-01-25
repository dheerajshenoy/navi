#include "UpdateDialog.hpp"

UpdateDialog::UpdateDialog(QString version, QWidget *parent) : m_version(version), QDialog(parent) {
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_spinner);
    m_layout->addWidget(m_version_label);
    m_version_label->setText(QString("Current Version: %1").arg(version));
    m_spinner->setRoundness(70.0);
    m_spinner->setMinimumTrailOpacity(15.0);
    m_spinner->setTrailFadePercentage(70.0);
    m_spinner->setNumberOfLines(12);
    m_spinner->setLineLength(10);
    m_spinner->setLineWidth(5);
    m_spinner->setInnerRadius(10);
    m_spinner->setRevolutionsPerSecond(1);
    this->setLayout(m_layout);
    fetch_version_from_repo();
    m_label->setTextFormat(Qt::RichText);
    m_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_label->setOpenExternalLinks(true);
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignmentFlag::AlignVCenter);
}

void UpdateDialog::fetch_version_from_repo() noexcept {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    m_spinner->start();
    QString rawFileUrl = "https://raw.githubusercontent.com/dheerajshenoy/navi/refs/heads/main/latest-version.txt";
    QNetworkRequest request(rawFileUrl);

    QNetworkReply* reply = manager->get(request);

    connect(manager, &QNetworkAccessManager::finished, this, [&](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QString latestVersion = reply->readAll().trimmed().mid(1);

            if (latestVersion > m_version.mid(1)) {
                m_label->setText(QString("New version %1 available. Please update the software."
                                         " Check <a href='https://navi.readthedocs.io'>update section</a>"
                                         " on navi.readthedocs.io for more information on how to"
                                         " update Navi").arg(latestVersion));
            } else {
                m_label->setText("You have the latest version of Navi");
            }
        } else {
            m_label->setText(QString("Error checking for update:").arg(reply->errorString()));
        }
        m_spinner->stop();
        reply->deleteLater();
    });

}

