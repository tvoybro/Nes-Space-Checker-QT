#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <about.h>
#include <QMimeData>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    About *about;

private slots:
    void updateTimer();
    void DrawPRG(int x,int y,char *data, QPainter *localpainter, int fill);
    void DrawCHR(int x,int y,char *data, QPainter *localpainter);

    bool LoadNES(QString file_name);
    void ShowNES();
    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

    void on_action0xff_triggered(bool checked);

    void on_action0x00_triggered(bool checked);

    void on_actionAuto_triggered(bool checked);

    void on_actionReload_triggered();

    void on_actionWatch_triggered(bool checked);

    void on_actionAbout_triggered();

    void on_actionSave_report_triggered();

public slots:
    void clear();

signals:
    void changed(const QMimeData *mimeData = nullptr);

private:
    void readSettings();
    void writeSettings();
    Ui::MainWindow *ui;
    QTimer *timer;
    QDateTime lastmodified;
    QString filename;
};
#endif // MAINWINDOW_H
