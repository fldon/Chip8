#ifndef QTFRONTEND_H
#define QTFRONTEND_H
#include "emumanager.h"
#include <qt6/QtWidgets/QtWidgets>
#include <QMainWindow>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class QTFrontend;
}
QT_END_NAMESPACE

class QTFrontend :public QMainWindow
{
    Q_OBJECT

public:

    explicit QTFrontend(EmuManager &nManager, QWidget *parent = nullptr);
    void closeEvent (QCloseEvent *event) override;
    ~QTFrontend() override;
    int run();

private slots:
    void on_actionOpen_triggered();

    void on_actionStop_Emulator_triggered();

    void on_actionLegacy_C8_triggered();

    void on_actionSCHIP_triggered();

    void on_actionStart_Debugging_triggered();

    void on_DebugStepBtn_clicked();

private:
    void UpdateEmu();
    void StopEmu();
    void FillDebugRegisterTbl();
    void ClearDebugRegisterTbl();

    Ui::QTFrontend *ui;
    EmuManager &mEmumanager;
    std::unique_ptr<QTimer> timer;

    bool debugMode{false};
};

#endif // QTFRONTEND_H
