#include "SimulationRunner.h"

SimulationRunner::SimulationRunner(AED* aed)
    : state(AED::AEDState::PowerOffState)
    , simAED(aed)
    , rand(QRandomGenerator::global()) {}

SimulationRunner::~SimulationRunner() {}

void SimulationRunner::setState(AED::AEDState s){
    qInfo() << "[SIM] set sim state to " << s;
    state = s;
}

void SimulationRunner::setHeartRhythm(AED::HeartRhythm hr){
    heartRhythm = hr;
}

// sim handles
void SimulationRunner::handlePowerOn()
{
    simAED->setDisplayText("UNIT OK");
    simAED->sendLogText("[AED] UNIT OK");
    QThread::msleep(_DELAY);
    simAED->setDisplayText("STAY CALM");
    simAED->sendLogText("[AED] STAY CALM");
    QThread::msleep(_DELAY);
    simAED->setDisplayText("PLUG IN CABLE");
    simAED->sendLogText("[AED] PLUG IN ELECTRODE CABLE");

    state = AED::AEDState::ElectrodePlacementState;
    emit eventHandled(AED::AEDState::ElectrodePlacementState);
}

void SimulationRunner::handleElectrodes(QString type,bool attached)
{
    simAED->setDisplayText("ATTACH DEFIB PADS");
    simAED->sendLogText("[AED] ATTACH DEFIB PADS TO PATIENT'S BARE CHEST");
    QThread::msleep(_DELAY);
    if(attached){
        if(type == "Adult"){
            simAED->setDisplayText("ADULT");
            simAED->sendLogText("[AED] ADULT PADS DETTECTED");
        }
        else{
            simAED->setDisplayText("CHILD");
            simAED->sendLogText("[AED] CHILD PADS DETTECTED");
        }
        state = AED::AEDState::HeartRhythmAnalysisState;
        emit eventHandled(AED::AEDState::HeartRhythmAnalysisState);
    }
}

void SimulationRunner::handleHeartRythm(AED::HeartRhythm)
{
    simAED->setDisplayText("DON'T TOUCH");
    simAED->sendLogText("[AED] DONT TOUCH THE PATIENT");
    QThread::msleep(_DELAY);
    simAED->setDisplayText("STAND CLEAR");
    simAED->sendLogText("[AED] STAND CLEAR FROM PATIENT");
    QThread::msleep(_DELAY);

    for(int i=0;i<2;i++){
        simAED->setDisplayText("ANALYZING");
        QThread::msleep(1000);

        simAED->setDisplayText("ANALYZING.");
        QThread::msleep(1000);

        simAED->setDisplayText("ANALYZING..");
        QThread::msleep(1000);

        simAED->setDisplayText("ANALYZING...");
        QThread::msleep(1000);
    }
    if(simAED->showECGResults(heartRhythm)){
        simAED->setDisplayText("SHOCK ADVISED");
        simAED->sendLogText("[AED] SHOCK ADVISED");
        state = AED::AEDState::ShockDeliveryState;
        emit eventHandled(AED::AEDState::ShockDeliveryState);
    }
    else{
        simAED->setDisplayText("NO SHOCK ADVISED");
        simAED->sendLogText("[AED] NO SHOCK ADVISED");
        state = AED::AEDState::CPRState;
        emit eventHandled(AED::AEDState::CPRState);
    }

    emit
}

void SimulationRunner::handleShock()
{
    // handle shock (Use Case 5), write a function handleShock() and move all code there

    if (state != AED::AEDState::ShockDeliveryState) {
        qInfo() << "Invalid state for shock delivery!";
        return;
    }
    if (!simAED->showECGResults(heartRhythm)) {
        qInfo() << "[ERROR] Invalid heart rhythm for shock delivery";
        state = AED::AEDState::CPRState;
        emit eventHandled(AED::AEDState::CPRState);
        return;
    }

    simAED->setDisplayText("DO NOT TOUCH THE PATIENT");
    simAED->sendLogText("[AED] INSTRUCTING RESPONDER: DO NOT TOUCH THE PATIENT BEFORE SHOCK");

    QThread::msleep(_DELAY);

    for (int count = 3; count > 0; --count) {
        simAED->setDisplayText(QString::number(count));
        QThread::msleep(1000);
    }
    simAED->setDisplayText("DELIVER SHOCK NOW");
    simAED->sendLogText("[AED] DELIVER SHOCK NOW");

    QThread::msleep(_DELAY);
}

void SimulationRunner::handleCPR()
{
    simAED->setDisplayText("START CPR");
    simAED->sendLogText("[AED] START CPR");
    QThread::msleep(2000);

    simAED->setDisplayText("CHECK RESPONSIVENESS");
    simAED->sendLogText("[AED] CHECK RESPONSIVENESS");
    QThread::msleep(2000);

    simAED->setDisplayText("OPEN AIRWAY, CHECK BREATHING");
    simAED->sendLogText("[AED] OPEN AIRWAY, CHECK BREATHING");
    QThread::msleep(2000);

    simAED->setDisplayText("GIVE 2 RESCUE BREATHS");
    simAED->sendLogText("[AED] GIVE 2 RESCUE BREATHS");
    QThread::msleep(2000);

    simAED->setDisplayText("START CHEST COMPRESSIONS");
    simAED->sendLogText("[AED] START CHEST COMPRESSIONS");
    QThread::msleep(2000);

    for (int i = 0; i < 5; ++i) {
        simAED->setDisplayText("CONTINUE CPR");
        simAED->sendLogText("[AED] CONTINUE CPR");
        QThread::msleep(_DELAY);
        simAED->setDisplayText("GOOD COMPRESSIONS");
        simAED->sendLogText("[AED] GOOD COMPRESSIONS");
        QThread::msleep(2000);
    }

    // Final instruction to stop CPR and reanalyze rhythm
    simAED->setDisplayText("STOP CPR, ANALYZE RHYTHM");
    simAED->sendLogText("[AED] STOP CPR, ANALYZE RHYTHM");
    QThread::msleep(_DELAY);

    // Transition to rhythm analysis state
    state = AED::AEDState::HeartRhythmAnalysisState;
    emit eventHandled(AED::AEDState::HeartRhythmAnalysisState);
}

void SimulationRunner::handleMiscEval()
{
    qInfo() << "ContinuedEvaluationState";
    QThread::msleep(_DELAY);
}

void SimulationRunner::handlePowerOff()
{
    qInfo() << "PowerOffState";
    QThread::msleep(_DELAY);
}

// sim loop

void SimulationRunner::simLoop(){

//    switch(state){
//    case AED::AEDState::PowerOnState:

//        handlePowerOn();
//        break;

//    case AED::AEDState::ElectrodePlacementState:

//        handleElectrodes();
//        break;

//    case AED::AEDState::HeartRhythmAnalysisState:

//        handleHeartRythm();
//        break;

//    case AED::AEDState::ShockDeliveryState:
//        handleShock();
//        break;

//    case AED::AEDState::CPRState:
//        handleCPR();
//        break;

//    case AED::AEDState::ContinuedEvaluationState:
//        handleMiscEval();
//        break;

//    case AED::AEDState::PowerOffState:
//    default:
//        handlePowerOff();
//        break;
//    }
}
