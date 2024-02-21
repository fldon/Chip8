#ifndef SDLWIDGET_H
#define SDLWIDGET_H


#include <SDL2/SDL.h>
#include <QWidget>
#include <iostream>
#include "ISDLInputReceiver.h"
#include <qt6/QtWidgets/QtWidgets>


class SDLWidget : public QWidget {
    Q_OBJECT

public:
    SDLWidget(ISDLINPUTRECEIVER *nInputReceiver, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags(0))
        : QWidget(parent, f), mInputReceiver(nInputReceiver)
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

        setAttribute(Qt::WA_PaintOnScreen);
        setUpdatesEnabled(false);

        //TODO: Set window size in constructor and make it fixed

        //SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

        // initialize default Video
        if((SDL_Init(SDL_INIT_VIDEO) == -1)) {
            std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        }


        void *NativeQtWindow = nullptr;
        NativeQtWindow = (void*) (winId());

        //mScreen = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);
        mWindow = SDL_CreateWindowFrom(NativeQtWindow);
        mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED );
        //mScreen = SDL_GetWindowSurface(mWindow);

        if(mRenderer == nullptr)
        {
            std::cerr <<  "Renderer could not be created! SDL Error: %s\n", SDL_GetError();
        }
        show();
    }

    virtual ~SDLWidget() {
        SDL_DestroyRenderer( mRenderer );
        SDL_DestroyWindow( mWindow );
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        mInputReceiver->SendSDLKeyPressedEvent(qttosdlkeys.at(static_cast<Qt::Key>(event->key())));
    }
    void keyReleaseEvent(QKeyEvent *event) override
    {
        mInputReceiver->SendSDLKeyReleasedEvent(qttosdlkeys.at(static_cast<Qt::Key>(event->key())));
    }

    void closeEvent (QCloseEvent *event) override
    {
        event->ignore();
    }

    SDL_Window* mWindow = nullptr;
    SDL_Renderer *mRenderer = nullptr;
private:
    ISDLINPUTRECEIVER *mInputReceiver = nullptr;
    std::map<Qt::Key, SDL_Keycode> qttosdlkeys;
};

#endif // SDLWIDGET_H
