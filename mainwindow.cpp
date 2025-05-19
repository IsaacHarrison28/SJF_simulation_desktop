#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLineEdit>
#include <QIntValidator>
#include <QTableWidgetItem>
#include <vector>
#include <algorithm>

QVector<QLineEdit*> burstInputs;
QVector<Process> processes;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , processCount(0)
{
    ui->setupUi(this);
    ui->averageResultsLabel->setText(""); // Clear result label at start

    // Explicitly set table headers to ensure they show
    QStringList headers = {"Process ID", "Burst Time", "Waiting Time", "Turnaround Time"};
    ui->resultsTable->setColumnCount(headers.size());
    ui->resultsTable->setHorizontalHeaderLabels(headers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_processes_continue_clicked()
{
    processCount = ui->process_num->text().toInt();
    if (processCount <= 0) return;

    ui->stackedWidget->setCurrentIndex(1);

    // Clear previous inputs
    QLayoutItem *item;
    while ((item = ui->processInputLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    burstInputs.clear();

    for (int i = 0; i < processCount; ++i) {
        QLineEdit *input = new QLineEdit(this);
        input->setPlaceholderText("Burst Time for Process " + QString::number(i + 1));
        input->setObjectName("burstInput_" + QString::number(i));
        input->setMinimumHeight(40);
        input->setStyleSheet("padding: 8px; font-size: 16px;");
        input->setAlignment(Qt::AlignCenter);
        input->setValidator(new QIntValidator(1, 10000, this));  // Only allow positive integers
        ui->processInputLayout->addWidget(input);
        burstInputs.push_back(input);
    }
}

void MainWindow::on_simulate_button_clicked()
{
    processes.clear();

    // Validate inputs
    for (int i = 0; i < burstInputs.size(); ++i) {
        QString text = burstInputs[i]->text();
        int burstTime = text.toInt();
        if (burstTime <= 0) burstTime = 1; // fallback if somehow invalid

        Process p;
        p.id = i + 1;
        p.burstTime = burstTime;
        processes.push_back(p);
    }

    // Sort processes by burst time ascending (SJF)
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.burstTime < b.burstTime;
    });

    int totalWT = 0, totalTAT = 0;
    int currentTime = 0;

    // ==============================================
    // TABLE SETUP AND STYLING
    // ==============================================
    ui->resultsTable->setRowCount(processes.size());
    
    // Remove vertical header (row numbers)
    ui->resultsTable->verticalHeader()->setVisible(false);

    // Configure horizontal header
    ui->resultsTable->horizontalHeader()->setVisible(true);
    ui->resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultsTable->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #2b7bb9;"  // Match your button color
        "    color: white;"              // White text
        "    padding: 8px;"
        "    border: 1px solid #225c8c;" // Darker border
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    text-align: center;"
        "}"
    );

    // Style the table itself
    ui->resultsTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    alternate-background-color: #f4f6fa;"  // Match window bg
        "    gridline-color: #ddd;"
        "    font-size: 14px;"
        "    selection-background-color: #d8e6f2;"
        "    selection-color: black;"
        "}"
        "QTableWidget::item {"
        "    padding: 6px;"
        "}"
    );

    // Enable alternating row colors
    ui->resultsTable->setAlternatingRowColors(true);

    // ==============================================
    // FILL TABLE DATA
    // ==============================================
    for (int i = 0; i < processes.size(); ++i) {
        int waitingTime = currentTime;
        int turnaroundTime = waitingTime + processes[i].burstTime;

        // Create items and center-align them
        QTableWidgetItem* idItem = new QTableWidgetItem("P" + QString::number(processes[i].id));
        QTableWidgetItem* burstItem = new QTableWidgetItem(QString::number(processes[i].burstTime));
        QTableWidgetItem* waitItem = new QTableWidgetItem(QString::number(waitingTime));
        QTableWidgetItem* turnaroundItem = new QTableWidgetItem(QString::number(turnaroundTime));

        // Center-align all cells
        idItem->setTextAlignment(Qt::AlignCenter);
        burstItem->setTextAlignment(Qt::AlignCenter);
        waitItem->setTextAlignment(Qt::AlignCenter);
        turnaroundItem->setTextAlignment(Qt::AlignCenter);

        // Set items in table
        ui->resultsTable->setItem(i, 0, idItem);
        ui->resultsTable->setItem(i, 1, burstItem);
        ui->resultsTable->setItem(i, 2, waitItem);
        ui->resultsTable->setItem(i, 3, turnaroundItem);

        currentTime += processes[i].burstTime;
        totalWT += waitingTime;
        totalTAT += turnaroundTime;
    }

    // ==============================================
    // CALCULATE AND DISPLAY AVERAGES
    // ==============================================
    float avgWT = (float)totalWT / processes.size();
    float avgTAT = (float)totalTAT / processes.size();

    ui->averageResultsLabel->setText(
        QString("Average Waiting Time: %1   |   Average Turnaround Time: %2")
        .arg(avgWT, 0, 'f', 2)
        .arg(avgTAT, 0, 'f', 2)
    );

    // Switch to results page
    ui->stackedWidget->setCurrentIndex(2);
}