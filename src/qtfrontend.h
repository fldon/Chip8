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
    ~QTFrontend() override;
    int run();

private slots:
    void on_actionOpen_triggered();

private:
    void UpdateEmu();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    std::map<Qt::Key, SDL_Keycode> qttosdlkeys;

    Ui::QTFrontend *ui;
    EmuManager &mEmumanager;
    QTimer *timer;
};

#endif // QTFRONTEND_H
