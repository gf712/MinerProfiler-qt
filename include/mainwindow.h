#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <set>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Window functions
    void createSpinBoxes();
    void createTimeBoxes();

    // FILE PARSERS
    void parseDirectory();
    void parsePortfolioUSD(QString fileName);
    void parsePortfolioCoin(QString fileName);
    void parseSpeed(QString fileName, QString orderName, int orderNumber);
    void parsePrice(QString fileName, QString orderName, int orderNumber);

    // PARSER HELPERS
    QDateTime parseDate(QString element);
    QString parseValue(QString element, QString *denomination);

    // PLOTTING FUNCTIONS
    void plotPortfolioUSD();
    void plotPortfolioCoin();
    void plotOrders();
//    void plotPrice();
    void createPlots();

    void addPadding();

    void SMAplot(QVector<double> &x, QVector<double> &y, const QString &xlabel,
                 const QString &ylabel, const QString &label, QCustomPlot *plotObject,
                 int graphNumber);

    void plot(QVector<double> &x, QVector<double> &y, const QString &xlabel,
              const QString &ylabel, const QString &label, QCustomPlot *plotObject,
              int graphNumber);

    void clearPlots();
    void clearPlot(QCustomPlot *plotObject, int count);

    // CLEANING DATA TOOLS
    void SMA(QVector<double> &x, QVector<double> &y,
             QVector<double> *newTimestamps, QVector<double> *newValues);
    void setFixedWindow();

    // GETTERS
    QHash<QString, QHash<QString, QVector<double>>> get_dataHashTable() {return dataHashTable;}

    // SETTERS
    void set_currentDirectory(QString directoryName) {currentDirectory = directoryName;}
    void set_SMA(int SMA_) {SMAValue = SMA_;}

private slots:

    void on_loadDataButton_clicked();

    void on_SMAValue_valueChanged(int SMAValue);
    void on_SMAValue_editingFinished();

    void on_startTimeEdit_editingFinished();
    void on_endTimeEdit_editingFinished();

private:

    Ui::MainWindow *ui;
    QHash<QString, QHash<QString, QVector<double>>> dataHashTable;
    QString currentDirectory;
    QVector<QString> CoinNames;
    double minXRange;
    double maxXRange;
    bool dataLoaded;
    std::set<QString> OrderNames;
    int SMAValue;
    QHash<QString, double> maxValues;

};

#endif // MAINWINDOW_H
