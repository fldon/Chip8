#ifndef IC8EMULATOR_H
#define IC8EMULATOR_H

class IC8Emulator{
public:
    virtual void StartProcessing() = 0;
    virtual void StopProcessing() = 0;
    virtual void mainloop() = 0;
    virtual ~IC8Emulator(){};
};

#endif // IC8EMULATOR_H
