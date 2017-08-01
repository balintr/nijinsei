#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SongParser parser(QString("/home/s3l3ctr/.local/share/osu-wine/OSU/Songs"));
    parser.Start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
