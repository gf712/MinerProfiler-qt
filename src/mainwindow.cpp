#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <algorithm>
#include <cmath>
#include <boost/range/combine.hpp>

//######################################################################################################################
//                                            CONSTANT DEFINITIONS
//######################################################################################################################

QVector<QColor> COLORS = {Qt::blue, Qt::red};
int DEFAULT_SMA = 60;

//######################################################################################################################
//                                          END OF CONSTANT DEFINITIONS
//######################################################################################################################



//######################################################################################################################
//                                    CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
//######################################################################################################################

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataLoaded = false;
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

//######################################################################################################################
//                                 END OF CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
//######################################################################################################################

//######################################################################################################################
//                                               PARSER ROUTINES
//######################################################################################################################

void MainWindow::parseDirectory()
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

        qDebug() << "FILENAME" << fileName;

        if (fileName.compare("portfolio_value_USD.txt") == 0) {
            // this is the porfolio value in USD
            parsePortfolioUSD(filePath);
        }
        else if (fileName.compare("portfolio_value.txt") == 0) {
            parsePortfolioCoin(filePath);
        }
        else if (fileName.contains("speed_log_")) {

            parseSpeed(filePath, speedRe.match(fileName).captured(1), speedLogFile);

            speedLogFile++;

            OrderNames.insert(speedRe.match(fileName).captured(1));
        }

        else if (fileName.contains("price_log_")) {

            parsePrice(filePath, priceRe.match(fileName).captured(1), priceLogFile);

            priceLogFile++;

            OrderNames.insert(priceRe.match(fileName).captured(1));
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

    auto denomination = new QString("");

    while (!in.atEnd()) {

        QString line = in.readLine();

        QStringList lineElements = line.split(":\t");

        QDateTime timestamp = parseDate(lineElements.at(0));
        QString value = parseValue(lineElements.at(1), denomination);

        // store data
        dataHashTable["PortfolioUSD"]["x"].append(timestamp.toTime_t());
        dataHashTable["PortfolioUSD"]["y"].append(value.toDouble());

        i++;
    }

    file.close();

}

void MainWindow::parsePortfolioCoin(QString fileName)
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

        for (int j=0; j<valueElements.length(); j++) {

            auto *coinName = new QString("");

            QString value = parseValue(valueElements[j], coinName);

            if (first) {
                CoinNames.push_back(*coinName);
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
}

void MainWindow::parseSpeed(QString fileName, QString orderName, int orderNumber){

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

        // store data
        dataHashTable[name]["x"].append(timestamp.toTime_t());
        dataHashTable[name]["y"].append(value.toDouble());

        i++;
    }

    file.close();
}

void MainWindow::parsePrice(QString fileName, QString orderName, int orderNumber)
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

        // store data
        dataHashTable[name]["x"].append(timestamp.toTime_t());
        dataHashTable[name]["y"].append(value.toDouble());

        i++;
    }

    file.close();
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


//######################################################################################################################
//                                           END OF PARSER ROUTINES
//######################################################################################################################


//######################################################################################################################
//                                           CLEANING DATA ROUTINES
//######################################################################################################################

void MainWindow::SMA(QVector<double> &x, QVector<double> &y,
                     QVector<double>* newTimestamps, QVector<double>* newValues)
{

    // get number of steps from start to end
    int nSteps = static_cast<int>(floor((x.back() - x[0]) / (SMAValue * 60))) + 1;

    qDebug() << x.size() << y.size() <<  nSteps;

    for (int i=0; i<nSteps; i++) {

        QVector<double> lastNSteps;

        // current timestep
        double currentStep = x[0] + (i * (SMAValue * 60));

        QDateTime timestamp;
        timestamp.setTime_t(currentStep);

        newTimestamps->append(currentStep);

        // get last N steps to calculate moving average
        if (i == 0) {
            newValues->append(y[0]);
        }

        else {
            // super useful boost function for parallel looping
            for(auto const& values: boost::combine(x, y))
            {

                double x_i, y_i;

                boost::tie(x_i, y_i) = values;

                if (x_i > currentStep) {
                    break;
                }

                if (x_i >= (currentStep - (SMAValue * 60))) {
                    lastNSteps.push_back(y_i);

                }
            }

            double smaValue = std::accumulate(lastNSteps.begin(), lastNSteps.end(), 0.0) / lastNSteps.size();
            QDateTime timestamp;
            timestamp.setTime_t(currentStep);

            newValues->append(smaValue);
        }
    }


    int firstNaPos = -1;
    int lastNaPos = -1;
    int i = 0;
    double first, last;
    // now we need to interpolate NaN values
    for(auto it = newValues->begin(); it != newValues->end(); ++it) {
        if (std::isnan(*it) && firstNaPos < 0) {
            // NaN value!
            // hit first Na, so store previous value which is not NaN
            // if it's the first value just put it as 0
            (it == newValues->begin()) ? firstNaPos = 0 : firstNaPos = i - 1;
            }

        else if (!std::isnan(*it) && lastNaPos < 0 && firstNaPos > -1) {
            // if it is not a NaN, but we found a NaN
            (it == newValues->end()) ? lastNaPos = i : lastNaPos = i + 1;
            }

        if (firstNaPos > -1 && lastNaPos > -1){
            // and now that we have both corners we do linear interpolation
            // check boundaries, and if outside date size put 0
            (firstNaPos == 0) ? first = 0: first = (*newValues)[firstNaPos];
            (lastNaPos == newValues->length()) ? last = 0: last = (*newValues)[lastNaPos];

            double diff = last - first;
            double stepwiseDelta = diff / (lastNaPos - firstNaPos + 1);

            int iter = 1;
            // and fill NaN
            for (int j = firstNaPos; j < lastNaPos; j++) {
                (*newValues)[j] = first + iter * stepwiseDelta;
                iter++;
            }

            // reset values of positions to detect more NaN
            firstNaPos = -1;
            lastNaPos = -1;
        }

        i++;
    }

    double maxXCurrent = *std::max_element(newTimestamps->constBegin(),
                                    newTimestamps->constEnd());

    qDebug() << maxXCurrent;

    if (maxXCurrent > maxXRange)
        maxXRange = maxXCurrent;

    double minXCurrent = *std::min_element(newTimestamps->constBegin(),
                                           newTimestamps->constEnd());

    qDebug() << minXCurrent;

    if (minXCurrent < minXRange)
        minXRange = minXCurrent;

}

//######################################################################################################################
//                                        END OF CLEANING DATA ROUTINES
//######################################################################################################################


//######################################################################################################################
//                                               PLOTTING ROUTINES
//######################################################################################################################


void MainWindow::SMAplot(QVector<double> &x, QVector<double> &y, const QString &xlabel,
                         const QString &ylabel, const QString &label, QCustomPlot *plotObject,
                         int graphNumber) {

    auto tempX = new QVector<double>;
    auto tempY = new QVector<double>;

    SMA(x, y, tempX, tempY);

    plot(*tempX, *tempY, xlabel, ylabel, label, plotObject, graphNumber);

    delete  tempX;
    delete tempY;
}

void MainWindow::plotPortfolioUSD() {

    SMAplot(dataHashTable["PortfolioUSD"]["x"], dataHashTable["PortfolioUSD"]["y"], "Time",
            "Portfolio value", "USD", ui->plotPortfolioValue, 0);
}

void MainWindow::plotPortfolioCoin() {

    int i= 0;

    for (auto coinName: CoinNames) {

        QString name = QString("PortfolioCoin_%1").arg(coinName);

        SMAplot(dataHashTable[name]["x"], dataHashTable[name]["y"], "Time",
                "Portfolio value", coinName, ui->plotPortfolioCoins, i);

        i++;
    }
}

void MainWindow::plotOrders() {

    int i = 0;

    for (const auto &orderName: OrderNames) {

        auto priceName = QString("price_%1").arg(orderName);
        auto speedName = QString("speed_%1").arg(orderName);

        qDebug() << priceName << speedName;

        SMAplot(dataHashTable[priceName]["x"], dataHashTable[priceName]["y"], "Time",
                "Mining Speed", orderName, ui->plotSpeed, i);

        SMAplot(dataHashTable[speedName]["x"], dataHashTable[speedName]["y"], "Time",
                "Mining Power Price", orderName, ui->plotPrice, i);

        i++;
    }
}


void MainWindow::createPlots() {

    plotPortfolioUSD();
    plotPortfolioCoin();
    plotOrders();

    addPadding();

}

void MainWindow::plot(QVector<double> &x, QVector<double> &y, const QString &xlabel, const QString &ylabel,
                      const QString &label, QCustomPlot *plotObject, int graphNumber) {

    plotObject->addGraph();

    plotObject->graph(graphNumber)->selectionDecorator()->setPen(QPen(Qt::green));

//    plotObject->graph(graphNumber)->setScatterStyle(QCPScatterStyle::ssCircle);
    plotObject->graph(graphNumber)->setLineStyle(QCPGraph::LineStyle::lsLine);
    plotObject->graph(graphNumber)->setPen(QPen(COLORS[graphNumber]));

    plotObject->legend->setVisible(true);

    plotObject->graph(graphNumber)->setData(x, y);
    plotObject->graph(graphNumber)->setName(label);

    plotObject->xAxis->setLabel(xlabel);
    plotObject->yAxis->setLabel(ylabel);
    plotObject->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    plotObject->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd/MM/yy\nhh:mm:ss");
    plotObject->xAxis->setTicker(dateTicker);
//    plotObject->xAxis->setTickLabelRotation(60);

    plotObject->rescaleAxes();
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

void MainWindow::addPadding() {
    ui->plotPortfolioCoins->yAxis->setRange(0, ui->plotPortfolioCoins->yAxis->range().upper * 1.4);
    ui->plotPortfolioCoins->replot();

    ui->plotPortfolioValue->yAxis->setRange(0, ui->plotPortfolioValue->yAxis->range().upper * 1.4);
    ui->plotPortfolioValue->replot();

    ui->plotPrice->yAxis->setRange(0, ui->plotPrice->yAxis->range().upper * 1.4);
    ui->plotPrice->replot();

    ui->plotSpeed->yAxis->setRange(0, ui->plotSpeed->yAxis->range().upper * 1.4);
    ui->plotSpeed->replot();
}

//######################################################################################################################
//                                            END OF PLOTTING ROUTINES
//######################################################################################################################

//######################################################################################################################
//                                            EVENT HANDLING ROUTINES
//######################################################################################################################

void MainWindow::setFixedWindow() {

    ui->plotPortfolioCoins->xAxis->setRange(minXRange, maxXRange);
    ui->plotPortfolioCoins->replot();

    ui->plotPortfolioValue->xAxis->setRange(minXRange, maxXRange);
    ui->plotPortfolioValue->replot();

    ui->plotPrice->xAxis->setRange(minXRange, maxXRange);
    ui->plotPrice->replot();

    ui->plotSpeed->xAxis->setRange(minXRange, maxXRange);
    ui->plotSpeed->replot();

}

void MainWindow::on_SMAValue_valueChanged(int SMAValue_)
{
    if (dataLoaded) {

        clearPlots();
        SMAValue = SMAValue_;
        createPlots();

    }
}

void MainWindow::on_SMAValue_editingFinished()
{
    on_SMAValue_valueChanged(this->ui->SMAValue->text().toInt());
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

    parseDirectory();

    dataLoaded = true;

    SMAValue = DEFAULT_SMA;

    createPlots();

    createTimeBoxes();

    setFixedWindow();
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

//######################################################################################################################
//                                          END OF EVENT HANDLING ROUTINES
//######################################################################################################################