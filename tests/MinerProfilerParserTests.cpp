//
// Created by gil on 06/02/18.
//

#include <QTest>
#include "mainwindow.h"

class TestParsers: public QObject {

Q_OBJECT

private slots:

    void initTestCase() {
        mainWindowObject = new MainWindow;
    }
    void DirectoryTest();
    void PortfolioValueTest();
    void PortfolioCoinsTest();
    void PortfolioSpeedTest();
    void PortfolioPriceTest();

public:
    MainWindow *mainWindowObject;

};

void TestParsers::PortfolioValueTest() {

    mainWindowObject->parsePortfolioUSD("data/portfolio_value_USD.txt");

    QCOMPARE(mainWindowObject->get_dataHashTable()["PortfolioUSD"]["x"][0], 1517926817.0);

}

void TestParsers::PortfolioCoinsTest() {

    mainWindowObject->parsePortfolioCoin("data/portfolio_value.txt");

    QCOMPARE(mainWindowObject->get_dataHashTable()["PortfolioCoin_BTC"]["y"][0], 0.2303);
    QCOMPARE(mainWindowObject->get_dataHashTable()["PortfolioCoin_ETH"]["y"][0], 0.7101);

}

void TestParsers::PortfolioSpeedTest() {

    mainWindowObject->parseSpeed("data/speed_log_us-order1.txt", "us-order1", 0);

    QCOMPARE(mainWindowObject->get_dataHashTable()["speed_us-order1"]["y"][0], 93.21303);

}

void TestParsers::PortfolioPriceTest() {

    mainWindowObject->parsePrice("data/price_log_us-order1.txt", "us-order1", 0);

    QCOMPARE(mainWindowObject->get_dataHashTable()["price_us-order1"]["y"][0], 0.00870);

}

void TestParsers::DirectoryTest() {
    mainWindowObject->set_currentDirectory("data/");
    mainWindowObject->parseDirectory();
    QCOMPARE(mainWindowObject->get_dataHashTable()["speed_us-order1"]["y"][0], 93.21303);
}

QTEST_MAIN(TestParsers);
#include "MinerProfilerParserTests.moc"