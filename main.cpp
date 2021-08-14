#include "monitorcapture.h"

#include <iostream>

using namespace std;

int main()
{
    MonitorCapture monitor_capture;

    monitor_capture.EnumAllMonitor();
    for (auto i : monitor_capture.monitor_list) {
        cout << i.name << endl;
        cout << i.is_primary << endl;
        cout << i.virtual_coord.right - i.virtual_coord.left << " * "
            << i.virtual_coord.bottom - i.virtual_coord.top << endl;
        cout << i.physical_coord.right - i.physical_coord.left << " * "
            << i.physical_coord.bottom - i.physical_coord.top << endl;
        cout << i.work_coord.right - i.work_coord.left << " * "
            << i.work_coord.bottom - i.work_coord.top << endl;
    }

    monitor_capture.CaptureScreen();

    return 0;
}
