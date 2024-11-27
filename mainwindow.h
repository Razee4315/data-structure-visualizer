#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "stackvisualizer.h"
#include "queuevisualizer.h"
#include "infixtopostfix.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showAboutDialog();
    void showStackHelp();
    void showQueueHelp();
    void showInfixToPostfixHelp();

private:
    Ui::MainWindow *ui;
    StackVisualizer *stackVisualizer;
    QueueVisualizer *queueVisualizer;
    InfixToPostfix *infixToPostfix;
    QTabWidget *tabWidget;

    void setupMenuBar();
    void createHelpMenu();
};

#endif // MAINWINDOW_H
