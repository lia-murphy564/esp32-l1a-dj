#include "lib/_CSDHub.h"

namespace CSD
{

CSDHub::CSDHub()
{}

CSDHub::~CSDHub()
{}

void CSDHub::setup()
{
    myDisplay.setDisplay(*displayDebug);
    myDisplay.setup();

    comLinkHub.begin();
}

void CSDHub::update()
{
    static int hubBtn1 = 0, hubBtn2 = 0;

    Event e;
    
    while (comLinkHub.receive(e))
    {
    switch (e.msg.control)
    {
        case CTL_BTN1: 
            hubBtn1 = e.msg.value;
            midiOut.controlChange(1, 20, hubBtn1 == LOW ? 127 : 0);
            break;
        case CTL_BTN2: 
            hubBtn2 = e.msg.value;
            midiOut.controlChange(1, 21, hubBtn2 == LOW ? 127 : 0);
            break;
        case CTL_POT:  
            displayDebug->setPot(e.msg.value);
            midiOut.controlChange(1, 22, std::clamp(e.msg.value >> 5, 0, 127));
            break;
    }

    displayDebug->setButtons(hubBtn1, hubBtn2);
    myDisplay.setDisplay(*displayDebug);
    }

    comLinkHub.update();
    
    myDisplay.update();

    myledblue.update();
    myledwhite.update();
    myledgreen.update();
}




// MODULE
CSDModule::CSDModule()
{}

CSDModule::~CSDModule()
{}

void CSDModule::setup()
{

}

void CSDModule::update()
{
    
}

}