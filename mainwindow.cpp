#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _parser(nullptr)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(startParsing()));
}

void MainWindow::startParsing()
{
  QString path = ui->lineEdit->text();

  if (_parser) delete(_parser);
  _parser = new SongParser(path);
   connect(_parser, SIGNAL(advanced(uint,uint)), this, SLOT(parseProgressed(uint, uint)));
  _parser->Start();
}

void MainWindow::parseProgressed(uint progress, uint total)
{
  ui->progressBar->setMaximum(total);
  ui->progressBar->setValue(progress);
  ui->progressBar->setFormat(QString::number(progress) + " / " + QString::number(total));
  ui->progressBar->update();
}

MainWindow::~MainWindow()
{
  delete _parser;
  delete ui;
}
