#include "qtfrontend.h"
#include "ui_qtfrontend.h"

QTFrontend::QTFrontend(EmuManager &nManager, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::QTFrontend), mEmumanager(nManager)
{


    ui->setupUi(this);
    show();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QTFrontend::UpdateEmu));
}

QTFrontend::~QTFrontend()
{
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
    static int ExecuteInterval = 1; //test
    timer->start(ExecuteInterval);
}
