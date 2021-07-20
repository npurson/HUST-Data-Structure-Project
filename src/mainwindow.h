#pragma once
#include "parser.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QMessageBox>
#include <QDialog>
#include <QFileDialog>
#include <QStyle>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFile>
#include <QFileDevice>
#include <QProcess>
#include <QTextStream>
#include <QDataStream>
#include <QtEvents>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTextEdit* textEdit;
    char text[STR_LENGTH] = { 0 };
    char** analysis;
    void updateText();
    void openF();

signals:
    void textChanged();
};
