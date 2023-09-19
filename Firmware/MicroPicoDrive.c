#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "MicroDriveControl.h"
#include "UserInterface.h"

int main()
{
    //We don't give a fuck, overclock to 200Mhz because we can :P
    set_sys_clock_khz(200000, true);

    //Start the MD control in core1
    multicore_launch_core1(RunUserInterface);

    //Run the user interface in core0
    RunMDControl();

    return 0;
}
