#include "window.h"

#ifdef OS_WINDOWS
#include "core/logging.h"
#include "pmem.h"
#include <Windows.h>

const char win_class_name[] = "gdf_window";
static u16 current_window_id = 0;
static HMODULE class_h_instance = NULL;

typedef struct {
    HWND hwnd;
} InternalWindowState;

LRESULT CALLBACK process_msg(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }
        case WM_CLOSE:
        {

        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: 
        {
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Fire an event for window resize
            break;
        } 
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: 
        {
            // Key pressed/released
            //b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing

        } break;
        case WM_MOUSEMOVE: 
        {
            // Mouse move
            //i32 x_position = GET_X_LPARAM(l_param);
            //i32 y_position = GET_Y_LPARAM(l_param);
            // TODO: input processing
            break;
        }
        case WM_MOUSEWHEEL: 
        {
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
            //     // Flatten the input to an OS-independent (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: input processing.
            // }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: 
        {
            // bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing
            break;
        } 
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

bool GDF_InitWindowing()
{
    // resgister window class
    // HICON icon = LoadIconA(internals->h_instance, IDI_APPLICATION);

    WNDCLASSA win_class;
    memset(&win_class, 0, sizeof(win_class));
    win_class.style = CS_DBLCLKS;
    win_class.lpfnWndProc = process_msg;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = class_h_instance;
    win_class.hIcon = NULL;
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = NULL;
    win_class.lpszClassName = win_class_name;

    if (RegisterClassA(&win_class) == 0)
    {
        MessageBoxA(0,"Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        LOG_FATAL("Could not register window class.");
        return false;
    }
    class_h_instance = GetModuleHandleA(0);
    return true;
}

GDF_Window* GDF_CreateWindow(i16 x_, i16 y_, i16 w, i16 h, const char* title) 
{
    // TODO!
    i16 x = x_ == GDF_WIN_CENTERED ?  300 /*calc later*/ : x_;
    i16 y = y_ == GDF_WIN_CENTERED ?  300 /*calc later*/ : y_;
    // create window and stuff

    GDF_Window* window = malloc(sizeof(GDF_Window));
    window->internals = malloc(sizeof(InternalWindowState));
    window->id = current_window_id++;

    InternalWindowState* internals = (InternalWindowState*) window->internals;
    
    // create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = w;
    u32 client_height = h;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border.
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // In this case, the border rectangle is negative.
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the OS border.
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(
        window_ex_style, win_class_name, title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, class_h_instance, 0);

    if (handle == NULL) 
    {
        LOG_FATAL("Could not create window: %d", GetLastError());
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    } 
    internals->hwnd = handle;

    // Show the window
    bool should_activate = true;  // TODO: if the window should not accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(internals->hwnd, show_window_command_flags);

    return window;
}

bool GDF_SetWindowPos(i16 dest_x, i16 dest_y)
{
    return false;
}

bool GDF_SetWindowSize(i16 w, i16 h)
{
    return false;
}

bool GDF_PumpMessages()
{
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return true;
}

bool GDF_DestroyWindow(GDF_Window* window)
{
    InternalWindowState* internals = (InternalWindowState*) window->internals;
    if (internals->hwnd) 
    {
        DestroyWindow(internals->hwnd);
    }
    return true;
}

#endif