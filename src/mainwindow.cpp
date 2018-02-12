#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <algorithm>
#include <cmath>

QVector<QColor> COLORS = {Qt::blue, Qt::red};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createSpinBoxes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createSpinBoxes()
{
    ui->SMAValue->setRange(5,20000);
    ui->SMAValue->setValue(7200);
}

void MainWindow::parseDirectory(int SMAValue)
{

    QDirIterator it(currentDirectory, QStringList("*.txt"), QDir::Files);

    int speedLogFile = 0;
    int priceLogFile = 0;

    maxXRange = -1;
    minXRange = QDateTime::currentDateTime().toTime_t();

    QRegularExpression priceRe("price_log_(.*)\\.txt");
    QRegularExpression speedRe("speed_log_(.*)\\.txt");
    
    while (it.hasNext()) {

        // this is the file path, not the file name
        QString filePath = it.next();

        // just get the last element after splitting by "/" -> file name
        QString fileName = filePath.section("/", -1, -1);

//        qDebug() << fileName;

        if (fileName.compare("portfolio_value_USD.txt") == 0) {
            // this is the porfolio value in USD
            parsePortfolioUSD(filePath, SMAValue);
        }
        else if (fileName.compare("portfolio_value.txt") == 0) {
            parsePortfolioCoin(filePath, SMAValue);
        }
        else if (fileName.contains("speed_log_")) {
            
            parseSpeed(filePath, speedRe.match(fileName).captured(1), speedLogFile, SMAValue);

            speedLogFile++;
        }

        else if (fileName.contains("price_log_")) {
            
            parsePrice(filePath, priceRe.match(fileName).captured(1), priceLogFile, SMAValue);

            priceLogFile++;
        }
    }

    setFixedWindow();
    createTimeBoxes();
}

void MainWindow::parsePortfolioUSD(QString fileName, int SMAValue) {

    QFile file(fileName);

    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly)) {

        qWarning("Cannot open file for reading");
        return;

    }
    
    int i = 0;

    auto denomination = new QString("");

    while (!in.atEnd()) {

        QString line = in.readLine();

        QStringList lineElements = line.split(":\t");

        QDateTime timestamp = parseDate(lineElements.at(0));
        QString value = parseValue(lineElements.at(1), denomination);

//        qDebug() << i << line;
//        qDebug() << i << timestamp << "\t\t" << value;

        // store data
        dataHashTable["PortfolioUSD"]["x"].append(timestamp.toTime_t());
        dataHashTable["PortfolioUSD"]["y"].append(value.toDouble());

        i++;
    }

    file.close();

    double max = *std::max_element(dataHashTable["PortfolioUSD"]["y"].constBegin(),
                                   dataHashTable["PortfolioUSD"]["y"].constEnd());

    SMA(dataHashTable["PortfolioUSD"]["x"], dataHashTable["PortfolioUSD"]["y"], "Time",
        "Portfolio value", *denomination, ui->plotPortfolioValue, 0,
        dataHashTable["PortfolioUSD"]["x"][0], dataHashTable["PortfolioUSD"]["x"].back(),
        0.0, max * 1.5, SMAValue);

}

void MainWindow::parsePortfolioCoin(QString fileName, int SMAValue)
{
    QFile file(fileName);

    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly)) {

        qWarning("Cannot open file for reading");
        return;

    }

    int i = 0;

    QVector<QString> coinNames;

    bool first = true;

    while (!in.atEnd()) {

        QString line = in.readLine();

        QStringList lineElements = line.split(":\t");

        QDateTime timestamp = parseDate(lineElements.at(0));

        QStringList valueElements = lineElements.at(1).split(",\t");

        // parse over each column of each line

        for (int i=0; i<valueElements.length(); i++) {

            QString *coinName = new QString("");

            QString value = parseValue(valueElements[i], coinName);

            if (first) {
                coinNames.append(*coinName);
            }

            QString name = QString("PortfolioCoin_%1").arg(*coinName);

            // store data
            dataHashTable[name]["x"].append(timestamp.toTime_t());
            dataHashTable[name]["y"].append(value.toDouble());
        }

        i++;

        first = false;
    }

    file.close();

    double max = 0;

    for (int i=0; i<coinNames.length(); i++) {

        QString name = QString("PortfolioCoin_%1").arg(coinNames[i]);

        double max_i = *std::max_element(dataHashTable[name]["y"].constBegin(),
                                         dataHashTable[name]["y"].constEnd());

        if (max_i > max) {
            max = max_i;
        }
    }

    for (int i=0; i<coinNames.length(); i++) {

        QString name = QString("PortfolioCoin_%1").arg(coinNames[i]);

        SMA(dataHashTable[name]["x"], dataHashTable[name]["y"], "Time",
                "Portfolio assets", coinNames[i], ui->plotPortfolioCoins, i,
                dataHashTable[name]["x"][0], dataHashTable[name]["x"].back(),
                0.0, max * 1.5, SMAValue);
    }
}

void MainWindow::parseSpeed(QString fileName, QString orderName, int orderNumber,
                            int SMAValue){

    QFile file(fileName);

    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly)) {

        qWarning("Cannot open file for reading");
        return;

    }

    auto name = QString("speed_%1").arg(orderName);

    auto speedUnit = new QString;

    int i = 0;

    while (!in.atEnd()) {

        QString line = in.readLine();

        QStringList lineElements = line.split(": ");

        QDateTime timestamp = parseDate(lineElements.at(0));
        QString value = parseValue(lineElements.at(1), speedUnit);

//        qDebug() << i << line;
//        qDebug() << i << timestamp << "\t\t" << value;

        // store data
        dataHashTable[name]["x"].append(timestamp.toTime_t());
        dataHashTable[name]["y"].append(value.toDouble());

        i++;
    }

    file.close();

    double max = *std::max_element(dataHashTable[name]["y"].constBegin(),
                                   dataHashTable[name]["y"].constEnd());

    SMA(dataHashTable[name]["x"], dataHashTable[name]["y"], "Time",
        "Mining Speed", orderName, ui->plotSpeed, orderNumber,
        dataHashTable[name]["x"][0], dataHashTable[name]["x"].back(),
            0.0, max * 1.5, SMAValue);
}

void MainWindow::parsePrice(QString fileName, QString orderName, int orderNumber,
                            int SMAValue)
{
    QFile file(fileName);

    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly)) {

        qWarning("Cannot open file for reading");
        return;

    }

    auto name = QString("price_%1").arg(orderName);

    auto speedUnit = new QString;

    int i = 0;

    while (!in.atEnd()) {

        QString line = in.readLine();

        QStringList lineElements = line.split(": ");

        QDateTime timestamp = parseDate(lineElements.at(0));
        QString value = parseValue(lineElements.at(1), speedUnit);

//        qDebug() << i << line;
//        qDebug() << i << timestamp << "\t\t" << value;

        // store data
        dataHashTable[name]["x"].append(timestamp.toTime_t());
        dataHashTable[name]["y"].append(value.toDouble());

        i++;
    }

    file.close();

    double max = *std::max_element(dataHashTable[name]["y"].constBegin(),
                                   dataHashTable[name]["y"].constEnd());

    SMA(dataHashTable[name]["x"], dataHashTable[name]["y"], "Time",
        "Mining Power Price", orderName, ui->plotPrice, orderNumber,
        dataHashTable[name]["x"][0], dataHashTable[name]["x"].back(),
            0.0, max * 1.5, SMAValue);
}

QDateTime MainWindow::parseDate(QString element) {

    // parse the date
    // sometimes there will be 2 spaces in

    element.replace("[", "");
    element.replace("]", "");

    element.replace("  ", " ");

    return QDateTime::fromString(element, "ddd MMM d HH:mm:ss yyyy");
}

QString MainWindow::parseValue(QString element, QString *denomination) {

    // extract just the value (exclude currency information)
    QRegularExpression valueRe("[\\d|.]+");
    QRegularExpression remainingRe("[A-Z]+");

    *denomination = remainingRe.match(element).captured(0);

    return valueRe.match(element).captured(0);
}

void MainWindow::SMA(QVector<double> x, QVector<double> y, QString xlabel,
                     QString ylabel, QString label, QCustomPlot *plotObject,
                     int graphNumber, double xminRange, double xmaxRange,
                     double yminRange, double ymaxRange, int SMAValue)
{

    double step = 3600;

    QVector<double> newTimestamps;
    QVector<double> newValues;

    // get number of steps from start to end
    int nSteps = floor((x.back() - x[0]) / step);

    for (int i=0; i<nSteps; i++) {

        // current timestep
        double currentStep = x[0] + (i * step);

        newTimestamps.append(currentStep);

        // get last N steps to calculate moving average
        int j = 0;
        int lower = -1;
        int upper = -1;

        if (i == 0) {
//            qDebug() << "MA: " << QString::number(currentStep, 'g', 10) << y[0];
            newValues.append(y[0]);
        }

        else {
            for(QVector<double>::iterator it = x.begin() + 1; it !=x.end(); ++it) {

                if (*it >= (currentStep - SMAValue) && (lower == -1)) {
                    lower = j;
                }
                if (*it >= currentStep) {
                    upper = j;
                }

                if ((lower > -1) && (upper > -1)){
                    break;
                }

                j++;
            }

            QVector<double>::const_iterator lower_bound = y.begin() + lower;
            QVector<double>::const_iterator upper_bound = y.begin() + upper;

            // need to use vector, since QVector cannot handle instantiation with const..
            std::vector<double> lastNSteps(lower_bound, upper_bound);

//            qDebug() << "MA: " << QString::number(currentStep, 'g', 10) << std::accumulate(lastNSteps.begin(), lastNSteps.end(), 0.0) / lastNSteps.size();

            double smaValue = std::accumulate(lastNSteps.begin(), lastNSteps.end(), 0.0) / lastNSteps.size();

            newValues.append(smaValue);
        }
    }

    int firstNaPos = -1;
    int lastNaPos = -1;
    int i = 0;
    double first, last;
    // now we need to interpolate NaN values
    for(QVector<double>::iterator it = newValues.begin(); it != newValues.end(); ++it) {
        if (std::isnan(*it) && firstNaPos < 0) {
            // NaN value!
            // hit first Na, so store previous value which is not NaN
            // if it's the first value just put it as 0
            (it == newValues.begin()) ? firstNaPos = 0 : firstNaPos = i - 1;
            }

        else if (!std::isnan(*it) && lastNaPos < 0 && firstNaPos > -1) {
            // if it is not a NaN, but we found a NaN
            (it == newValues.end()) ? lastNaPos = i : lastNaPos = i + 1;
            }

        if (firstNaPos > -1 && lastNaPos > -1){
            // and now that we have both corners we do linear interpolation
            // check boundaries, and if outside date size put 0
            (firstNaPos == 0) ? first = 0: first = newValues[firstNaPos];
            (lastNaPos == newValues.length()) ? last = 0: last = newValues[lastNaPos];

            double diff = last - first;
            double stepwiseDelta = diff / (lastNaPos - firstNaPos + 1);

            int iter = 1;
            // and fill NaN
            for (int j = firstNaPos; j < lastNaPos; j++) {
                newValues[j] = first + iter * stepwiseDelta;
                iter++;
            }

            // reset values of positions to detect more NaN
            firstNaPos = -1;
            lastNaPos = -1;
        }

        i++;
    }

    double maxXCurrent = *std::max_element(newTimestamps.constBegin(),
                                    newTimestamps.constEnd());

    qDebug() << maxXCurrent;

    if (maxXCurrent > maxXRange)
        maxXRange = maxXCurrent;

    double minXCurrent = *std::min_element(newTimestamps.constBegin(),
                                           newTimestamps.constEnd());

    qDebug() << minXCurrent;

    if (minXCurrent < minXRange)
        minXRange = minXCurrent;


    plot(newTimestamps, newValues, xlabel, ylabel, label, plotObject, graphNumber,
         xminRange, xmaxRange, yminRange, ymaxRange);
}

void MainWindow::plot(QVector<double> x, QVector<double> y, QString xlabel, QString ylabel,
                      QString label, QCustomPlot *plotObject, int graphNumber, double xminRange,
                      double xmaxRange, double yminRange, double ymaxRange) {

    plotObject->addGraph();

    plotObject->graph(graphNumber)->selectionDecorator()->setPen(QPen(Qt::green));

//    plotObject->graph(graphNumber)->setScatterStyle(QCPScatterStyle::ssCircle);
    plotObject->graph(graphNumber)->setLineStyle(QCPGraph::LineStyle::lsLine);
    plotObject->graph(graphNumber)->setPen(QPen(COLORS[graphNumber]));

    plotObject->legend->setVisible(true);

    plotObject->graph(graphNumber)->setData(x, y);
    plotObject->graph(graphNumber)->setName(label);

//    plotObject->xAxis->setRange(xminRange, xmaxRange);
    plotObject->yAxis->setRange(yminRange, ymaxRange);

    plotObject->xAxis->setLabel(xlabel);
    plotObject->yAxis->setLabel(ylabel);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd/MM/yy\nhh:mm:ss");
    plotObject->xAxis->setTicker(dateTicker);
    plotObject->xAxis->setTickLabelRotation(60);

    plotObject->replot();
}

void MainWindow::clearPlots()
{

    clearPlot(ui->plotPortfolioCoins, ui->plotPortfolioCoins->graphCount());
    clearPlot(ui->plotPortfolioValue, ui->plotPortfolioValue->graphCount());
    clearPlot(ui->plotPrice, ui->plotPrice->graphCount());
    clearPlot(ui->plotSpeed, ui->plotSpeed->graphCount());

}

void MainWindow::clearPlot(QCustomPlot *plotObject, int count)
{
    for( int g=0; g<count; g++)
    {
        plotObject->removeGraph(0);
    }
    plotObject->replot();

}

void MainWindow::setFixedWindow() {

    qDebug() << minXRange << maxXRange;

    ui->plotPortfolioCoins->xAxis->setRange(minXRange, maxXRange);
    ui->plotPortfolioCoins->replot();
    ui->plotPortfolioValue->xAxis->setRange(minXRange, maxXRange);
    ui->plotPortfolioValue->replot();
    ui->plotPrice->xAxis->setRange(minXRange, maxXRange);
    ui->plotPrice->replot();
    ui->plotSpeed->xAxis->setRange(minXRange, maxXRange);
    ui->plotSpeed->replot();
}

//void MainWindow::on_SMAValue_valueChanged(int SMAValue)
//{
//    clearPlots();
//    parseDirectory(SMAValue);
//}

void MainWindow::on_SMAValue_editingFinished()
{
    clearPlots();
    parseDirectory(this->ui->SMAValue->text().toInt());
//    on_SMAValue_valueChanged(this->ui->SMAValue->text().toInt());
}


void MainWindow::on_loadDataButton_clicked()
{
    clearPlots();

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::DontUseNativeDialog,true);
    dialog.setOption(QFileDialog::DontResolveSymlinks);
    dialog.setViewMode(QFileDialog::Detail);

    currentDirectory =  dialog.getExistingDirectory(this);

    parseDirectory(14400);
}


void MainWindow::on_startTimeEdit_editingFinished()
{
    minXRange = this->ui->startTimeEdit->dateTime().toTime_t();
    setFixedWindow();
}

void MainWindow::on_endTimeEdit_editingFinished()
{
    maxXRange = this->ui->endTimeEdit->dateTime().toTime_t();
    setFixedWindow();
}

void MainWindow::createTimeBoxes() {
    QDateTime timestamp;
    timestamp.setTime_t(static_cast<uint>(maxXRange));
    this->ui->endTimeEdit->setDateTime(timestamp);
    timestamp.setTime_t(static_cast<uint>(minXRange));
    this->ui->startTimeEdit->setDateTime(timestamp);
}
