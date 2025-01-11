#ifndef AED_H
#define AED_H

#define MIN_BATTERY 20

#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QString>
#include <QPushButton>

using namespace std;

class AED : public QObject
{
    Q_OBJECT

public:

    enum AEDState {
        PowerOffState,
        PowerOnState,
        ElectrodePlacementState,
        HeartRhythmAnalysisState,
        ShockDeliveryState,
        CPRState,
        ContinuedEvaluationState
    };
    Q_ENUM(AEDState)

    enum ElectrodePadType{
        Adult,
        Child
    };
    Q_ENUM(ElectrodePadType)

    enum HeartRhythm{
        PEA,
        Asystole,
        VF,
        VT
    };
    Q_ENUM(HeartRhythm)

    AED();
    ~AED();
    void aedLoop();
    void switchPower();
    void sendBatteryLevel(int p);
    void setState(HeartRhythm);
    void setDisplayText(QString s);
    void sendLogText(QString s);

    bool getPowerState();
    bool getPadsAttached();
    HeartRhythm getState();
    ElectrodePadType getPadType();
    void changeBattery();
    bool selfTestCheck();

    void attachPads(ElectrodePadType type);
    bool showECGResults(HeartRhythm hr);

private:
    QThread* aedThread;
    HeartRhythm state;
    int batteryLevel;
    bool power;
    ElectrodePadType padType;
    bool padsAttached;

signals:
    void currBatteryLevel(int p);
    void displayText(QString s);
    void logText(QString s);
    void batteryDead();

    void showPEA();
    void showAsystole();
    void showVF();
    void showVT();

    void sendImagePath(QString s);
};

Q_DECLARE_METATYPE(AED::AEDState)
Q_DECLARE_METATYPE(AED::HeartRhythm)
#endif
