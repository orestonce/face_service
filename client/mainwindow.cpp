#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPaintEvent>
#include <QPainter>
#include <QFileDialog>
#include <QPicture>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QMessageBox>
#include <QBuffer>
#include <QTime>
#include <assert.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_NetMgr(NULL)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(this->contentsRect().topLeft(), this->m_ShowImage);
    p.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
    for (const QRect& r : this->m_FaceList)
    {
        p.drawRect(r);
    }
    for (const QPoint& kp: this->m_KeyPointList)
    {
        p.drawPoint(kp);
    }
}

void MainWindow::reloadImage()
{
    if (this->m_ImagePath.isEmpty())
    {
        return;
    }
    QFile f(this->m_ImagePath);
    f.open(QIODevice::ReadOnly);
    QByteArray data= f.readAll();
    QImage img;
    bool ok = img.loadFromData(data);
    if (!ok)
    {
        this->reportError("加载图片失败");
        return;
    }
    QSize sz;
    sz.setWidth(this->ui->groupBoxRight->width()+img.width()+14);
    sz.setHeight(std::max(this->ui->groupBoxRight->height(), img.height()));

    this->resize(sz);
    this->m_ShowImage = img;
    this->m_FaceList.clear();
    this->m_KeyPointList.clear();

    if (this->m_NetMgr == NULL)
    {
        this->m_NetMgr = new QNetworkAccessManager(this);
    }
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("image_content", data.toPercentEncoding());
    QString urlStr = this->ui->lineEdit_serverAdrress->text();
    QString curText = this->ui->comboBox_algorithm->currentText();
    if ( curText == "facedetectcnn") {
        req.setUrl(QUrl(urlStr + "facedetectcnn"));
    } else if (curText == "seetaface2-point5") {
        req.setUrl(QUrl(urlStr + "facedetectsf"));
        params.addQueryItem("use_pts", "5");
    } else if (curText == "seetaface2-point81") {
        req.setUrl(QUrl(urlStr + "facedetectsf"));
        params.addQueryItem("use_pts", "81");
    } else {
        assert(false);
    }

    QTime startTime = QTime::currentTime();
    QNetworkReply* reply = this->m_NetMgr->post(req, params.query().toUtf8());
    connect(reply, &QNetworkReply::finished, [=](){
        if (reply->error() != 0 )
        {
            this->reportError(reply->errorString());
            return;
        }
        int msec = startTime.msecsTo(QTime::currentTime());
        this->ui->lineEdit_usedTime->setText(QString().sprintf("%d ms", msec));
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonArray array = doc.array();
        for (QJsonArray::iterator it=array.begin(); it != array.end(); it++)
        {
            QJsonObject obj = it->toObject();
            QRect f;
            f.setX(obj["X"].toInt());
            f.setY(obj["Y"].toInt());
            f.setWidth(obj["W"].toInt());
            f.setHeight(obj["H"].toInt());
            this->m_FaceList.push_back(f);
            QJsonArray kpArray = obj["KeyPointList"].toArray();
            for (QJsonArray::Iterator it=kpArray.begin(); it != kpArray.end(); it++)
            {
                QJsonObject obj = it->toObject();
                QPoint p;
                p.setX(obj["X"].toInt());
                p.setY(obj["Y"].toInt());
                this->m_KeyPointList.push_back(p);
            }
        }
        this->repaint();
    });
}

void MainWindow::on_pushButton_startRun_clicked()
{
    this->reloadImage();
}

void MainWindow::on_lineEdit_serverAdrress_editingFinished()
{
    QString text = this->ui->lineEdit_serverAdrress->text();
    if (!text.endsWith("/") && !text.isEmpty()) {
        text.append("/");
        this->ui->lineEdit_serverAdrress->setText(text);
    }
}

void MainWindow::on_pushButton_openFile_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, QString(), "D:/", "*.jpg *.png");
    this->m_ImagePath = imagePath;
    this->reloadImage();
}

void MainWindow::reportError(const QString& errStr)
{
    QMessageBox::warning(this, "错误", errStr);
}
