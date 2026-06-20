#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startSimulation();
    void restartApplication();

private:
    QComboBox *waterTypeBox;

    QLineEdit *timeLimitEdit;
    QLineEdit *oilVolumeEdit;

    QPushButton *startButton;
    QPushButton *restartButton;
    QPushButton *closeButton;  
};

#endif // MAINWINDOW_H
