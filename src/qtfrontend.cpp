#include "qtfrontend.h"
#include "ui_qtfrontend.h"

QTFrontend::QTFrontend(EmuManager &nManager, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::QTFrontend), mEmumanager(nManager)
{


    ui->setupUi(this);
    show();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QTFrontend::UpdateEmu));
    setAttribute(Qt::WA_QuitOnClose);
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
    }
    static int ExecuteInterval = 16; //test
    timer->start(ExecuteInterval);
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
}

void QTFrontend::closeEvent (QCloseEvent *event)
{
    StopEmu();
    event->accept();
}
