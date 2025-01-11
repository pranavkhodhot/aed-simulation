#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QDebug>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

#include "AED.h"
#include "SimulationRunner.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void runSimRunner();
    void updateState(AED::AEDState s);
    void electrodesPlaced(QString s,bool b);
    void sendHeartState(AED::HeartRhythm h);

private:
    Ui::MainWindow *ui;
    AED* aed;
    SimulationRunner* simRunner;

    QPushButton* glowingButton;

private slots:
    void enableButtons();
    void setAsystoleScenerio();
    void setPEAScenerio();
    void setVTScenerio();
    void setVFScenerio();
    void setClearScenerio();
    void updateBatteryDisplay(int p);
    void updateLCDDisplay(QString s);
    void pressPowerButton();
    void handleChangeBattery();
    void setScenario(const QString &imagePath);
    void setScenario(const QString &imagePath, QPushButton *button);
    void handleElectrodeCheckboxChange(int state);
    void updateLogBox(QString s);
    void handlePadType(QString s);
    void deliverShock();
    void setCompressionDepth(int i);
    void runSim();
    void handleBatteryDead();
    void handleEvent(AED::AEDState s);
    void sendElectrodeState();

    void on_pushButton_released();
};

#endif // MAINWINDOW_H
