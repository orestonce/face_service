#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QNetworkAccessManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    virtual void paintEvent(QPaintEvent*);

private slots:
    void on_pushButton_startRun_clicked();

    void on_lineEdit_serverAdrress_editingFinished();

    void on_pushButton_openFile_clicked();

private:
    void reloadImage();
    void reportError(const QString& errStr);
private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *m_NetMgr;
    QString m_ImagePath;
    QImage m_ShowImage;
    QVector<QRect> m_FaceList;
    QVector<QPoint> m_KeyPointList;
};

#endif // MAINWINDOW_H
