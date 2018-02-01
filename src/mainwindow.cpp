#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <algorithm>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plotPortfolioValue->addGraph();

    ui->plotPortfolioValue->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plotPortfolioValue->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseDirectory()
{

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::DontUseNativeDialog,true);
    dialog.setOption(QFileDialog::DontResolveSymlinks);
    dialog.setViewMode(QFileDialog::Detail);

    QString directory =  dialog.getExistingDirectory(this);

    qDebug() << directory;

    QDirIterator it(directory, QStringList("*.txt"), QDir::Files);

    while (it.hasNext()) {

        // this is the file path, not the file name
        QString filePath = it.next();

        // just get the last element after splitting by "/" -> file name
        QString fileName = filePath.section("/", -1, -1);

        qDebug() << fileName;

        if (fileName.compare("portfolio_value_USD.txt") == 0) {
            // this is the porfolio value in USD
            parsePortfolioUSD(filePath);
        }
    }
}

void MainWindow::parsePortfolioUSD(QString fileName) {

    QFile file(fileName);

    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly)) {

        qWarning("Cannot open file for reading");
        return;

    }


    int i = 0;

    while (!in.atEnd()) {

        QDateTime timestamp;

        QString line = in.readLine();

        QStringList lineElements = line.split(":\t");

        // extract just the value (exclude currency information)
        QRegularExpression valueRe("[\\d|.]+");
        QString value = valueRe.match(lineElements.at(1)).captured(0);

        // parse the date
        QString customDateString = lineElements.at(0);
        // sometimes there will be 2 spaces in
        customDateString.replace("  ", " ");

        timestamp = QDateTime::fromString(customDateString, "ddd MMM d HH:mm:ss yyyy");

        qDebug() << i << line;
        qDebug() << i << timestamp << "\t\t" << value;

        // store data
        qv_time.append(timestamp.toTime_t());
        qv_target.append(value.toDouble());

        i++;
    }

    file.close();
}

void MainWindow::plot()
{
    ui->plotPortfolioValue->graph(0)->setData(qv_time, qv_target);

    double max = *std::max_element(qv_target.constBegin(), qv_target.constEnd());

    ui->plotPortfolioValue->xAxis->setRange(qv_time.at(0), qv_time.back());
    ui->plotPortfolioValue->yAxis->setRange(0, max * 1.5);

    ui->plotPortfolioValue->xAxis->setLabel("Time");
    ui->plotPortfolioValue->yAxis->setLabel("Portfolio value [USD]");

    ui->plotPortfolioValue->replot();
}

void MainWindow::on_loadDataButton_clicked()
{
    parseDirectory();
    plot();
}
