#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void plot(QVector<double> x, QVector<double> y, QString xlabel,
              QString ylabel, QString label, QCustomPlot *plotObject,
              int graphNumber, double xminRange, double xmaxRange,
              double yminRange, double ymaxRange);

    // CLEANING DATA TOOLS
    void SMA(QVector<double> x, QVector<double> y, QString xlabel,
             QString ylabel, QString label, QCustomPlot *plotObject,
             int graphNumber, double xminRange, double xmaxRange,
             double yminRange, double ymaxRange);

private slots:
    void on_loadDataButton_clicked();

private:

    Ui::MainWindow *ui;
    QHash<QString, QHash<QString, QVector<double>>> dataHashTable;

};

#endif // MAINWINDOW_H
