#include "monitorcapture.h"

#include <iostream>

using namespace std;

void SaveBitmapToFile(HBITMAP hBitMap, LPCWSTR lpstrFileName)
{
    BITMAP bitmap;
    GetObject(hBitMap, sizeof(BITMAP), &bitmap);
    cout << "write file" << endl;

    BITMAPFILEHEADER     bmfHdr;           //λͼ�ļ�ͷ�ṹ
    BITMAPINFOHEADER     bi;               //λͼ��Ϣͷ�ṹ
    LPBITMAPINFOHEADER   lpbi;             //ָ��λͼ��Ϣͷ�ṹ

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bitmap.bmWidth;
    bi.biHeight = bitmap.bmHeight;
    cout << "1: " << bitmap.bmWidth << " 2: " << bitmap.bmHeight << endl;
    bi.biPlanes = 1;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    int iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1) bi.biBitCount = 1;
    else if (iBits <= 4) bi.biBitCount = 4;
    else if (iBits <= 8) bi.biBitCount = 8;
    else if (iBits <= 24) bi.biBitCount = 24;
    else bi.biBitCount = iBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmBitsSize = ((bitmap.bmWidth * bi.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;

    // �����ɫ���С
    DWORD dwPaletteSize = 0;
    if (bi.biBitCount <= 8) dwPaletteSize = (1 << bi.biBitCount) * sizeof(RGBQUAD);

    // ����λͼ�ļ�ͷ
    bmfHdr.bfType = 0x4D42;     // "BM "
    DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    // Ϊλͼ���ݷ����ڴ�
    HANDLE hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;
    // �����ɫ��      
    HPALETTE hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
    HPALETTE hOldPal = NULL;
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal = SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
    }
    // ��ȡ�õ�ɫ�����µ�����ֵ
    GetDIBits(hDC, hBitMap, 0, (UINT)bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
    // �ָ���ɫ��      
    if (hOldPal)
    {
        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    HANDLE hFile = CreateFile("test.bmp", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    // д��λͼ�ļ�ͷ
    DWORD dwWritten = 0;
    cout << "Write file " << WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL) << endl;
    // д��λͼ�ļ���������
    WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(hFile);
}

void MonitorCapture::EnumAllMonitor()
{
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM) this);
}

void MonitorCapture::CaptureScreen()
{
    HDC screen_dc, mem_dc;
    HBITMAP screen_bitmap, old_bitmap;

    int width = monitor_list[0].width;
    int height = monitor_list[0].height;

    screen_dc = GetDC(NULL);
    mem_dc = CreateCompatibleDC(screen_dc);

    screen_bitmap = CreateCompatibleBitmap(screen_dc, width, height);
    old_bitmap = (HBITMAP) SelectObject(mem_dc, screen_bitmap);

    BitBlt(mem_dc, 0, 0, width, height, screen_dc, 0, 0, SRCCOPY);

    SaveBitmapToFile(screen_bitmap, L"test.bmp");
}

inline BOOL CALLBACK MonitorCapture::MonitorEnumProc(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param)
{
    MonitorCapture *monitor_capture;
    MONITORINFOEX info_ex;

    monitor_capture = (MonitorCapture *)param;

    memset(&info_ex, 0, sizeof info_ex);
    info_ex.cbSize = sizeof info_ex;

    if (!GetMonitorInfo(handle, &info_ex) || monitor_capture->monitor_list.size() >= 4) {
        return false;
    }

    monitor_info_t monitor_info;

    monitor_info.handle = handle;
    monitor_info.name = info_ex.szDevice;
    monitor_info.is_primary = info_ex.dwFlags == MONITORINFOF_PRIMARY;
    monitor_info.physical_coord = info_ex.rcMonitor;
    monitor_info.work_coord = info_ex.rcWork;

    if (rect) {
        monitor_info.virtual_coord = *rect;
        monitor_info.width = rect->right - rect->left;
        monitor_info.height = rect->bottom - rect->top;
    }

    monitor_capture->monitor_list.push_back(monitor_info);

    return true;
}
