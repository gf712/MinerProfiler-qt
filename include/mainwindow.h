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

    void parseDirectory();
    void parsePortfolioUSD(QString fileName);
    void plot(QVector<double> x, QVector<double> y, QString xlabel, QString ylabel, QString label, QCustomPlot *plotObject, int graphNumber);

private slots:
    void on_loadDataButton_clicked();

private:
    Ui::MainWindow *ui;

//    QVector<double> qv_time, qv_target;

    QHash<QString, QHash<QString, QVector<double>>> dataHashTable;

};

#endif // MAINWINDOW_H
