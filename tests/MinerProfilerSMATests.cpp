//
// Created by gil on 13/02/18.
//

#include <QTest>
#include "mainwindow.h"

class TestSMA: public QObject {

Q_OBJECT

private slots:

    void initTestCase() {
        mainWindowObject = new MainWindow;
        mainWindowObject->set_SMA(30);
        mainWindowObject->set_currentDirectory("data/");
        mainWindowObject->parseDirectory();
    }
//    void TestSMA::Test() {
//        QCOMPARE(true, true);
//    }
    void PortfolioSMATest();

public:
    MainWindow* mainWindowObject;
    int SMAValue;
};

void TestSMA::PortfolioSMATest() {

    QVector<double> x = mainWindowObject->get_dataHashTable()["PortfolioUSD"]["x"];
    QVector<double> y = mainWindowObject->get_dataHashTable()["PortfolioUSD"]["y"];

    auto tempX = new QVector<double>;
    auto tempY = new QVector<double>;

    mainWindowObject->SMA(x, y, tempX, tempY);

    QCOMPARE((*tempY)[0], 2308.3760);

    qDebug() << tempY->length();
    qDebug() << tempX->length();
    QCOMPARE(tempY->last(), 2333.4138666666668);

    delete tempX;
    delete tempY;
}

QTEST_MAIN(TestSMA);
#include "MinerProfilerSMATests.moc"