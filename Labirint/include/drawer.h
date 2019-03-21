#ifndef DRAWER_H
#define DRAWER_H

#include <Arduino.h>
#include <GeeGrow_ILI9341.h>

class Drawer : public GeeGrow_ILI9341{
    public:
        static Drawer& instance() {
            static Drawer instance;
            return instance;
        }

    private:
        Drawer(){}
        ~Drawer(){}
        Drawer(const Drawer&);
        Drawer& operator=(Drawer&);
};

#endif /* DRAWER_H */