#pragma once

#include "lib/CSDDevice.h"

namespace CSD
{
    class CSDModule : public CSDDevice
    {
    public:
        CSDModule();
        ~CSDModule();
        void setup();
        void update();
    };
}