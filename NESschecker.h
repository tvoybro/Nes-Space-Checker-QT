#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>
#include <QPainter>
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <about.h>


QT_BEGIN_NAMESPACE
namespace Ui { class FormMain; }
QT_END_NAMESPACE

class FormMain : public QMainWindow
{
    Q_OBJECT

public:
    FormMain(QWidget *parent = nullptr);
    ~FormMain();

protected:
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

private:
    void readSettings();
    void writeSettings();
    Ui::FormMain *ui;
    QTimer *timer;
    QDateTime lastmodified;
    QString filename;
};
#endif // FORMMAIN_H
