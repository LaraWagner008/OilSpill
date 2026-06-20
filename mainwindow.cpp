#include "mainwindow.h"
#include "oilspillwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QProcess>

#include <QMessageBox>
#include <QFrame>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Система поддержки принятия решений");

    setFixedSize(500, 820);

    setStyleSheet(

                "QMainWindow {"
                "background-color: rgb(4,14,28,500);"
                "}"

                "QLabel {"
                "color: rgb(0,255,255);"
                "font-size: 15px;"
                "font-family: Bahnschrift;"
                "letter-spacing: 1px;"
                "}"

                "QComboBox {"
                "background-color: rgba(4,14,28,500);"
                "border: 1px solid rgb(0,255,255);"
                "border-radius: 12px;"
                "padding: 10px;"
                "color: white;"
                "font-size: 15px;"
                "font-family: Bahnschrift;"
                "}"

                "QComboBox QAbstractItemView {"
                "background-color: rgb(4,14,28,500);"
                "color: white;"
                "border: 1px solid rgb(0,255,255);"
                "selection-background-color: rgb(0,180,255);"
                "selection-color: black;"
                "outline: 0px;"
                "padding: 4px;"
                "}"

                "QLineEdit {"
                "background-color: rgba(10,20,35,230);"
                "border: 1px solid rgb(0,255,255);"
                "border-radius: 12px;"
                "padding: 10px;"
                "color: white;"
                "font-size: 15px;"
                "font-family: Bahnschrift;"
                "}"

                "QPushButton {"
                "background-color: rgba(0,180,255,70);"
                "border: 1px solid rgb(0,255,255);"
                "border-radius: 14px;"
                "padding: 14px;"
                "color: white;"
                "font-size: 15px;"
                "font-family: Bahnschrift;"
                "}"

                "QPushButton:hover {"
                "background-color: rgba(0,255,255,140);"
                "border: 2px solid rgb(0,255,255);"
                "color: rgb(255,255,255);"
                "}"

                "QPushButton:pressed {"
                "background-color: rgba(0,255,255,220);"
                "border: 2px solid rgb(255,255,255);"
                "padding-left: 16px;"
                "padding-top: 16px;"
                "}"

                );

    QWidget *central = new QWidget(this);

    setCentralWidget(central);

    central->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 rgb(4, 14, 28),"
        "        stop:0.3 rgb(6, 18, 35),"
        "        stop:0.6 rgb(5, 16, 32),"
        "        stop:1 rgb(8, 22, 45));"
        "}"
    );

    QHBoxLayout *root =
            new QHBoxLayout(central);

    root->setContentsMargins(18,18,18,18);

    // =====================================================
    // LEFT PANEL
    // =====================================================

    QFrame *leftPanel = new QFrame();

    leftPanel->setFixedWidth(450);

    leftPanel->setStyleSheet(

                "background-color: rgba(6,12,24,220);"
                "border: 1px solid rgba(0,255,255,120);"
                "border-radius: 22px;"

                );

    QVBoxLayout *left =
            new QVBoxLayout(leftPanel);

    left->setContentsMargins(28,28,28,28);

    QLabel *title =
            new QLabel(
                "ПАРАМЕТРЫ РАЗЛИВА НЕФТИ");
    title->setStyleSheet(

                "font-size: 25px;"
                "font-weight: bold;"
                "color: rgb(0,255,255);"
                "padding-bottom: 12px;"
                );

    left->addWidget(title);

    QFrame *line = new QFrame();

    line->setFixedHeight(2);

    line->setStyleSheet(
                "background-color: rgba(0,255,255,120);"
                );

    left->addWidget(line);

    left->addSpacing(25);

    // =====================================================
    // TYPE
    // =====================================================

    QLabel *waterLabel =
            new QLabel("ТИП АКВАТОРИИ");

    waterTypeBox = new QComboBox();

    waterTypeBox->addItem("Океан");
    waterTypeBox->addItem("Море");
    waterTypeBox->addItem("Река");

    // =====================================================
    // TIME
    // =====================================================

    QLabel *timeLabel =
            new QLabel("ОГРАНИЧЕНИЕ НА ВРЕМЯ (ч)");

    timeLimitEdit = new QLineEdit();

    timeLimitEdit->setPlaceholderText(
                "Например: 6");

    QRegularExpression timeRegex(
                "^\\d*(\\.\\d*)?$");

    timeLimitEdit->setValidator(
                new QRegularExpressionValidator(
                    timeRegex,
                    this));

    // =====================================================
    // OIL
    // =====================================================

    QLabel *oilLabel =
            new QLabel("ОБЪЁМ ВЫБРОСА НЕФТИ (м³)");

    oilVolumeEdit = new QLineEdit();

    oilVolumeEdit->setPlaceholderText(
                "Например: 1000");

    QRegularExpression oilRegex(
                "^\\d*(\\.\\d*)?$");

    oilVolumeEdit->setValidator(
                new QRegularExpressionValidator(
                    oilRegex,
                    this));

    // =====================================================
    // BUTTON
    // =====================================================

    startButton =
            new QPushButton(
                "ЗАПУСТИТЬ МОДЕЛЬ");

    startButton->setFixedHeight(60);

    startButton->setShortcut(Qt::Key_Return); // Enter

    restartButton =
            new QPushButton(
                "ПЕРЕЗАПУСТИТЬ");

    restartButton->setFixedHeight(60);

    closeButton =
            new QPushButton(
                "ЗАКРЫТЬ");

    closeButton->setFixedHeight(60);

    // =====================================================
    // ADD
    // =====================================================

    left->addWidget(waterLabel);
    left->addWidget(waterTypeBox);

    left->addSpacing(22);

    left->addWidget(timeLabel);
    left->addWidget(timeLimitEdit);

    left->addSpacing(22);

    left->addWidget(oilLabel);
    left->addWidget(oilVolumeEdit);

    left->addSpacing(175);

    left->addWidget(startButton);
    left->addWidget(restartButton);
    left->addWidget(closeButton);

    left->addStretch();

    // =====================================================
    // RIGHT PANEL
    // =====================================================
/*
    QFrame *rightPanel = new QFrame();

    rightPanel->setStyleSheet(

                "background-color: rgba(6,12,24,210);"
                "border: 1px solid rgba(0,255,255,80);"
                "border-radius: 22px;"

                );

    QVBoxLayout *right =
            new QVBoxLayout(rightPanel);

    right->setContentsMargins(28,28,28,28);

    QLabel *systemTitle =
            new QLabel("СТАТУС СИСТЕМЫ");

    systemTitle->setStyleSheet(

                "font-size: 26px;"
                "font-weight: bold;"
                "color: rgb(0,255,255);"

                );

    right->addWidget(systemTitle);

    right->addSpacing(25);

    QLabel *info =
            new QLabel(

                "• Моделирование дрейфа нефтяного пятна\n\n"

                "• Поддержка принятия решений\n\n"

                "• Анализ эффективности авиационных группировок\n\n"

                "• Визуализация распространения нефти\n\n"

                "• Подготовка к алгоритмам обнаружения и ликвидации\n\n"

                "• Тактический мониторинг акватории"

                );

    info->setStyleSheet(

                "font-size: 17px;"
                "color: rgb(120,220,255);"
                "line-height: 28px;"
                );

    right->addWidget(info);

    right->addStretch();
*/
    root->addWidget(leftPanel);
    //root->addWidget(rightPanel);

    connect(startButton,
            &QPushButton::clicked,
            this,
            &MainWindow::startSimulation);

    connect(closeButton,
            &QPushButton::clicked,
            qApp,
            &QApplication::quit);

    connect(restartButton,
            &QPushButton::clicked,
            this,
            &MainWindow::restartApplication);

}



void MainWindow::startSimulation()
{
    if(timeLimitEdit->text().isEmpty()
            ||
       oilVolumeEdit->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    "Ошибка",
                    "Введите все параметры");

        return;
    }

    QString waterType =
            waterTypeBox->currentText();

    double timeLimit =
            timeLimitEdit->text().toDouble();

    double oilVolume =
            oilVolumeEdit->text().toDouble();

    if(timeLimit <= 0.0)
    {
        QMessageBox::warning(
                    this,
                    "Ошибка",
                    "Ограничение на время должно быть больше нуля");

        return;
    }

    if(oilVolume <= 0.0)
    {
        QMessageBox::warning(
                    this,
                    "Ошибка",
                    "Объем разлива должен быть больше нуля");

        return;
    }

    OilSpillWindow *window =
            new OilSpillWindow(
                waterType,
                timeLimit,
                oilVolume);


    window->show();
}

void MainWindow::restartApplication()
{
    QProcess::startDetached(
                QApplication::applicationFilePath());

    QApplication::quit();
}
