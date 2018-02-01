#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <algorithm>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plot->addGraph();

    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseData()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load data"), "",
                                                    tr("file (*.txt);;All Files (*)"));
    if (fileName.isEmpty())
            return;
    else {

        QFile file(fileName);

        QTextStream in(&file);

        if (!file.open(QIODevice::ReadOnly)) {

            qWarning("Cannot open file for reading");
            return;

        }

        int  i = 0;

        while (!in.atEnd()) {

            QString line = in.readLine();

            QStringList lineElements = line.split(":\t");

            // extract just the value (exclude currency information)
            QRegularExpression valueRe("[\\d|.]+");
            QString value = valueRe.match(lineElements.at(1)).captured(0);

            // parse the date
            QString customDateString = lineElements.at(0);
            QDateTime timestamp = QDateTime::fromString(customDateString, "ddd MMM d HH:mm:ss yyyy");

            qDebug() << i << line;
            qDebug() << i << timestamp << "\t\t" << value;

            // store data
            qv_time.append(timestamp.toTime_t());
            qv_target.append(value.toDouble());

            i++;
        }

        file.close();
    }
}

void MainWindow::plot()
{
    ui->plot->graph(0)->setData(qv_time, qv_target);

    double max = *std::max_element(qv_target.constBegin(), qv_target.constEnd());

    ui->plot->xAxis->setRange(qv_time.at(0), qv_time.back());
    ui->plot->yAxis->setRange(0, max * 1.5);

    ui->plot->xAxis->setLabel("Time");
    ui->plot->yAxis->setLabel("Portfolio value [USD]");

    ui->plot->replot();
}

void MainWindow::on_loadDataButton_clicked()
{
    parseData();
    plot();
}
