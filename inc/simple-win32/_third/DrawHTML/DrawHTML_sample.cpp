/*
 * DrawHTML_sample.C
 *
 * This example shows a simple string formatted as HTML.
 *
 * Compiling:
 *   Borland C++
 *      bcc32 -tW DrawHTML_sample.cpp DrawHTML.c
 *
 *   Microsoft C/C++
 *      cl -GA DrawHTML_sample.cpp DrawHTML.c user32.lib gdi32.lib
 *
 *   Watcom C/C++ 11.0
 *      wcl386 /l=nt_win DrawHTML_sample.cpp DrawHTML.c
 *
 */
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "drawhtml.h"


/* constants */
static const int Margin       = 10;
static const char szAppName[] = "DrawHTML example";

/* global variables */
static HFONT hfontBase;

static BOOL Cls_OnCreate(HWND /*hwnd*/, CREATESTRUCT FAR * /*lpCreateStruct*/)
{
  hfontBase = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                         ANSI_CHARSET, ANSI_CHARSET, CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY, VARIABLE_PITCH, "Georgia");
  return TRUE;
}

static void Cls_OnDestroy(HWND /*hwnd*/)
{
  DeleteObject(hfontBase);
  PostQuitMessage(0);
}

static void Cls_OnPaint(HWND hwnd)
{
  PAINTSTRUCT PaintStruct;
  BeginPaint(hwnd, &PaintStruct);
  HFONT hfontOrg = (HFONT)SelectObject(PaintStruct.hdc, hfontBase);

  RECT rc;
  GetClientRect(hwnd, &rc);
  SetRect(&rc, rc.left + Margin, rc.top + Margin,
               rc.right - Margin, rc.bottom - Margin);

  DrawHTML(PaintStruct.hdc,
           "<p>Beauty, success, truth ..."
           "<br><em>He is blessed who has two.</em>"
           "<br><font color='#C00000'><b>Your program has none.</b></font>"
           "<p><em>Ken Carpenter</em>",
           -1,
           &rc,
           DT_WORDBREAK);

  SelectObject(PaintStruct.hdc, hfontOrg);
  EndPaint(hwnd, &PaintStruct);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
                         WPARAM wParam, LPARAM lParam )
{
  switch (Message) {
    HANDLE_MSG(hwnd, WM_CREATE,          Cls_OnCreate);
    HANDLE_MSG(hwnd, WM_DESTROY,         Cls_OnDestroy);
    HANDLE_MSG(hwnd, WM_PAINT,           Cls_OnPaint);
  default:
    return DefWindowProc(hwnd, Message, wParam, lParam);
  } /* switch */
}

static BOOL InitApplication(HINSTANCE hInstance)
{
  WNDCLASS wc = { 0 };
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = szAppName;
  return (RegisterClass(&wc) != 0);
}

static HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  HWND hwnd = CreateWindow(szAppName, szAppName,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                           NULL, NULL, hInstance, NULL );
  if (hwnd != NULL) {
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
  } /* if */
  return hwnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR /*lpszCmdParam*/, int nCmdShow)
{
  if (hPrevInstance == NULL && !InitApplication(hInstance))
    return FALSE;

  if (!InitInstance(hInstance, nCmdShow))
    return FALSE;

  MSG Msg;
  while (GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage( &Msg );
  } /* while */
  return Msg.wParam;
}
