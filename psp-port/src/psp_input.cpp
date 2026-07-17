#include "psp_input.h"
#include <pspkernel.h>
#include <pspctrl.h>

void PspInput::Init()
{
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}
