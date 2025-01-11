#ifndef SIMULATIONRUNNER_H
#define SIMULATIONRUNNER_H

#define _DELAY  1000

#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QString>
#include <QRandomGenerator>
#include <QComboBox>
#include <QCheckBox>

#include "AED.h"

using namespace std;

class SimulationRunner : public QObject
{
    Q_OBJECT

public:

    SimulationRunner(AED* aed);
    ~SimulationRunner();
    void simLoop();
    void setHeartRhythm(AED::HeartRhythm hr);

signals:
    void eventHandled(AED::AEDState);

public slots:
    void setState(AED::AEDState s);

    void handlePowerOff();
    void handlePowerOn();
    void handleElectrodes(QString s,bool b);
    void handleHeartRythm(AED::HeartRhythm);
    void handleShock();
    void handleCPR();
    void handleMiscEval();

private:
    AED::AEDState state;
    AED* simAED;
    AED::HeartRhythm heartRhythm;
    QRandomGenerator* rand;

};

#endif
