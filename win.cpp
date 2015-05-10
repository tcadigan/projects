/*
 * win.cpp
 * 
 * 2006 Shamus Young
 *
 * Create the main windows and make it go.
 *
 */

#include "win.hpp"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <GL/glut.h>

#include "primitives/types.hpp"

#include "camera.hpp"
#include "car.hpp"
#include "entity.hpp"
#include "ini.hpp"
#include "macro.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

#define MOUSE_MOVEMENT 0.5f

#ifdef _WIN32
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "glu32.lib")
#if SCREENSAVER
#pragma comment (lib, "scrnsave.lib")
#endif
#endif

#ifdef _WIN32
static HWND hwnd;
static HINSTANCE module;
#endif

// HACK
static int width = 640;
static int height = 480;

static int half_width;
static int half_height;
static bool lmb;
static bool rmb;
static bool mouse_forced;

#ifdef _WIN32
static POINT select_pos;
static POINT mouse_pos;
#endif

static bool quit;

#ifdef _WIN32
static HINSTANCE instance;
LONG WINAPI ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

static void CenterCurso()
{
#ifdef _WIN32
    // FIXME
    int center_x;
    int center_y;
    RECT rect;

    SetCursor(NULL);
    mouse_forced = true;
    GetWindowRect(hwnd, &rect);
    center_x = rect.left + ((rect.right - rect.left) / 2);
    center_y = rect.top + ((rect.bottom - rect.top) / 2);
    SetCursorPos(center_x, center_y);
#endif
}

static void MoveCursor(int x, int y)
{
#ifdef _WIN32
    // FIXME
    int center_x;
    int center_y;
    RECT rect;

    SetCursor(NULL);
    mouse_forced = true;
    GetWindowRect(hwnd, &rect);
    center_x = rect.let + x;
    center_y = rect.top + y;
    SetCursorPos(center_x, center_y);
#endif
}

void WinPopup(char *message, ...)
{
    va_list marker;
    char buf[1024];

    va_start(marker, message);
    vsprintf(buf, message, marker);
    va_end(marker);

#ifdef _WIN32
    MessageBox(NULL, buf, APP_TITLE, MB_ICONSTOP | MB_OK | MB_TASKMODAL);
#else
    // FIXME
    printf("%s\n", buf);
#endif
}

int WinWidth(void)
{
    return width;
}

void WinMousePosition(int *x, int *y)
{
#ifdef _WIN32
    // FIXME
    *x = select_pos.x;
    *y = select_pos.y;
#endif
}

int WinHeight(void)
{
    return height;
}

void WinTerm(void)
{
#ifdef _WIN32
    // FIXME
#if !SCREENSAVER
    DestroyWindow(hwnd);
#endif
#endif
}

#ifdef _WIN32
HWND WinHwnd(void)
{
    return hwnd;
}
#endif

void AppQuit()
{
    quit = true;
}

void AppUpdate()
{
    CameraUpdate();
    EntityUpdate();
    WorldUpdate();
    TextureUpdate();
    VisibleUpdate();
    CarUpdate();
    RenderUpdate();
}

void AppInit(void)
{
    RandomInit(time(NULL));
    CameraInit();
    RenderInit();
    TextureInit();
    WorldInit();
}

void AppTerm(void)
{
    TextureTerm();
    WorldTerm();
    RenderTerm();
    CameraTerm();
    WinTerm();
}

#ifndef _WIN32
static void resize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    
    IniIntSet("WindowWidth", width);
    IniIntSet("WindowHeight", height);
    
    RenderResize();
}

static void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'R':
    case 'r':
        WorldReset();
        break;
    case 'W':
    case 'w':
        RenderWireframeToggle();
        break;
    case 'E':
    case 'e':
        RenderEffectCycle();
        break;
    case 'L':
    case 'l':
        RenderLetterboxToggle();
        break;
    case 'F':
    case 'f':
        RenderFPSToggle();
        break;
    case 'G':
    case 'g':
        RenderFogToggle();
        break;
    case 'T':
    case 't':
        RenderFlatToggle();
        break;
#if !SCREENSAVER
    case 'C':
    case 'c':
        CameraAutoToggle();
        break;
    case 'B':
    case 'b':
        CameraNextBehavior();
        break;
#endif
    default:
        return;
    }

    glutPostRedisplay();
}

static void keyboard_s(int key, int x, int y)
{
    switch(key) {
    case GLUT_KEY_F1:
        RenderHelpToggle();
        break;
#if !SCREENSAVER
    case GLUT_KEY_F5:
        CameraReset();
        break;
    case GLUT_KEY_UP:
        CameraMedial(1.0f);
        break;
    case GLUT_KEY_DOWN:
        CameraMedial(-1.0f);
        break;
    case GLUT_KEY_LEFT:
        CameraLateral(1.0f);
        break;
    case GLUT_KEY_RIGHT:
        CameraLateral(-1.0f);
        break;
    case GLUT_KEY_PAGE_UP:
        CameraVertical(1.0f);
        break;
    case GLUT_KEY_PAGE_DOWN:
        CameraVertical(-1.0f);
        break;
#endif
    default:
        return;
    }

    glutPostRedisplay();
}

static void idle()
{
    AppUpdate();
}

static void visible(int vis)
{
    glutIdleFunc(vis == GLUT_VISIBLE ? idle : NULL);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitDisplayString("double rgba depth >= 16 rgba");
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindows(APP_TITLE);
    glutVisibilityFunc(visible);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_s);

    AppInit();
    
    glutMainLoop();
    
    AppTerm();

    return 0;
}

# else
#if !SCREENSAVER
int PASCAL WinMain(HINSTANCE instance_in, 
                   HINSTANCE previous_instance,
                   LPSTR command_line,
                   int show_style)
{
    MSG msg;

    instance = instance_in;
    WinInit();
    AppInit();
    while(!quit) {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                quit = true;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            AppUpdate();
        }
    }

    AppTerm();
    
    return 0;
}

#else 
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg,
                                       UINT msg,
                                       WPARAM wParam,
                                       LParam)
{
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

#endif

LONG WINAPI ScreenSaverProc(HWND hwnd_in,
                            UINT message,
                            WPARAM wparam,
                            LPARAM lparam)
{
    RECT r;
    int key;
    float delta_x;
    float delta_y;
    POINT p;

    // Handles screen saver message
    switch(message) {
    case WM_SIZE:
        // Width of client area
        width = LOWORD(lparam);
        // Height of client area
        height = HIWORD(lparam);

        if(wparam == SIZE_MAXIMIZED) {
            IniIntSet("WindowMaximized", 1);
        }
        else {
            IniIntSet("WindowWidth", width);
            IniIntSet("WindowHeight", height);
            IniIntSet("WindowMaximized", 0);
        }
        
        RenderResize();
        break;
    case WM_KEYDOWN:                               
        key = (int)wparam;
        if(key == 'R') {
            WorldReset();
        }
        else if(key == 'W') {
            RenderWireFrameToggle();
        }
        else if(key == 'E') {
            RenderEffectCycle();
        }
        else if(key == 'L') {
            RenderLetterboxToggle();
        }
        else if(key == 'F') {
            RenderFPSToggle();
        }
        else if(key == 'G') {
            RenderFogToggle();
        }
        else if(key == 'T') {
            RenderFlatToggle();
        }
        else if(key == VK_F1) {
            RenderHelpToggle();
        }
        else if(key == VK_ESCAPE) {
            break;
        }
        else if(!SCREENSAVER) {
            // Dev mode keys
            if(key == 'C') {
                CameraAutoToggle();
            }
            if(key == 'B') {
                CameraNextBehavior();
            }
            if(key == VK_F5) {
                CameraReset();
            }
            if(key == VK_UP) {
                CameraMedial(1.0f);
            }
            if(key == VK_DOWN) {
                CameraMedial(-1.0f);
            }
            if(key == VK_LEFT) {
                CameraLateral(1.0f);
            }
            if(key == VK_RIGHT) {
                CameraLateral(-1.0f);
            }
            if(key == VK_PRIOR) {
                CameraVertical(1.0f);
            }
            if(key == VK_NEXT) {
                CameraVertical(-1.0f);
            }
            if(key == VK_F5) {
                CameraReset();
            }
            return 0;
        }
        else {
            break;
        }
    case WM_MOVE:
        GetClientRect(hwnd, &r);
        height = r.bottom - r.top;
        width = r.right - r.left;
        IniIntSet("WindowX", r.left);
        IniIntSet("WindowY", r.top);
        IniIntSet("WindowWidth", width);
        IniIntSet("WindowHeight", height);
        half_width = width / 2;
        half_height = height / 2;
        return 0;
    case WM_LBUTTONDOWN:
        lmb = true;
        SetCapture(hwnd);
        break;
    case WM_RBUTTONDOWN:
        rmb = true;
        SetCapture(hwnd);
        break;
    case WM_LBUTTONUP:
        lmb = false;
        if(!rmb) {
            ReleaseCapture();
            MoveCursor(select_pos.x, select_pos.y);
        }

        break;
    case WM_RBUTTONUP:
        rmb = false;
        if(!lmb) {
            ReleaseCapture();
            MoveCursor(select_pos.x, select_pos.y);
        }

        break;
    case WM_MOUSEMOVE:
        // Horizontal position of the cursor
        p.x = LOWORD(lparam);
        // Vertical position of the cursor
        p.y = HIWORD(lparam);

        if((p.x < 0) || (p.x > width)) {
            break;
        }
        
        if((p.y < 0) || (p.y > height)) {
            break;
        }

        if(!mouse_forced && !lmb && !rmb) {
            select_pos = p;
        }

        if(mouse_forced) {
            mouse_forced = false;
        }
        else if(rmb || lmb) {
            CenterCursor();
            delta_x = (float)(mouse_pos.x - p.x) * MOUSE_MOVEMENT;
            delta_y = (float)(mouse_pos.y - p.y) * MOUSE_MOVEMENT;
            if(rmb && lmb) {
                GLVector pos;
                CameraPan(delta_x);
                pos = CameraPosition();
                pos.y  += delta_y;
                CameraPositionSet(pos);
            }
            else if(rmb) {
                CameraPan(delta_x);
                CameraForward(delta_y);
            }
            else if(lmb) {
                GLvector angle;
                angle = CameraAngle();
                angle.y -= delta_x;
                angle.x += delta_y;
                CameraAngleSet(angle);
            }
        }

        mouse_pos = p;
        break;
    case WM_CREATE:
        hwnd = hwnd_in;
        if(SCREENSAVE) {
            AppInit();
        }
        SetTimer(hwd, 1, 7, NULL);
        return 0;
    case WM_TIMER:
        AppUpdate();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

#if SCREENSAVER
    return DefScreenSaverProc(hwdn_in, message, wparam, lparam);
#else
    return DefWindowProc(hwnd_in, message, wparam, lparam);
#endif
}

bool WinInit(void)
{
    WNDCLASSEX wcex;
    int x;
    int y;
    int style;
    bool max;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)ScreenSaverProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wxec.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszCclassName = APP_TITLE;
    wcex.hIconSm = NULL;

    if(!RegisterClassEx(&wcex)) {
        WinPopup("Cannot create window class");
        return false;
    }

    x = IniInt("WindowX");
    y = IniInt("WindowY");
    style = WS_TILEWINDOW;
    style |= WS_MAXSIZE;
    wisth = IniInt("WindowWidth");
    height = IniInt("WindowHeight");
    width = CLAMP(width, 800, 2048);
    height = CLAMP(height, 600, 2048);
    half_width = width / 2;
    half_height = height / 2;
    max = (IniInt("WindowMaximized") == 1);
    
    hwnd = CreateWindowEx(0,
                          APP_TITLE,
                          APP_TITLE,
                          style,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          instance,
                          NULL);

    if(!hwnd) {
        WinPopup("Cannot create window");
        return false;
    }

    if(max) {
        ShowWindow(hwnd, SW_MAXSIZE);
    }
    else {
        ShowWindow(hwnd, SW_SHOW);
    }

    UpdateWindow(hwnd);

    return true;
}

#endif
    
