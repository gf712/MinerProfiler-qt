//
// Created by gil on 06/02/18.
//

//#include "MinerProfiler_test.h"

#include <QTest>
#include <QDebug>
#include "mainwindow.h"

class TestParsers: public QObject
{
Q_OBJECT
private slots:
    void PortfolioValueTest();
};

void TestParsers::PortfolioValueTest() {

    auto mainWindowObject = new MainWindow;

    mainWindowObject->parsePortfolioUSD("tests/data/portfolio_value_test_data.txt", 1);

    QCOMPARE(mainWindowObject->get_dataHashTable()["PortfolioUSD"]["x"][0], 1517926817);

}

QTEST_MAIN(TestParsers);
#include "MinerProfiler_test.moc"