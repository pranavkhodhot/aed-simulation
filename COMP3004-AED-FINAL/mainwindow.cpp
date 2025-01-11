#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , aed(new AED)
    , simRunner(new SimulationRunner(aed))
    , glowingButton(nullptr)
{

    ui->setupUi(this);

    QPixmap image(":/img/Blank.png");
    ui->ECG->setPixmap(image.scaled(311,71,Qt::KeepAspectRatio));

    connect(ui->powerButton, SIGNAL(released()), this, SLOT (pressPowerButton()));
    connect(ui->changeBatteries, SIGNAL(released()), this, SLOT (handleChangeBattery()));

    connect(ui->autoSim, SIGNAL(stateChanged(int)), this, SLOT(enableButtons()));
    connect(ui->runSim, SIGNAL(released()), this, SLOT(runSim()));

    connect(ui->resetSimButton, SIGNAL(released()), this, SLOT (setClearScenerio()));
    connect(ui->peaButton, SIGNAL(released()), this, SLOT (setPEAScenerio()));
    connect(ui->asystoleButton, SIGNAL(released()), this, SLOT (setAsystoleScenerio()));
    connect(ui->vtButton, SIGNAL(released()), this, SLOT (setVTScenerio()));
    connect(ui->vfButton, SIGNAL(released()), this, SLOT (setVFScenerio()));

    connect(ui->electrodeCheckbox, SIGNAL(stateChanged(int)), this, SLOT(handleElectrodeCheckboxChange(int)));
    connect(ui->padSelector, SIGNAL(currentTextChanged(QString)), this, SLOT (handlePadType(QString)));
    connect(ui->deliverShock, SIGNAL(released()), this, SLOT (deliverShock()));
    connect(ui->compressionDepth, SIGNAL(valueChanged(int)), this, SLOT(setCompressionDepth(int)));

    connect(aed, &AED::currBatteryLevel, this, &MainWindow::updateBatteryDisplay);
    connect(aed, &AED::displayText, this, &MainWindow::updateLCDDisplay);
    connect(aed, &AED::logText, this, &MainWindow::updateLogBox);
    connect(aed, &AED::batteryDead, this, &MainWindow::handleBatteryDead);


    connect(aed, &AED::sendImagePath, this, [this](const QString &imagePath) {
        setScenario(imagePath);
    });

    QThread *simThread = new QThread();
    simRunner->moveToThread(simThread);

    connect(ui->runSim, SIGNAL(released()), simThread, SLOT(start()));
    connect(simThread, &QThread::started, simRunner, &SimulationRunner::handlePowerOn);
    connect(simThread, &QThread::finished, [&](){qInfo("Finished");});

    connect(this, &MainWindow::updateState, simRunner, &SimulationRunner::setState);
    connect(simRunner, SIGNAL(eventHandled(AED::AEDState)), this, SLOT(handleEvent(AED::AEDState)));

    enableButtons();
}

MainWindow::~MainWindow()
{
    delete simRunner;
    delete aed;
    delete ui;
}

// Battery and Power Functions

void MainWindow::pressPowerButton() {

    // if AED is off, perform the self test
    if(!aed->getPowerState())
    {
        // disable button while test is happening
        ui->powerButton->setEnabled(false);

        if(!aed->selfTestCheck()){
            qInfo() << "[SIM] Batterry is too low. Channge batteries please. AED turning off.";
            ui->powerButton->setEnabled(true);
            return;
        }
        ui->lcdDisplay->setText("UNIT OK");
        ui->powerButton->setEnabled(true);
    }

    // if the test passes or AED was on, switch state
    aed->switchPower();

    if(aed->getPowerState()){
        qInfo() << "[UI] Power On";
        ui->lcdDisplay->setText("POWER ON");
        ui->powerButton->setText("Power Off");

        emit updateState(AED::AEDState::PowerOnState);
    }
    else{
        qInfo() << "[UI] Power Off";
        ui->lcdDisplay->clear();
        ui->powerButton->setText("Power On");

        emit updateState(AED::AEDState::PowerOffState);
    }

    // update buttons accordingly
    enableButtons();
}

void MainWindow::handleChangeBattery(){
    qInfo() << "[UI] Charge and Reset";
    aed->changeBattery();
}

void MainWindow::updateBatteryDisplay(int p){
    qInfo() << "[UI] Update Battery Display to " << p;
    ui->batteryLevel->setValue(p);
}

// Scenario Functions

void MainWindow::runSim()
{
    qInfo() << "[UI] Run Sim";
}

void MainWindow::setScenario(const QString &imagePath)
{
    setScenario(imagePath, nullptr);
}

void MainWindow::setScenario(const QString &imagePath, QPushButton *button) {
    QPixmap image(imagePath);
    ui->ECG->setPixmap(image.scaled(311, 71, Qt::KeepAspectRatio));

    if (glowingButton != nullptr && glowingButton != button) {
        glowingButton->setStyleSheet("");
    }
    if (button) {
        QString styleSheet = "QPushButton {"
                             "background-color: #88ff88;"
                             "border: 2px solid #00aa00;"
                             "border-radius: 5px;"
                             "padding: 5px;"
                             "}";
        button->setStyleSheet(styleSheet);
    }
    glowingButton = button;

    connect(this, SIGNAL(sendHeartState(AED::HeartRhythm)), simRunner, SLOT(handleHeartRythm(AED::HeartRhythm)));
    emit sendHeartState(aed->getState());
}

void MainWindow::setAsystoleScenerio(){
    qInfo() << "[UI] Asystole Scenario";
    aed->setState(AED::HeartRhythm::Asystole);
    setScenario(":/img/Asystole.png", ui->asystoleButton);
}

void MainWindow::setPEAScenerio(){
    qInfo() << "[UI] PEA Scenario";
    aed->setState(AED::HeartRhythm::PEA);
    setScenario(":/img/PEA.png", ui->peaButton);
}
void MainWindow::setVTScenerio(){
    qInfo() << "[UI] VT Scenario";
    aed->setState(AED::HeartRhythm::VT);
    setScenario(":/img/VT.png", ui->vtButton);
}

void MainWindow::setVFScenerio(){
    qInfo() << "[UI] VF Scenario";
    aed->setState(AED::HeartRhythm::VF);
    setScenario(":/img/VF.png", ui->vfButton);
}

void MainWindow::setClearScenerio(){
    qInfo() << "[UI] Clear Scenario";
    setScenario(":/img/Blank.png", nullptr);
}

void MainWindow::handleBatteryDead()
{
    emit updateState(AED::AEDState::PowerOffState);
    enableButtons();
}
// Update UI Functions

void MainWindow::updateLCDDisplay(QString s){
    qInfo() << "[UI] Update LCD Display";
    ui->lcdDisplay->setText(s);
}

void MainWindow::updateLogBox(QString s){
    qInfo() << "[UI] Update Log Box";
    ui->logBox->append(s);
}

void MainWindow::handleElectrodeCheckboxChange(int state) {
    qInfo() << "[UI] Checkbox " << (state == Qt::Checked ? "Enabled" : "Disabled");
    enableButtons();
}

// Updates the state of UI buttons based on AED power and electrodeCheckbox, made to enable ONLY if electrodes on & power is on.
void MainWindow::enableButtons(){

    bool isPowerOn = aed->getPowerState();
    bool isAuto = ui->autoSim->isChecked();
    bool isElectrodePlaced = ui->electrodeCheckbox->isChecked();

    // if power is on
    ui->simOptions->setEnabled(isPowerOn);
    ui->controlPanel->setEnabled(isPowerOn);
    ui->display->setEnabled(isPowerOn);
    ui->changeBatteries->setEnabled(!isPowerOn);
    ui->batteryLabel->setEnabled(isPowerOn);
    ui->batteryLevel->setEnabled(isPowerOn);

    // if auto Sim
    ui->runSim->setEnabled(isAuto);
    ui->simulatedScenarios->setEnabled(!isAuto);
}

void MainWindow::handlePadType(QString s)
{
    qInfo() << "[UI] Pad Type " << s;
}

void MainWindow::deliverShock()
{
    qInfo() << "[UI] Deliver Shock";
}

void MainWindow::setCompressionDepth(int i)
{
    qInfo() << "[UI] Compression Depth at " << i;
}

void MainWindow::handleEvent(AED::AEDState s)
{
    qInfo() << "[UI] Handle " << s;
    auto g = QRandomGenerator::global();
    int i = 0;
    switch (s) {
        case AED::AEDState::PowerOnState:
        break;

        case AED::AEDState::ElectrodePlacementState:
            connect(this, SIGNAL(electrodesPlaced(QString,bool)), simRunner, SLOT(handleElectrodes(QString,bool)));
            connect(ui->electrodeCheckbox, SIGNAL(stateChanged(int)), this, SLOT(sendElectrodeState()));
            connect(ui->padSelector, SIGNAL(currentTextChanged(QString)), this, SLOT(sendElectrodeState()));
            sendElectrodeState();
            break;

        case AED::AEDState::HeartRhythmAnalysisState:
            i = g->generate() % 4;
            switch(i)
            {
            case 0:
            setAsystoleScenerio();
            break;
            case 1:
            setPEAScenerio();
            break;
            case 2:
            setVFScenerio();
            break;
            case 3:
            setVTScenerio();
                break;
            }

            break;

        case AED::AEDState::ShockDeliveryState:
        break;

        case AED::AEDState::CPRState:
        break;

        default:
        case AED::AEDState::PowerOffState:
        break;
    }
}

void MainWindow::sendElectrodeState()
{
    emit electrodesPlaced(ui->padSelector->currentText(), ui->electrodeCheckbox->isChecked());
}

void MainWindow::on_pushButton_released()
{
    qInfo() << "CPR BUTTON";
}

