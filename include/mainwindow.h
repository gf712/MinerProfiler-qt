#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void plot();

private slots:
    void on_loadDataButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<double> qv_time, qv_target;

};

#endif // MAINWINDOW_H
