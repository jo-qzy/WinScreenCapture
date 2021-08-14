#pragma once

#include <windows.h>
#include <winuser.h>

#include <vector>
#include <string>

using namespace std;

struct monitor_info_t
{
    HMONITOR handle; // monitor handle
    string name; // monitor name
    RECT virtual_coord; // Virtual display coordinates
    RECT physical_coord; // Physical display coordinate
    RECT work_coord; // Work display coordinates
    BOOL is_primary; // Primary monitor flag
    int width;
    int height;
};

class MonitorCapture
{
public:
    void EnumAllMonitor();
    void CaptureScreen();

private:
    static BOOL CALLBACK MonitorEnumProc(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);

public:
    vector<monitor_info_t> monitor_list;
};

