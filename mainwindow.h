#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "songparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void parseProgressed(uint, uint);
    void startParsing();

private:
    Ui::MainWindow *ui;
    SongParser::Parser* _parser;
};

#endif // MAINWINDOW_H
