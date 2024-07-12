#include "window.h"

#ifdef OS_WINDOWS
#include "engine/core/logging.h"
#include "engine/core/mem.h"
#include <windows.h>
#include <windowsx.h>
#include <vulkan/vulkan_win32.h>
#include "engine/core/event.h"
#include <hidusage.h>

const char win_class_name[] = "gdf_window";
static u16 current_window_id = 0;
static HMODULE class_h_instance = NULL;

static GDF_Window* MAIN_WINDOW = NULL;

typedef struct {
    HWND hwnd;
    u32 client_w;
    u32 client_h;
    // top left corner
    u32 x;
    // top left corner
    u32 y;
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
            GDF_EventContext ctx = {};
            GDF_EVENT_Fire(GDF_EVENT_INTERNAL_APP_QUIT, NULL, ctx);
            return true;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_MOVE: 
        {
            // TODO! UPDATE WINDOW INTERNALS
            break;
        }
        case WM_KILLFOCUS:
        {
            GDF_EventContext ctx;
            ctx.data.b = false;
            GDF_EVENT_Fire(GDF_EVENT_INTERNAL_WINDOW_FOCUS_CHANGE, NULL, ctx);
            break;
        }
        case WM_SETFOCUS:
        {
            GDF_EventContext ctx;
            ctx.data.b = true;
            GDF_EVENT_Fire(GDF_EVENT_INTERNAL_WINDOW_FOCUS_CHANGE, NULL, ctx);
            break;
        }
        case WM_SIZE: 
        {
            // TODO! UPDATE WINDOW INTERNALS
            RECT r;
            GetClientRect(hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;
            
            GDF_EventContext ctx;
            ctx.data.u16[0] = width;
            ctx.data.u16[1] = height;
            GDF_EVENT_Fire(GDF_EVENT_INTERNAL_WINDOW_RESIZE, NULL, ctx);
            break;
        } 
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: 
        {
            bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            GDF_KEYCODE key = (u16)w_param;

            // COPY AND PASTED HOW DOES THIS WORK WTF
            bool is_extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;

            // Keypress only determines if _any_ alt/ctrl/shift key is pressed. Determine which one if so.
            if (w_param == VK_MENU) {
                key = is_extended ? GDF_KEYCODE_RALT : GDF_KEYCODE_LALT;
            } else if (w_param == VK_SHIFT) {
                // Annoyingly, KF_EXTENDED is not set for shift keys.
                u32 left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
                u32 scancode = ((l_param & (0xFF << 16)) >> 16);
                key = scancode == left_shift ? GDF_KEYCODE_LSHIFT : GDF_KEYCODE_RSHIFT;
            } else if (w_param == VK_CONTROL) {
                key = is_extended ? GDF_KEYCODE_RCONTROL : GDF_KEYCODE_LCONTROL;
            }

            // HACK: This is gross windows keybind crap.
            if (key == VK_OEM_1) {
                key = GDF_KEYCODE_SEMICOLON;
            }

            __input_process_key(key, pressed);

            return 0; // to prevent default window behaviors
        } 
        case WM_MOUSEMOVE: 
        {
            i32 x = GET_X_LPARAM(l_param);
            i32 y = GET_Y_LPARAM(l_param);

            __input_process_mouse_move(x, y);
            break;
        }
        case WM_MOUSEWHEEL: 
        {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) 
            {
                // Flatten the input to an OS-independent (-1, 1)
                z_delta = (z_delta < 0) ? -1 : 1;
                __input_process_mouse_wheel(z_delta);
            }
            break;
        } 
        // cases fall through
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: 
        {
            bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            GDF_MBUTTON mouse_button = GDF_MBUTTON_MAX;
            switch (msg) 
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button = GDF_MBUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button = GDF_MBUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button = GDF_MBUTTON_RIGHT;
                    break;
            }

            if (mouse_button != GDF_MBUTTON_MAX) 
            {
                __input_process_button(mouse_button, pressed);
            }
            break;
        }
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)l_param, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
            
            RAWINPUT* raw = (RAWINPUT*)lpb;
            
            if (raw->header.dwType == RIM_TYPEMOUSE) 
            {
                i32 dx = raw->data.mouse.lLastX;
                i32 dy = raw->data.mouse.lLastY;
                __input_process_raw_mouse_move(dx, dy);
            }
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
    char ico_path[500];
    GDF_GetAbsolutePath("resources/icon.ico", ico_path);
    HICON icon = (HICON)LoadImage(class_h_instance, ico_path, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
    if (icon == NULL)
    {
        LOG_ERR("failed to create icon");
    }
    win_class.lpfnWndProc = process_msg;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = class_h_instance;
    win_class.hIcon = icon;
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = NULL;
    win_class.lpszClassName = win_class_name;

    if (RegisterClassA(&win_class) == 0)
    {
        LOG_FATAL("Could not register window class. Last error: %d", GetLastError());
        MessageBoxA(0,"Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    class_h_instance = GetModuleHandleA(0);
    return true;
}

void GDF_ShutdownWindowing()
{
    // TODO! destroy windows then unregisterclass 
    // even though it should be automatic but its fine
}

GDF_Window* GDF_CreateWindow(i16 x_, i16 y_, i16 w, i16 h, const char* title) 
{
    // TODO!
    i16 x = x_ == GDF_WIN_CENTERED ?  300 /*calc later*/ : x_;
    i16 y = y_ == GDF_WIN_CENTERED ?  300 /*calc later*/ : y_;
    // create window and stuff

    GDF_Window* window = GDF_Malloc(sizeof(GDF_Window), GDF_MEMTAG_APPLICATION);
    window->internals = GDF_Malloc(sizeof(InternalWindowState), GDF_MEMTAG_APPLICATION);
    window->id = current_window_id++;

    InternalWindowState* internals = (InternalWindowState*) window->internals;
    
    // create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = w;
    u32 client_height = h;

    internals->client_w = client_width;
    internals->client_h = client_height;
    internals->x = client_x;
    internals->y = client_y;

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

    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
    rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = handle;

    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

    // Show the window
    bool should_activate = true;  // TODO: if the window should not accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(internals->hwnd, show_window_command_flags);

    if (MAIN_WINDOW == NULL)
    {
        MAIN_WINDOW = window;
    }

    return window;
}

// TODO! this is DEFINITELY INCOMPLETE.
bool GDF_SetWindowPos(i16 dest_x, i16 dest_y)
{
    return false;
}

void GDF_GetWindowPos(i16* x, i16* y)
{
    InternalWindowState* internals = (InternalWindowState*) MAIN_WINDOW->internals;
    *x = internals->x;
    *y = internals->y;
}

// TODO! this seems to be incomplete..
bool GDF_SetWindowSizeInternal(i16 w, i16 h)
{
    InternalWindowState* internals = (InternalWindowState*) MAIN_WINDOW->internals;
    internals->client_w = w;
    internals->client_h = h;
    return true;
}

void GDF_GetWindowSize(i16* w, i16* h)
{
    InternalWindowState* internals = (InternalWindowState*) MAIN_WINDOW->internals;
    *w = internals->client_w;
    *h = internals->client_h;
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

void GDF_VK_GetRequiredExtensionNames(const char*** names_list)
{
    GDF_LIST_Push(*names_list, &"VK_KHR_win32_surface");
}

bool GDF_VK_CreateSurface(vk_renderer_context* context)
{
    VkWin32SurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    create_info.hinstance = class_h_instance;
    create_info.hwnd = ((InternalWindowState*)MAIN_WINDOW->internals)->hwnd;

    if (&vkCreateWin32SurfaceKHR == NULL)
    {
        LOG_ERR("WTF BRO");
    }
    VkResult result = vkCreateWin32SurfaceKHR(context->instance, &create_info, context->allocator, &context->surface);
    if (result != VK_SUCCESS) {
        LOG_ERR("Vulkan surface creation failed.");
        return false;
    }

    LOG_DEBUG("Created Vulkan surface.");
    return true;
}

GDF_Window* GDF_GetMainWindow()
{
    return MAIN_WINDOW;
}

#endif