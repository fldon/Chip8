#include "qtfrontend.h"
#include "ui_qtfrontend.h"

QTFrontend::QTFrontend(EmuManager &nManager, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::QTFrontend), mEmumanager(nManager)
{


    ui->setupUi(this);
    show();
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, QOverload<>::of(&QTFrontend::UpdateEmu));
    setAttribute(Qt::WA_QuitOnClose);

    //Set default Enabled states
    ui->actionStop_Emulator->setEnabled(false);
    ui->DebugStepBtn->setEnabled(false);
}

QTFrontend::~QTFrontend()
{
    StopEmu();
    delete ui;
}

void QTFrontend::UpdateEmu()
{
    mEmumanager.ExecuteEmulatorStep();
    FillDebugRegisterTbl();
}

void QTFrontend::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select File to load"), QDir::homePath());
    if ( fileName.isNull() == false )
    {
        mEmumanager.StartEmulator(fileName.toStdString());
        ui->actionOpen->setEnabled(false);
        ui->actionStop_Emulator->setEnabled(true);

        if(!debugMode)
        {
            static int ExecuteInterval = 4; //250HZ
            timer->start(ExecuteInterval);
        }
    }
}

void QTFrontend::on_actionStop_Emulator_triggered()
{
    StopEmu();
}

void QTFrontend::StopEmu()
{
    //stop timer before stopping emulator
    timer->stop();
    mEmumanager.StopEmulator();
    ui->actionOpen->setEnabled(true);
    ui->actionStop_Emulator->setEnabled(false);
    ClearDebugRegisterTbl();
}

void QTFrontend::closeEvent (QCloseEvent *event)
{
    StopEmu();
    event->accept();
}

void QTFrontend::on_actionLegacy_C8_triggered()
{
    StopEmu();
    ui->actionSCHIP->setChecked(false);
    ui->actionSCHIP->setEnabled(true);
    ui->actionLegacy_C8->setEnabled(false);
    mEmumanager.Switchmode(EmuManager::EmuMode::LegacyC8);
}


void QTFrontend::on_actionSCHIP_triggered()
{
    StopEmu();
    ui->actionLegacy_C8->setChecked(false);
    ui->actionLegacy_C8->setEnabled(true);
    ui->actionSCHIP->setEnabled(false);
    mEmumanager.Switchmode(EmuManager::EmuMode::SCHIP);
}


void QTFrontend::on_actionStart_Debugging_triggered()
{
    StopEmu();
    if(!debugMode)
    {
        ui->DebugStepBtn->setEnabled(true);
        debugMode = true;
        ui->actionStart_Debugging->setText("Stop debugging");
    }
    else
    {
        ui->DebugStepBtn->setEnabled(false);
        debugMode = false;
        ui->actionStart_Debugging->setText("Start debugging");
        ClearDebugRegisterTbl();
    }
}


void QTFrontend::on_DebugStepBtn_clicked()
{
    //Check if debugmode is active. If not, do nothing (Button press shouldn't even be possible then)
    if(!debugMode)
        throw std::runtime_error("Debug button pressed outside of debug mode! This should never be possible!");

    UpdateEmu();
}

void QTFrontend::FillDebugRegisterTbl()
{
    //get all register values and print them in main window table
    std::vector<uint8_t> regvec = mEmumanager.GetCurrentRegisters();
    //Fill variable size Register table with regvec
    ClearDebugRegisterTbl();
    for(std::size_t i = 0; i < regvec.size(); ++i)
    {
        //ui->RegisterTbl->insertRow(i);
        //setData(createIndex(i, 0), QVariant::fromValue<int>(regvec.at(i)), Qt::EditRole);
        //ui->RegisterTbl->setItem(0, i, regvec.at(i));
        QTableWidgetItem *theItem = new QTableWidgetItem();
        theItem->setData(Qt::EditRole, regvec.at(i));
        ui->RegisterTbl->setItem(i+2, 0, theItem);
    }

    //Also fill table with Ireg and PC value
    QTableWidgetItem *PCItem = new QTableWidgetItem();
    PCItem->setData(Qt::EditRole, mEmumanager.GetCurrentPC());
    QTableWidgetItem *IRegItem = new QTableWidgetItem();
    IRegItem->setData(Qt::EditRole, mEmumanager.GetCurrentIReg());

    ui->RegisterTbl->setItem(0,0, PCItem);
    ui->RegisterTbl->setItem(1,0, IRegItem);
}

void QTFrontend::ClearDebugRegisterTbl()
{
    ui->RegisterTbl->clearContents();
}
