#include "window.h"

#ifdef OS_WINDOWS
#include "core/logging.h"
#include "mem.h"
#include <Windows.h>

const char win_class_name[] = "gdf_window";
static u16 current_window_id = 0;

typedef struct {
    HMODULE h_instance;
    HWND hwnd;
} InternalWindowState;

LRESULT CALLBACK process_msg(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

GDF_Window* GDF_CreateWindow(i16 x, i16 y, i16 w, i16 h, const char* title) 
{
    // create window and stuff

    GDF_Window* window = malloc(sizeof(GDF_Window));
    window->internals = malloc(sizeof(InternalWindowState));
    window->id = current_window_id++;

    InternalWindowState* internals = (InternalWindowState*)window->internals;

    internals->h_instance = GetModuleHandleA(0);

    // resgister window class
    // HICON icon = LoadIconA(internals->h_instance, IDI_APPLICATION);

    WNDCLASSA win_class;
    memset(&win_class, 0, sizeof(win_class));
    win_class.style = CS_DBLCLKS;
    win_class.lpfnWndProc = process_msg;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = internals->h_instance;
    win_class.hIcon = NULL;
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = NULL;
    win_class.lpszClassName = win_class_name;

    if (RegisterClassA(&win_class) == 0)
    {
        MessageBoxA(0,"Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        LOG_FATAL("Could not register window class.");
        return NULL;
    }
    
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
        window_style, x, y, w, h,
        0, 0, internals->h_instance, 0);

    DWORD last_err = GetLastError();
    LOG_DEBUG("%d", last_err);
  
    if (handle == NULL) 
    {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        LOG_FATAL("Could not create window: %d", last_err);
        return NULL;
    } 
    else 
    {
        internals->hwnd = handle;
    }

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

bool GDF_DestroyWindow(GDF_Window* window)
{
    return false;
}

#endif