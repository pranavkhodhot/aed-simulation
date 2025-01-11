#include "AED.h"

AED::AED()
    : state(AED::HeartRhythm::Asystole),
        batteryLevel(100),
      power(false),
      padType(Adult),
      padsAttached(false)
{
    qRegisterMetaType<AED::AEDState>();
    qRegisterMetaType<AED::HeartRhythm>();
    aedThread= new QThread();
    moveToThread(aedThread);
    connect(aedThread, &QThread::started, this, &AED::aedLoop);
    aedThread->start();
    qInfo() << "AED Initlialized";

}


AED::~AED()
{
    aedThread->wait();
    aedThread->quit();

    delete aedThread;
}

bool AED::getPowerState(){return power;}
bool AED::getPadsAttached(){return padsAttached;}
AED::ElectrodePadType AED::getPadType() {return padType;}

void AED::setState(AED::HeartRhythm r)
{
    state = r;
}

AED::HeartRhythm AED::getState()
{
    return state;
}

bool AED::showECGResults(HeartRhythm hr){
    switch (hr) {
            case PEA:
                emit sendImagePath(":/img/PEA.png");
                return false;
            case Asystole:
                emit sendImagePath(":/img/Asystole.png");
                return false;
            case VT:
                emit sendImagePath(":/img/VT.png");
                return true;
            case VF:
                emit sendImagePath(":/img/VF.png");
                return true;
            default:
                qInfo() << "Invalid Heart Rhythm\n";
                return false;
        }
}

void AED::attachPads(ElectrodePadType type){
    padType = type;
    padsAttached = true;
}

void AED::switchPower(){
    power = !power;
}

void AED::sendBatteryLevel(int p){
    emit currBatteryLevel(p);
}

void AED::setDisplayText(QString s){
    emit displayText(s);
}

void AED::sendLogText(QString s){
    emit logText(s);
}

void AED::aedLoop(){
    while(true){
        while(power){
            if(batteryLevel == 0){
                power = false;
                emit batteryDead();
            }
            sendBatteryLevel(batteryLevel--);
            QThread::msleep(1000);
        }
    }
}

void AED::changeBattery(){
    if(!power){
        batteryLevel = 100;
        sendBatteryLevel(batteryLevel);
        qInfo() << "[USER] Opens back of device, takes out old batteries, inserts fresh new ones, closes back of the lid.";
    } else {
        qInfo() << "[SIM] Cannot change battery while AED is powered on";
    }
}

bool AED::selfTestCheck()
{
    if (!batteryLevel)
    {
        return false;
    }

    if (batteryLevel <= MIN_BATTERY)
    {
        qInfo() << "[AED] Battery Low!";
        return false;
    }

    qInfo() << "Testing Electrodes...";
    qInfo() << "Testing ECG Signal...";
    qInfo() << "Testing Electronics and Power...";
    qInfo() << "Validating Firmware...";
    qInfo() << "Testing Diagnostic Tools...";
    qInfo() << "Testing A/V I/O...";
    qInfo() << "Ready for use!";

    return true;
}
