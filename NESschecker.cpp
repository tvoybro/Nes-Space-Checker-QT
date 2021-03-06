#include "NESschecker.h"
#include "ui_formmain.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QPainter>
#include <QImage>
#include <QSettings>
#include <QCloseEvent>

static char *file_data;
static int file_size;
static int file_type;
static QString lastdir = "";

enum {
    TYPE_NES,
    TYPE_RAW
};

FormMain::FormMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FormMain)
{
    ui->setupUi(this);
    readSettings();
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
}


FormMain::~FormMain()
{
    delete ui;
}

void FormMain::DrawPRG(int x,int y,char *data,QPainter *localpainter, int fill)
{
    QImage PRGimage = QImage(64, 256, QImage::Format_RGB16);

    const int markw[17]={8,3,3,3,6,3,3,3,8,3,3,3,6,3,3,3,8};
    int i,j,n,bf,sy;
    char *src;

    src = data;

    localpainter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    localpainter->drawRect(x-1,y-1,66,258);

    for(i=0;i<256;++i)
    {
        for(j=0;j<64;++j) {
            if (!(*src++==fill))
                PRGimage.setPixel(j,i, qRgb(0,255,0));
            else
                PRGimage.setPixel(j,i, qRgb(255,255,255));
        }
    }

    src=data;
    bf=0;

    for(i=0;i<256;++i)
    {
        n=64;

        for(j=0;j<64;++j) {
            if(*src++)
                n=0;
        }

        bf+=n;
    }

    sy=y;

    for(i=0;i<17;++i)
    {
        localpainter->drawLine(x+66,sy-1, x+66+markw[i],sy-1);
        localpainter->drawLine(x+66,sy, x+66+markw[i],sy);
        sy+=16;
    }

    // Draw PRG data from temp QImage to QPainter
    localpainter->drawImage(QRect(x, y, 64, 256), PRGimage);
    localpainter->drawText(QPoint(x,y+270),"~" + QString::number(bf/1024) + "K free");
}

void FormMain::DrawCHR(int x,int y,char *data,QPainter *localpainter)
{
    QImage CHRimage = QImage(64, 128, QImage::Format_RGB16);

    const int markw[9]={8,3,6,3,8,3,6,3,8};
    int i,j,sy;


    localpainter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    localpainter->drawRect(x-1,y-1,66,130);

    for(i=0;i<128;++i)
    {
        for(j=0;j<64;++j) {
            if (*data++)
                CHRimage.setPixel(j,i, qRgb(50,50,230));
            else
                CHRimage.setPixel(j,i, qRgb(255,255,255));
        }
    }


    sy=y;

    for(i=0;i<9;++i)
    {
        localpainter->drawLine(x+66,sy-1, x+66+markw[i],sy-1);
        localpainter->drawLine(x+66,sy, x+66+markw[i],sy);
        sy+=16;
    }

    // Draw CHR data from temp QImage to QPainter
    localpainter->drawImage(QRect(x, y, 64, 128), CHRimage);
}

void FormMain::ShowNES()
{
    QPainter painter(this);

    // painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    // painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
    // painter.drawEllipse(100, 50, 150, 150);

    QString str;
    int lens[256];
    int i,j,x,y,prg,chr,mapper,off,fill,hgt,len,prev,max;

    if (!file_data) {
        painter.drawText(QPoint(10,40),"No file loaded");
        return;
    }

    if (file_type==TYPE_NES) {
        prg = file_data[4];
        chr = file_data[5];
        mapper=((file_data[6]>>4)&0x0f)|(file_data[7]&0xf0);
        off=16;
    } else {
        prg=(file_size+16383)/16384;
        chr=0;
        mapper=-1;
        off=0;
    }

    for(i=0;i<256;i++) lens[i]=0;

    for(i=0;i<prg;i++)
    {
        len=0;
        prev=file_data[off++];

        for(j=1;j<16384;++j)
        {
            if(file_data[off]==prev)
            {
                len++;
            }
            else
            {
                if(len>=8&&len>lens[prev]) lens[prev]=len;

                prev=file_data[off];

                len=0;
            }

            off++;
        }
    }

   if (ui->actionAuto->isChecked()) {
        max=-1;
        fill=-1;

        for(i=0;i<256;i++)
        {
            if(lens[i]>max)
            {
                max=lens[i];
                fill=i;
            }
        }
    } else {
        if(!(ui->action0xff->isChecked())) {
            fill=0x00;
        } else {
            fill=0xff;
        }
    }

    str = filename;

    if(file_type==TYPE_NES)
    {
        str+="  Mapper #"+QString::number(mapper)+"  "+QString::number(prg)+"xPRG  "+QString::number(chr)+"xCHR  Empty is $"+QString::number(fill);
    }
    else
    {
        str+="  (raw binary, "+QString::number(file_size)+" bytes)";
    }

    // ImageView->Picture->Bitmap->Canvas->TextOut(10,10,str);

    painter.drawText(QPoint(10,40),str);

    off=16;
    x=10;
    y=60;
    hgt=256+80;

    // QFile file("d:\\nsc.dump");

    for(i=0;i<prg;i++)
    {
        // Draw PRG number
        QString PRGnumber = QString::number(i,16);
        PRGnumber = QString::fromStdString(std::string(2 - PRGnumber.length(), '0')) + PRGnumber;

        painter.drawText(QPoint(x,y),"PRG $" + PRGnumber);

        // Draw PRG data
        DrawPRG(x,y+10, &file_data[off], &painter, fill);

        off+=16384;
        x+=64+20;

        if(x+64+10>=ui->centralwidget->width()) {
            x=10;
            y+=hgt;
            if(i==prg-1)
                hgt=128+40;
        }
    }

    for(i=0;i<chr;i++)
    {
        // Draw CHR number
        QString CHRnumber = QString::number(i,16);
        CHRnumber = QString::fromStdString(std::string(2 - CHRnumber.length(), '0')) + CHRnumber;

        painter.drawText(QPoint(x,y),"CHR $" + CHRnumber);

        // Draw CHR data

        DrawCHR(x,y+10,&file_data[off], &painter);

        off+=8192;
        x+=64+20;

        if(x+64+10>=ui->centralwidget->width())
        {
            x=10;
            y+=hgt;
            hgt=128+40;
        }
    }
    painter.end();
}

bool FormMain::LoadNES(QString file_name)
{
    char NESHeader[16];

    QFile* nes_file = new QFile(file_name);

    filename = file_name;

    if(!nes_file->open(QIODevice::ReadOnly)) {
        return false;
    } else {
        nes_file->read(NESHeader, 16);
        QString str = QByteArray::fromRawData(NESHeader,16);
        nes_file->seek(0);
        str.resize(3);

        file_size = static_cast<int>(nes_file->size());

        int alloc_size;


        if (str.toUpper()=="NES") {
            file_type = TYPE_NES;
            alloc_size = file_size;
        } else {
            file_type = TYPE_RAW;
            alloc_size = (file_size+16383)/16384*16384;	//round the size up
        }

        // ui->statusbar->showMessage(QString::number(alloc_size));

        file_data = (char *) malloc(alloc_size);

        nes_file->read(file_data, alloc_size);

        QFileInfo fileinfo (file_name);

        lastmodified = fileinfo.lastModified();

        lastdir = fileinfo.path();

        nes_file->close();

        return true;
    }

}

void FormMain::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    ShowNES();
}

void FormMain::on_actionOpen_triggered()
{
    if (LoadNES(QFileDialog::getOpenFileName(this, "Open a file", lastdir))) {
       ui->actionReload->setEnabled(1);
    }
    ShowNES();
}

void FormMain::on_actionQuit_triggered()
{
    this->close();
}

void FormMain::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void FormMain::on_action0xff_triggered(bool checked)
{
    ui->actionAuto->setChecked(false);
    ui->action0x00->setChecked(false);
    ui->action0xff->setChecked(true);
    if (checked) {
        FormMain::repaint();
    }
}

void FormMain::on_action0x00_triggered(bool checked)
{
    ui->actionAuto->setChecked(false);
    ui->action0x00->setChecked(true);
    ui->action0xff->setChecked(false);

    if (checked) {
        FormMain::repaint();
    }
}

void FormMain::on_actionAuto_triggered(bool checked)
{
    ui->actionAuto->setChecked(true);
    ui->action0x00->setChecked(false);
    ui->action0xff->setChecked(false);
    if (checked) {
        FormMain::repaint();
    }
}

void FormMain::on_actionReload_triggered()
{
    LoadNES(filename);
    FormMain::repaint();
}

void FormMain::updateTimer()
{
    qDebug("Timer active");

    if (!(ui->actionWatch->isChecked()))
        return;

    QFileInfo fileinfo (filename);
    QDateTime modified = fileinfo.lastModified();
    if (modified!=lastmodified) {
        LoadNES(filename);
        FormMain::repaint();
    }
}

void FormMain::on_actionWatch_triggered(bool checked)
{
    if (checked)
        timer->start(1000);
    else
        timer->stop();
}

void FormMain::writeSettings()
{
    QSettings settings("nestea", "nes-space-checker");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("lastdir", lastdir);
}

void FormMain::readSettings()
{
    QSettings settings("nestea", "nes-space-checker");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    QString tmp = settings.value("lastdir", QDir::homePath()).toString();
    lastdir = tmp;
    resize(size);
    move(pos);
}

void FormMain::on_actionAbout_triggered()
{
    about = new About(this);
    about->show();
}
