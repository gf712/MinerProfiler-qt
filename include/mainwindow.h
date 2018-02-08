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

    // Window functions
    void createSpinBoxes();

    // FILE PARSERS
    void parseDirectory(int SMAValue);
    void parsePortfolioUSD(QString fileName, int SMAValue);
    void parsePortfolioCoin(QString fileName, int SMAValue);
    void parseSpeed(QString fileName, QString orderName, int orderNumber, int SMAValue);
    void parsePrice(QString fileName, QString orderName, int orderNumber, int SMAValue);

    // PARSER HELPERS
    QDateTime parseDate(QString element);
    QString parseValue(QString element, QString *denomination);

    // PLOTTING FUNCTIONS
    void plot(QVector<double> x, QVector<double> y, QString xlabel,
              QString ylabel, QString label, QCustomPlot *plotObject,
              int graphNumber, double xminRange, double xmaxRange,
              double yminRange, double ymaxRange);
    void clearPlots();
    void clearPlot(QCustomPlot *plotObject, int count);

    // CLEANING DATA TOOLS
    void SMA(QVector<double> x, QVector<double> y, QString xlabel,
             QString ylabel, QString label, QCustomPlot *plotObject,
             int graphNumber, double xminRange, double xmaxRange,
             double yminRange, double ymaxRange, int SMAValue);

    // GETTER
    QHash<QString, QHash<QString, QVector<double>>> get_dataHashTable() {return dataHashTable;}

private slots:
    void on_loadDataButton_clicked();
    void on_SMAValue_valueChanged(int arg1);

private:

    Ui::MainWindow *ui;
    QHash<QString, QHash<QString, QVector<double>>> dataHashTable;
    QString currentDirectory;

};

#endif // MAINWINDOW_H
