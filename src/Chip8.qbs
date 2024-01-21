import qbs

CppApplication {
    consoleApplication: true
    install: true
    files: [
        "IC8Emulator.h",
        "c8timer.cpp",
        "c8timer.h",
        "cc8emulator.cpp",
        "cc8emulator.h",
        "display.th",
        "emumanager.cpp",
        "emumanager.h",
        "main.cpp",
    ]
    Group {
            name: "Runtime resources"
            files: "../res/Beep.wav"
            qbs.install: true
            qbs.installDir: "res"
        }
    cpp.dynamicLibraries: ["SDL2", "SDL2_mixer"]
}
