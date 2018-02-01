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
        dataHashTable["PortfolioUSD"]["x"].append(timestamp.toTime_t());
        dataHashTable["PortfolioUSD"]["y"].append(value.toDouble());

        i++;
    }

    plot(dataHashTable["PortfolioUSD"]["x"], dataHashTable["PortfolioUSD"]["y"], "Time", "Portfolio value", "USD", ui->plotPortfolioValue, 0);

    file.close();
}

void MainWindow::plot(QVector<double> x, QVector<double> y, QString xlabel, QString ylabel, QString label, QCustomPlot *plotObject, int graphNumber) {

    plotObject->legend->setVisible(true);

    plotObject->graph(graphNumber)->setData(x, y);
    plotObject->graph(graphNumber)->setName(label);

    double max = *std::max_element(y.constBegin(), y.constEnd());

    plotObject->xAxis->setRange(x[0], x.back());
    plotObject->yAxis->setRange(0, max * 1.5);

    plotObject->xAxis->setLabel(xlabel);
    plotObject->yAxis->setLabel(ylabel);

    plotObject->replot();
}

void MainWindow::on_loadDataButton_clicked()
{
    parseDirectory();
//    plot();
}
