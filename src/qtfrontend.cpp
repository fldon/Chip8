#include "qtfrontend.h"
#include "ui_qtfrontend.h"

QTFrontend::QTFrontend(EmuManager &nManager, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::QTFrontend), mEmumanager(nManager)
{
    qttosdlkeys.insert(std::make_pair(Qt::Key_1, SDLK_1));
    qttosdlkeys.insert(std::make_pair(Qt::Key_2, SDLK_2));
    qttosdlkeys.insert(std::make_pair(Qt::Key_3, SDLK_3));
    qttosdlkeys.insert(std::make_pair(Qt::Key_Q, SDLK_q));
    qttosdlkeys.insert(std::make_pair(Qt::Key_W, SDLK_w));
    qttosdlkeys.insert(std::make_pair(Qt::Key_E, SDLK_e));
    qttosdlkeys.insert(std::make_pair(Qt::Key_A, SDLK_a));
    qttosdlkeys.insert(std::make_pair(Qt::Key_S, SDLK_s));
    qttosdlkeys.insert(std::make_pair(Qt::Key_D, SDLK_d));
    qttosdlkeys.insert(std::make_pair(Qt::Key_X, SDLK_x));
    qttosdlkeys.insert(std::make_pair(Qt::Key_4, SDLK_4));
    qttosdlkeys.insert(std::make_pair(Qt::Key_R, SDLK_r));
    qttosdlkeys.insert(std::make_pair(Qt::Key_F, SDLK_f));
    qttosdlkeys.insert(std::make_pair(Qt::Key_Y, SDLK_y));
    qttosdlkeys.insert(std::make_pair(Qt::Key_C, SDLK_c));
    qttosdlkeys.insert(std::make_pair(Qt::Key_V, SDLK_v));
    qttosdlkeys.insert(std::make_pair(Qt::Key_B, SDLK_b));


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

void QTFrontend::keyPressEvent(QKeyEvent *event)
{
    //TODO: map qt keypresses to SDL keys or to emu keys and give them to emumanager and then to emulator
    mEmumanager.SendSDLKeyPressedEvent(qttosdlkeys.at(static_cast<Qt::Key>(event->key())));
}

void QTFrontend::keyReleaseEvent(QKeyEvent *event)
{
    mEmumanager.SendSDLKeyReleasedEvent(qttosdlkeys.at(static_cast<Qt::Key>(event->key())));
}
