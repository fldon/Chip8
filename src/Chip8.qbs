import qbs

QtApplication {
    consoleApplication: true
    install: true
    files: [
        "IC8Emulator.h",
        "c8emudisplay.cpp",
        "c8emudisplay.h",
        "c8timer.cpp",
        "c8timer.h",
        "cc8emulator.cpp",
        "cc8emulator.h",
        "display.th",
        "emumanager.cpp",
        "emumanager.h",
        "main.cpp",
        "qtfrontend.cpp",
        "qtfrontend.h",
        "qtfrontend.ui",
        "sc8emulator.cpp",
        "sc8emulator.h",
    ]
    Group {
            name: "Runtime resources"
            files: "../res/Beep.wav"
            qbs.install: true
            qbs.installDir: "res"
        }
    cpp.dynamicLibraries: ["SDL2", "SDL2_mixer"]

    Depends { name: "Qt.widgets" }
}
