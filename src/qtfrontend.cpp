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
    ui->actionStop_Emulator->setEnabled(false);
}

QTFrontend::~QTFrontend()
{
    StopEmu();
    delete ui;
}

void QTFrontend::UpdateEmu()
{
    mEmumanager.ExecuteEmulatorStep();
}

void QTFrontend::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select File to load"), QDir::homePath());
    if ( fileName.isNull() == false )
    {
        mEmumanager.StartEmulator(fileName.toStdString());

        static int ExecuteInterval = 16; //test
        timer->start(ExecuteInterval);
        ui->actionOpen->setEnabled(false);
        ui->actionStop_Emulator->setEnabled(true);
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

