#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLineEdit>

struct Process {
    int id;
    int burstTime;
};

// These are declared globally in mainwindow.cpp
extern QVector<QLineEdit*> burstInputs;
extern QVector<Process> processes;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_processes_continue_clicked();
    void on_simulate_button_clicked();

private:
    Ui::MainWindow *ui;
    int processCount;
};

#endif // MAINWINDOW_H
