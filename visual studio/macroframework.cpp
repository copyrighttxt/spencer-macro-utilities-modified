// modified by lx aka copyright.txt
// github.com/copyrighttxt
#include <iostream>
#include <vector>
#include <windows.h>
#include <Psapi.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <cwctype>
#include <string>
#include <atomic>
#include <algorithm>  
#include <tlhelp32.h>
#include <d3d11.h>
#include "imgui-files/imgui.h"
#include "imgui-files/imgui_impl_dx11.h"
#include "imgui-files/imgui_impl_win32.h"
#include "imgui-files/json.hpp"
#include <wininet.h>
#include "resource.h"
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <format>
#include <unordered_map>
#include <math.h>
#include <memory.h>
#include <synchapi.h>
#include <dwmapi.h>
#include <variant>
#include <sstream>

// Library for HTTP (To get version data from my github page)
#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Theme system
struct Theme {
    std::string name;
    ImVec4 bg_dark;
    ImVec4 bg_medium;
    ImVec4 bg_light;
    ImVec4 accent_primary;
    ImVec4 accent_secondary;
    ImVec4 text_primary;
    ImVec4 text_secondary;
    ImVec4 success_color;
    ImVec4 warning_color;
    ImVec4 error_color;
    ImVec4 border_color;
    float window_rounding;
    float frame_rounding;
    float button_rounding;
};

// Default themes
std::vector<Theme> themes = {
    {
        "Modern Dark",
        ImVec4(0.08f, 0.08f, 0.12f, 1.0f),
        ImVec4(0.12f, 0.12f, 0.16f, 1.0f),
        ImVec4(0.16f, 0.16f, 0.20f, 1.0f),
        ImVec4(0.20f, 0.60f, 1.0f, 1.0f),
        ImVec4(0.15f, 0.50f, 0.85f, 1.0f),
        ImVec4(0.95f, 0.95f, 0.95f, 1.0f),
        ImVec4(0.70f, 0.70f, 0.70f, 1.0f),
        ImVec4(0.20f, 0.80f, 0.40f, 1.0f),
        ImVec4(0.90f, 0.60f, 0.20f, 1.0f),
        ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
        ImVec4(0.20f, 0.20f, 0.25f, 1.0f),
        8.0f, 4.0f, 4.0f
    },
    {
        "Cyberpunk",
        ImVec4(0.05f, 0.05f, 0.10f, 1.0f),
        ImVec4(0.10f, 0.10f, 0.15f, 1.0f),
        ImVec4(0.15f, 0.15f, 0.20f, 1.0f),
        ImVec4(0.00f, 0.80f, 1.0f, 1.0f),
        ImVec4(0.00f, 0.60f, 0.80f, 1.0f),
        ImVec4(0.90f, 0.90f, 1.0f, 1.0f),
        ImVec4(0.60f, 0.60f, 0.80f, 1.0f),
        ImVec4(0.00f, 1.0f, 0.50f, 1.0f),
        ImVec4(1.0f, 0.50f, 0.00f, 1.0f),
        ImVec4(1.0f, 0.20f, 0.40f, 1.0f),
        ImVec4(0.00f, 0.40f, 0.60f, 1.0f),
        6.0f, 3.0f, 3.0f
    },
    {
        "Forest Green",
        ImVec4(0.05f, 0.12f, 0.08f, 1.0f),
        ImVec4(0.08f, 0.16f, 0.12f, 1.0f),
        ImVec4(0.12f, 0.20f, 0.16f, 1.0f),
        ImVec4(0.20f, 0.80f, 0.40f, 1.0f),
        ImVec4(0.15f, 0.60f, 0.30f, 1.0f),
        ImVec4(0.90f, 0.95f, 0.90f, 1.0f),
        ImVec4(0.70f, 0.80f, 0.70f, 1.0f),
        ImVec4(0.30f, 0.90f, 0.50f, 1.0f),
        ImVec4(0.90f, 0.70f, 0.20f, 1.0f),
        ImVec4(0.80f, 0.30f, 0.30f, 1.0f),
        ImVec4(0.15f, 0.30f, 0.20f, 1.0f),
        10.0f, 5.0f, 5.0f
    },
    {
        "Sunset Orange",
        ImVec4(0.12f, 0.08f, 0.05f, 1.0f),
        ImVec4(0.16f, 0.12f, 0.08f, 1.0f),
        ImVec4(0.20f, 0.16f, 0.12f, 1.0f),
        ImVec4(1.0f, 0.50f, 0.20f, 1.0f),
        ImVec4(0.80f, 0.40f, 0.15f, 1.0f),
        ImVec4(1.0f, 0.95f, 0.90f, 1.0f),
        ImVec4(0.80f, 0.70f, 0.60f, 1.0f),
        ImVec4(0.40f, 0.80f, 0.30f, 1.0f),
        ImVec4(1.0f, 0.70f, 0.20f, 1.0f),
        ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
        ImVec4(0.30f, 0.20f, 0.15f, 1.0f),
        12.0f, 6.0f, 6.0f
    },
    {
        "Purple Haze",
        ImVec4(0.10f, 0.05f, 0.15f, 1.0f),
        ImVec4(0.15f, 0.10f, 0.20f, 1.0f),
        ImVec4(0.20f, 0.15f, 0.25f, 1.0f),
        ImVec4(0.80f, 0.40f, 1.0f, 1.0f),
        ImVec4(0.60f, 0.30f, 0.80f, 1.0f),
        ImVec4(0.95f, 0.90f, 1.0f, 1.0f),
        ImVec4(0.70f, 0.60f, 0.80f, 1.0f),
        ImVec4(0.40f, 0.80f, 0.60f, 1.0f),
        ImVec4(1.0f, 0.60f, 0.40f, 1.0f),
        ImVec4(0.90f, 0.30f, 0.50f, 1.0f),
        ImVec4(0.25f, 0.15f, 0.30f, 1.0f),
        8.0f, 4.0f, 4.0f
    }
};

// Current theme index and custom theme
int current_theme_index = 0;
Theme custom_theme = themes[0]; // Initialize with first theme
bool show_theme_editor = false;
bool theme_modified = false;

// DirectX11 Variables
ID3D11Device *g_pd3dDevice = NULL;
ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
IDXGISwapChain *g_pSwapChain = NULL;
ID3D11RenderTargetView *g_mainRenderTargetView = NULL;

// Forward Declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void RenderSectionSettings(int section);
static void RenderThemeEditor();
static void ApplyTheme(const Theme& theme);

// TO PUT IN A KEYBOARD KEY, GO TO https://www.millisecond.com/support/docs/current/html/language/scancodes.htm
// Convert the scancode into hexadecimal before putting it into the HoldKey or ReleaseKey functions
// Ex: E = 18 = 0x12 = HoldKey(0x12)

// If you want to create custom HOTKEYS for stuff that isn't an alphabet/function key, go to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes and get your virtual key code or value

std::string text = "/e dance2"; // Custom chat text

std::atomic<bool> isdesyncloop(false); // Set the variable used for the threads
std::atomic<bool> isspeed(false);
std::atomic<bool> isHHJ(false);
std::atomic<bool> isspamloop(false);
std::atomic<bool> isitemloop(false);
std::atomic<bool> iswallwalkloop(false);
std::atomic<bool> isbhoploop(false);

std::mutex renderMutex;
std::condition_variable renderCondVar;
bool renderFlag = false;
bool running = true;
HWND hwnd;

std::atomic<bool> g_isVk_BunnyhopHeldDown(false);
HHOOK g_keyboardHook = NULL;

const DWORD SCAN_CODE_FLAGS = KEYEVENTF_SCANCODE;
const DWORD RELEASE_FLAGS = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

INPUT inputkey = {};
INPUT inputhold = {};
INPUT inputrelease = {};

// Translate all VK's into variables so it's less annoying to debug it
unsigned int scancode_shift = 0x2A;
unsigned int vk_f5 = VK_F5;
unsigned int vk_f6 = VK_F6;
unsigned int vk_f8 = VK_F8;
unsigned int vk_mbutton = VK_MBUTTON;
unsigned int vk_xbutton1 = VK_XBUTTON1;
unsigned int vk_xbutton2 = VK_XBUTTON2;
unsigned int vk_spamkey = VK_LBUTTON;
unsigned int vk_clipkey = VK_F3;
unsigned int vk_wallkey = VK_F1;
unsigned int vk_laughkey = VK_F7;
unsigned int vk_zkey = VkKeyScanEx('Z', GetKeyboardLayout(0)) & 0xFF; // Use this for alphabet keys so it works across layouts
unsigned int vk_dkey = VkKeyScanEx('D', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_xkey = VkKeyScanEx('X', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_wkey = VkKeyScanEx('W', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_bouncekey = VkKeyScanEx('C', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_leftbracket = MapVirtualKey(0x1A, MAPVK_VSC_TO_VK);
unsigned int vk_bunnyhopkey = MapVirtualKey(0x39, MAPVK_VSC_TO_VK);

// Lookup Table - Backup to avoid displaying Hex if unneccessary
std::unordered_map<int, std::string> vkToString = {
    {VK_LBUTTON, "VK_LBUTTON"},
    {VK_RBUTTON, "VK_RBUTTON"},
    {VK_CANCEL, "VK_CANCEL"},
    {VK_MBUTTON, "VK_MBUTTON"},
    {VK_XBUTTON1, "VK_XBUTTON1"},
    {VK_XBUTTON2, "VK_XBUTTON2"},
    {VK_BACK, "VK_BACK"},
    {VK_TAB, "VK_TAB"},
    {VK_CLEAR, "VK_CLEAR"},
    {VK_RETURN, "VK_RETURN"},
    {VK_SHIFT, "VK_SHIFT"},
    {VK_CONTROL, "VK_CONTROL"},
    {VK_MENU, "VK_MENU"},
    {VK_PAUSE, "VK_PAUSE"},
    {VK_CAPITAL, "VK_CAPITAL"},
    {VK_ESCAPE, "VK_ESCAPE"},
    {VK_SPACE, "VK_SPACE"},
    {VK_PRIOR, "VK_PRIOR"},
    {VK_NEXT, "VK_NEXT"},
    {VK_END, "VK_END"},
    {VK_HOME, "VK_HOME"},
    {VK_LEFT, "VK_LEFT"},
    {VK_UP, "VK_UP"},
    {VK_RIGHT, "VK_RIGHT"},
    {VK_DOWN, "VK_DOWN"},
    {VK_SELECT, "VK_SELECT"},
    {VK_PRINT, "VK_PRINT"},
    {VK_EXECUTE, "VK_EXECUTE"},
    {VK_SNAPSHOT, "VK_SNAPSHOT"},
    {VK_INSERT, "VK_INSERT"},
    {VK_DELETE, "VK_DELETE"},
    {VK_HELP, "VK_HELP"},
    {VK_LWIN, "VK_LWIN"},
    {VK_RWIN, "VK_RWIN"},
    {VK_NUMPAD0, "VK_NUMPAD0"},
    {VK_NUMPAD1, "VK_NUMPAD1"},
    {VK_NUMPAD2, "VK_NUMPAD2"},
    {VK_NUMPAD3, "VK_NUMPAD3"},
    {VK_NUMPAD4, "VK_NUMPAD4"},
    {VK_NUMPAD5, "VK_NUMPAD5"},
    {VK_NUMPAD6, "VK_NUMPAD6"},
    {VK_NUMPAD7, "VK_NUMPAD7"},
    {VK_NUMPAD8, "VK_NUMPAD8"},
    {VK_NUMPAD9, "VK_NUMPAD9"},
    {VK_MULTIPLY, "VK_MULTIPLY"},
    {VK_ADD, "VK_ADD"},
    {VK_SEPARATOR, "VK_SEPARATOR"},
    {VK_SUBTRACT, "VK_SUBTRACT"},
    {VK_DECIMAL, "VK_DECIMAL"},
    {VK_DIVIDE, "VK_DIVIDE"},
    {VK_F1, "VK_F1"},
    {VK_F2, "VK_F2"},
    {VK_F3, "VK_F3"},
    {VK_F4, "VK_F4"},
    {VK_F5, "VK_F5"},
    {VK_F6, "VK_F6"},
    {VK_F7, "VK_F7"},
    {VK_F8, "VK_F8"},
    {VK_F9, "VK_F9"},
    {VK_F10, "VK_F10"},
    {VK_F11, "VK_F11"},
    {VK_F12, "VK_F12"},
    {VK_LSHIFT, "VK_LSHIFT"},
    {VK_RSHIFT, "VK_RSHIFT"},
    {VK_LCONTROL, "VK_LCONTROL"},
    {VK_RCONTROL, "VK_RCONTROL"},
    {VK_LMENU, "VK_LMENU"},
    {VK_RMENU, "VK_RMENU"},
    {VK_OEM_PLUS, "VK_OEM_PLUS"},
    {VK_OEM_COMMA, "VK_OEM_COMMA"},
    {VK_OEM_MINUS, "VK_OEM_MINUS"},
    {VK_OEM_PERIOD, "VK_OEM_PERIOD"},
    {VK_OEM_2, "VK_OEM_2"},
    {VK_OEM_3, "VK_OEM_3"},
    {VK_OEM_4, "VK_OEM_4"},
    {VK_OEM_5, "VK_OEM_5"},
    {VK_OEM_6, "VK_OEM_6"},
    {VK_OEM_7, "VK_OEM_7"},
    {VK_OEM_8, "VK_OEM_8"},
    {VK_OEM_102, "VK_OEM_102"}
};

// Window and UI settings
int screen_width = GetSystemMetrics(SM_CXSCREEN) / 1.5;
int screen_height = GetSystemMetrics(SM_CYSCREEN) / 1.5 + 10;
int selected_dropdown = 0;
std::string KeyButtonText = "Click to Bind Key";
std::string KeyButtonTextalt = "Click to Bind Key";
std::string chatkey = "/";

// Keybind and macro settings
char settingsBuffer[256] = "RobloxPlayerBeta.exe"; // Default value for the textbox
char KeyBuffer[256] = "None";
char KeyBufferalt[256] = "None";
char KeyBufferhuman[256] = "None";
char KeyBufferhumanalt[256] = "None";
char ItemDesyncSlot[256] = "5";
char ItemSpeedSlot[256] = "3";
char ItemClipSlot[256] = "7";
char ItemClipDelay[256] = "30";
char BunnyHopDelayChar[256] = "10";
char WallhopPixels[256] = "300";
char WallhopDegrees[256] = "150";
char SpamDelay[256] = "20";
char RobloxSensValue[256] = "0.5";
char RobloxPixelValueChar[256] = "716";
char RobloxWallWalkValueChar[256] = "-94";
char RobloxWallWalkValueDelayChar[256] = "72720";
char ChatKeyChar[2] = "/";
char CustomTextChar[256] = "";


// Toggles and switches
bool macrotoggled = true;
bool shiftswitch = false;
bool unequiptoggle = false;
bool camfixtoggle = false;
bool wallhopswitch = false;
bool wallwalktoggleside = false;
bool wallhopcamfix = false;
bool chatoverride = true;
bool toggle_jump = true;
bool toggle_flick = true;
bool fasthhj = false;
bool wallesslhjswitch = false;
bool autotoggle = false;
bool isspeedswitch = false;
bool isfreezeswitch = false;
bool freezeoutsideroblox = true;
bool iswallwalkswitch = false;
bool isspamswitch = false;
bool isitemclipswitch = false;
bool antiafktoggle = true;
bool bouncesidetoggle = false;
bool bouncerealignsideways = true;
bool bounceautohold = true;
bool laughmoveswitch = false;
bool takeallprocessids = false;
bool ontoptoggle = false;
bool bunnyhoptoggled = false;
bool bunnyhopsmart = true;
bool UserAcknowledgedV2992 = false;

// Section toggles and order
constexpr int section_amounts = 14;
bool section_toggles[14] = {true, true, true, true, true, false, true, true, true, false, false, false, false, false};
int section_order[14] = {0, 1, 2, 3, 4, 5, 6, 13, 7, 8, 9, 10, 11, 12};

// Numeric settings
int wallhop_dx = 300;
int wallhop_dy = -300;
int speed_strengthx = 959;
int wallwalk_strengthx = 94;
int speedoffsetx = 0;
int speed_strengthy = -959;
int wallwalk_strengthy = -94;
int speedoffsety = 0;
int speed_slot = 3;
int desync_slot = 5;
int clip_slot = 7;
int clip_delay = 30;
int BunnyHopDelay = 10;
int RobloxWallWalkValueDelay = 72720;
float spam_delay = 20.0f;
float maxfreezetime = 9.00f;
int maxfreezeoverride = 50;
int real_delay = 10000;
int RobloxPixelValue = 716;
int RobloxWallWalkValue = -94;

// Dropdown options
const char* optionsforoffset[] = {"/e dance2", "/e laugh", "/e cheer"};

// Window and UI state
RECT screen_rect;
int dragged_section = -1; // -1 means no section is being dragged
static int selected_section = -1;

// Process and timing state
bool processFound = false; // Initialize as no process found
bool done = false;
bool bindingMode = false;
bool bindingModealt = false;
bool notbinding = true;
bool wallhopupdate = false;
bool UserOutdated = false;
static bool wasMButtonPressed = false;

// Timing and chrono
auto rebindtime = std::chrono::high_resolution_clock::now();
auto suspendStartTime = std::chrono::steady_clock::time_point();

// Previous values (used for comparisons)
static float PreviousSensValue = -1.0f;
static float PreviousWallWalkSide = 0;
static float PreviousWallWalkValue = 0.5f;
static float windowOpacityPercent = 100.0f;


typedef LONG(NTAPI *NtSuspendProcess)(HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(HANDLE ProcessHandle);


// Helper function to suspend or resume a process
static void SuspendOrResumeProcess(NtSuspendProcess pfnSuspend, NtResumeProcess pfnResume, const std::vector<HANDLE>& pids, bool suspend)
{
    for (HANDLE pid : pids)
    {
        if (suspend)
        {
		pfnSuspend(pid);
        }
        else
        {
		pfnResume(pid);
        }
    }
}


static INPUT createInput()
{
	INPUT inputkey = {};
	inputkey.type = INPUT_KEYBOARD;
	return inputkey;
}

// Hold a key down, self explanatory
static void HoldKey(WORD scanCode)
{
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = SCAN_CODE_FLAGS;

	SendInput(1, &input, sizeof(INPUT));
}

static void ReleaseKey(WORD scanCode)
{
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = RELEASE_FLAGS;

	SendInput(1, &input, sizeof(INPUT));
}

// Keyboard Hook for Bhop
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const KBDLLHOOKSTRUCT* pkbhs = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (pkbhs->vkCode == vk_bunnyhopkey) {
            if ((pkbhs->flags & LLKHF_INJECTED) == 0) {
                g_isVk_BunnyhopHeldDown = ((wParam & 1) == 0);
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// Use these only if the input comes from a bind

static void HoldKeyBinded(WORD Vk_key)
{
    INPUT input = {};

    // Check if Vk corresponds to M1, M2, or M3
    if (Vk_key == VK_LBUTTON || Vk_key == VK_RBUTTON || Vk_key == VK_MBUTTON)
    {
        input.type = INPUT_MOUSE;
        switch (Vk_key)
        {
        case VK_LBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        case VK_RBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        case VK_MBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            break;
        }
    }
    else
    {
		Vk_key = MapVirtualKeyEx(Vk_key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = Vk_key;
        input.ki.dwFlags = SCAN_CODE_FLAGS;
    }

    SendInput(1, &input, sizeof(INPUT));
}

static void ReleaseKeyBinded(WORD Vk_key)
{
    INPUT input = {};

    // Check if Vk corresponds to M1, M2, or M3
    if (Vk_key == VK_LBUTTON || Vk_key == VK_RBUTTON || Vk_key == VK_MBUTTON)
    {
        input.type = INPUT_MOUSE;
        switch (Vk_key)
        {
        case VK_LBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case VK_RBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case VK_MBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        }
    }
    else
    {
		Vk_key = MapVirtualKeyEx(Vk_key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = Vk_key;
        input.ki.dwFlags = RELEASE_FLAGS;
    }

    SendInput(1, &input, sizeof(INPUT));
}


// Move your mouse to any coordinate
static void MoveMouse(int dx, int dy)
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dx = dx;
	input.mi.dy = dy;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(1, &input, sizeof(INPUT));
}

static void PasteText(const std::string &text)
{
	for (char c : text) {
        // Key down event
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;  // Unicode key down
        SendInput(1, &input, sizeof(INPUT));

        // Key up event
        input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;  // Unicode key up
        SendInput(1, &input, sizeof(INPUT));
    }
}

// This is ran in a separate thread to avoid interfering with other functions
static void ItemDesyncLoop()
{
	while (true) { // Efficient variable checking method
		while (!isdesyncloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[1]) {
			HoldKey(desync_slot + 1);
			ReleaseKey(desync_slot + 1);
			HoldKey(desync_slot + 1);
			ReleaseKey(desync_slot + 1);
		}
	}
}

static void Speedglitchloop()
{
	while (true) {
		while (!isspeed) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[3]) {
			MoveMouse(speed_strengthx, 0);
			std::this_thread::sleep_for(std::chrono::microseconds(6060));
			MoveMouse(speed_strengthy, 0);
			std::this_thread::sleep_for(std::chrono::microseconds(6060));
		}
	}
}

static void SpeedglitchloopHHJ()
{
	while (true) {
		while (!isHHJ) {
			std::this_thread::sleep_for(std::chrono::microseconds(99));
		}
		if (macrotoggled && notbinding && section_toggles[2]) {
			MoveMouse(speed_strengthx, 0);
			std::this_thread::sleep_for(std::chrono::microseconds(3030));
			MoveMouse(speed_strengthy, 0);
			std::this_thread::sleep_for(std::chrono::microseconds(3030));
		}
	}
}

static void SpamKeyLoop()
{
	while (true) { // Efficient variable checking method
		while (!isspamloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[11]) {
			HoldKeyBinded(vk_spamkey);
			std::this_thread::sleep_for(std::chrono::microseconds(real_delay));
			ReleaseKeyBinded(vk_spamkey);
			std::this_thread::sleep_for(std::chrono::microseconds(real_delay));
		}
	}
}

static void ItemClipLoop()
{
	while (true) { // Efficient variable checking method
		while (!isitemloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[8]) {
			HoldKey(clip_slot + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(clip_delay / 2));
			ReleaseKey(clip_slot + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(clip_delay / 2));
		}
	}
}

static void WallWalkLoop()
{
	while (true) { // Efficient variable checking method
		while (!iswallwalkloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[10]) {
			if (wallwalktoggleside) {
				MoveMouse(-wallwalk_strengthx, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(6060));
				MoveMouse(-wallwalk_strengthy, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(RobloxWallWalkValueDelay));
			} else {
				MoveMouse(wallwalk_strengthx, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(6060));
				MoveMouse(wallwalk_strengthy, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(RobloxWallWalkValueDelay));
			}
		}
	}
}

static void BhopLoop()
{
    while (true) {
        while (!isbhoploop.load(std::memory_order_acquire)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        HoldKeyBinded(vk_bunnyhopkey);
        std::this_thread::sleep_for(std::chrono::milliseconds(BunnyHopDelay / 2));
        ReleaseKeyBinded(vk_bunnyhopkey);
        std::this_thread::sleep_for(std::chrono::milliseconds(BunnyHopDelay / 2));
    }
}
static bool IsMainWindow(HWND hwnd)
{
	return (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL);
}

static std::vector<HWND> FindWindowByProcessHandle(const std::vector<HANDLE> &handles)
{
    std::vector<HWND> windows;
    for (HANDLE hProcess : handles) {
        DWORD targetPID = GetProcessId(hProcess);
        HWND rbxhwnd = FindWindowEx(NULL, NULL, NULL, NULL);
        while (rbxhwnd != NULL) {
            DWORD windowPID = 0;
            GetWindowThreadProcessId(rbxhwnd, &windowPID);
            if (windowPID == targetPID && IsMainWindow(rbxhwnd)) {
                windows.push_back(rbxhwnd);
                break; // Assume one main window per process
            }
            rbxhwnd = FindWindowEx(NULL, rbxhwnd, NULL, NULL);
        }
    }
    return windows;
}

// Find Process ID of latest Process in selection
static std::vector<DWORD> GetProcessIdByName(bool takeallprocessids)
{
    // Convert target name from settingsBuffer to wide string
    std::string targetName = settingsBuffer;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring targetNameW = converter.from_bytes(targetName);

    // Create a snapshot of all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        processFound = false;
        return std::vector<DWORD>(); // Return empty vector on failure
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    std::vector<DWORD> result;

    if (takeallprocessids)
    {
        // Collect all matching process IDs
        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (_wcsicmp(pe.szExeFile, targetNameW.c_str()) == 0)
                {
                    result.push_back(pe.th32ProcessID);
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        processFound = !result.empty();
    }
    else
    {
        // Find the newest matching process
        DWORD selectedPID = 0;
        ULONGLONG newestCreationTime = 0;
        bool foundAny = false;

        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (_wcsicmp(pe.szExeFile, targetNameW.c_str()) == 0)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
                    if (hProcess)
                    {
                        FILETIME ftCreation, ftExit, ftKernel, ftUser;
                        if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
                        {
                            ULONGLONG creationTime = (static_cast<ULONGLONG>(ftCreation.dwHighDateTime) << 32) | ftCreation.dwLowDateTime;
                            if (!foundAny || creationTime > newestCreationTime)
                            {
                                newestCreationTime = creationTime;
                                selectedPID = pe.th32ProcessID;
                                foundAny = true;
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32Next(hSnapshot, &pe));
        }

        if (foundAny)
        {
            result.push_back(selectedPID);
        }
        processFound = foundAny;
    }

    CloseHandle(hSnapshot);
    return result;
}

static std::vector<HANDLE> GetProcessHandles(const std::vector<DWORD> &pids, DWORD accessRights)
{
    std::vector<HANDLE> handles;
    for (DWORD pid : pids) {
        HANDLE hProcess = OpenProcess(accessRights, FALSE, pid);
        if (hProcess != NULL) { // Check if the handle is valid
            handles.push_back(hProcess);
        }
    }
    return handles;
}

static HWND FindNewestProcessWindow(const std::vector<HWND> &hwnds)
{
    if (hwnds.empty()) {
        return NULL;
    }

    DWORD newestPID = 0;
    ULONGLONG newestCreationTime = 0;
    HWND newestHWND = NULL;
    bool foundAny = false;

    for (HWND hwnd : hwnds) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        // Open the process to get its creation time
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (hProcess) {
            FILETIME ftCreation, ftExit, ftKernel, ftUser;
            if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                ULONGLONG creationTime = (static_cast<ULONGLONG>(ftCreation.dwHighDateTime) << 32) | ftCreation.dwLowDateTime;
                if ((!foundAny || creationTime > newestCreationTime) && IsMainWindow(hwnd)) {
                    newestCreationTime = creationTime;
                    newestPID = pid;
                    newestHWND = hwnd;
                    foundAny = true;
                }
            }
            CloseHandle(hProcess);
        }
    }

    return foundAny ? newestHWND : NULL;
}

static bool IsForegroundWindowProcess(const std::vector<HANDLE> &handles)
{
    HWND foreground = GetForegroundWindow();
    if (!foreground) return false; // No foreground window

    DWORD foregroundPID = 0;
    GetWindowThreadProcessId(foreground, &foregroundPID);

    for (HANDLE hProcess : handles) {
        DWORD targetPID = GetProcessId(hProcess);
        if (foregroundPID == targetPID) {
            return true; // Found a match
        }
    }
    return false; // No match
}


static std::string Trim(const std::string &str)
{ // Trim a string
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        ++start;
    }

    auto end = str.end();
    do {
        --end;
    } while (end != start && std::isspace(*end));

    return std::string(start, end + 1);
}

static size_t OutputReleaseVersion(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


static std::string GetRemoteVersion()
{
    // Set timeout value (MS)
    DWORD timeout = 2000; // Sadly, anything below 5 seconds goes back to 5 seconds
    
    HINTERNET hInternet = InternetOpen(L"User-Agent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    // Set timeouts for the internet handle
    InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hConnect = InternetOpenUrl(hInternet, 
        L"https://raw.githubusercontent.com/Spencer0187/Roblox-Macro-Utilities/main/version", 
        NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    // Read the response with timeout
    char buffer[4096];
    DWORD bytesRead;
    std::string response;
    DWORD startTime = GetTickCount();

    while (true) {
        if (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead)) {
            if (bytesRead == 0) break; // End of response
            response.append(buffer, bytesRead);
        } else {
            // Error occurred
            break;
        }

        // Check if timeout has been exceeded
        if (GetTickCount() - startTime > timeout) {
            response.clear();
            break;
        }
    }

    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return response;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true; // Forward ImGUI-related messages

    switch (msg) {
    case WM_SIZE:
		// Get window size
		GetWindowRect(hWnd, &screen_rect);

		screen_width = screen_rect.right - screen_rect.left;
		screen_height = screen_rect.bottom - screen_rect.top;

		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

        // Set the minimum screen size
        mmi->ptMinTrackSize.x = 1147;
        mmi->ptMinTrackSize.y = 780;
        return 0;
    }

    case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
    case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
    case WM_CLOSE:
		done = true;
		PostQuitMessage(0);
		return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2; // Use double buffering
    sd.BufferDesc.Width = window_width;
    sd.BufferDesc.Height = window_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 120;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pd3dDeviceContext);

    if (FAILED(res))
		return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
    }
    if (g_pd3dDeviceContext) {
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
    }
    if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
    }
}

static std::string TrimNullChars(const char *buffer, size_t size)
{
    size_t length = std::strlen(buffer);
    if (length > size)
		length = size;
    return std::string(buffer, length);
}



using NumericVar = std::variant<int*, float*, unsigned int*>;

// Boolean variables
const std::unordered_map<std::string, bool *> bool_vars = {
	{"macrotoggled", &macrotoggled},
	{"shiftswitch", &shiftswitch},
	{"wallhopswitch", &wallhopswitch},
	{"wallhopcamfix", &wallhopcamfix},
	{"unequiptoggle", &unequiptoggle},
	{"isspeedswitch", &isspeedswitch},
	{"isfreezeswitch", &isfreezeswitch},
	{"iswallwalkswitch", &iswallwalkswitch},
	{"isspamswitch", &isspamswitch},
	{"isitemclipswitch", &isitemclipswitch},
	{"autotoggle", &autotoggle},
	{"toggle_jump", &toggle_jump},
	{"toggle_flick", &toggle_flick},
	{"camfixtoggle", &camfixtoggle},
	{"wallwalktoggleside", &wallwalktoggleside},
	{"UserAcknowledgedV2992", &UserAcknowledgedV2992},
	{"antiafktoggle", &antiafktoggle},
	{"fasthhj", &fasthhj},
	{"wallesslhjswitch", &wallesslhjswitch},
	{"chatoverride", &chatoverride},
	{"bounceautohold", &bounceautohold},
	{"bouncerealignsideways", &bouncerealignsideways},
	{"bouncesidetoggle", &bouncesidetoggle},
	{"laughmoveswitch", &laughmoveswitch},
	{"freezeoutsideroblox", &freezeoutsideroblox},
	{"takeallprocessids", &takeallprocessids},
	{"ontoptoggle", &ontoptoggle},
	{"bunnyhopsmart", &bunnyhopsmart},
};

// Numeric variables
const std::unordered_map<std::string, NumericVar> numeric_vars = {
	{"scancode_shift", &scancode_shift},
	{"vk_f5", &vk_f5},
	{"vk_f6", &vk_f6},
	{"vk_f8", &vk_f8},
	{"vk_mbutton", &vk_mbutton},
	{"vk_xbutton1", &vk_xbutton1},
	{"vk_xbutton2", &vk_xbutton2},
	{"vk_leftbracket", &vk_leftbracket},
	{"vk_spamkey", &vk_spamkey},
	{"vk_zkey", &vk_zkey},
	{"vk_dkey", &vk_dkey},
	{"vk_xkey", &vk_xkey},
	{"vk_clipkey", &vk_clipkey},
	{"vk_laughkey", &vk_laughkey},
	{"vk_bouncekey", &vk_bouncekey},
	{"vk_bunnyhopkey", &vk_bunnyhopkey},
	{"selected_dropdown", &selected_dropdown},
	{"vk_wallkey", &vk_wallkey},
	{"PreviousWallWalkSide", &PreviousWallWalkSide},
	{"speed_slot", &speed_slot},
	{"desync_slot", &desync_slot},
	{"clip_slot", &clip_slot},
	{"spam_delay", &spam_delay},
	{"real_delay", &real_delay},
	{"wallhop_dx", &wallhop_dx},
	{"wallhop_dy", &wallhop_dy},
	{"PreviousWallWalkValue", &PreviousWallWalkValue},
	{"maxfreezetime", &maxfreezetime},
	{"maxfreezeoverride", &maxfreezeoverride},
	{"RobloxWallWalkValueDelay", &RobloxWallWalkValueDelay},
	{"speed_strengthx", &speed_strengthx},
	{"speedoffsetx", &speedoffsetx},
	{"speed_strengthy", &speed_strengthy},
	{"speedoffsety", &speedoffsety},
	{"clip_delay", &clip_delay},
	{"RobloxPixelValue", &RobloxPixelValue},
	{"PreviousSensValue", &PreviousSensValue},
	{"windowOpacityPercent", &windowOpacityPercent},
};

// Char variables
const std::vector<std::pair<std::string, std::pair<char*, size_t>>> char_arrays = {
    {"settingsBuffer", {settingsBuffer, sizeof(settingsBuffer)}},
    {"KeyBuffer", {KeyBuffer, sizeof(KeyBuffer)}},
    {"KeyBufferalt", {KeyBufferalt, sizeof(KeyBufferalt)}},
    {"ItemDesyncSlot", {ItemDesyncSlot, sizeof(ItemDesyncSlot)}},
    {"ItemSpeedSlot", {ItemSpeedSlot, sizeof(ItemSpeedSlot)}},
    {"ItemClipSlot", {ItemClipSlot, sizeof(ItemClipSlot)}},
    {"ItemClipDelay", {ItemClipDelay, sizeof(ItemClipDelay)}},
	{"BunnyHopDelayChar", {BunnyHopDelayChar, sizeof(BunnyHopDelayChar)}},
    {"RobloxSensValue", {RobloxSensValue, sizeof(RobloxSensValue)}},
    {"RobloxWallWalkValueChar", {RobloxWallWalkValueChar, sizeof(RobloxWallWalkValueChar)}},
    {"RobloxWallWalkValueDelayChar", {RobloxWallWalkValueDelayChar, sizeof(RobloxWallWalkValueDelayChar)}},
    {"WallhopPixels", {WallhopPixels, sizeof(WallhopPixels)}},
    {"SpamDelay", {SpamDelay, sizeof(SpamDelay)}},
    {"RobloxPixelValueChar", {RobloxPixelValueChar, sizeof(RobloxPixelValueChar)}},
    {"ChatKeyChar", {ChatKeyChar, sizeof(ChatKeyChar)}},
    {"CustomTextChar", {CustomTextChar, sizeof(CustomTextChar)}}
};

void SaveSettings(const std::string& filepath) {
    json settings;

    // Save booleans
    for (const auto& [key, ptr] : bool_vars) {
        settings[key] = *ptr;
    }

    // Save numerics
    for (const auto& [key, var] : numeric_vars) {
        std::visit([&](auto&& arg) { settings[key] = *arg; }, var);
    }

	// Save char arrays
    for (const auto &[key, cfg] : char_arrays) {
	settings[key] = TrimNullChars(cfg.first, cfg.second);
    }

    // Special cases
    settings["section_toggles"] = std::vector<bool>(section_toggles, section_toggles + section_amounts);
    settings["section_order_vector"] = std::vector<int>(section_order, section_order + section_amounts);
    settings["text"] = text;
    settings["screen_width"] = screen_width;
    settings["screen_height"] = screen_height;
    
    // Save theme data
    settings["current_theme_index"] = current_theme_index;
    settings["show_theme_editor"] = show_theme_editor;
    
    // Save custom theme if it exists
    if (current_theme_index == themes.size()) { // Custom theme is selected
        json custom_theme_json;
        custom_theme_json["name"] = custom_theme.name;
        custom_theme_json["bg_dark"] = {custom_theme.bg_dark.x, custom_theme.bg_dark.y, custom_theme.bg_dark.z, custom_theme.bg_dark.w};
        custom_theme_json["bg_medium"] = {custom_theme.bg_medium.x, custom_theme.bg_medium.y, custom_theme.bg_medium.z, custom_theme.bg_medium.w};
        custom_theme_json["bg_light"] = {custom_theme.bg_light.x, custom_theme.bg_light.y, custom_theme.bg_light.z, custom_theme.bg_light.w};
        custom_theme_json["accent_primary"] = {custom_theme.accent_primary.x, custom_theme.accent_primary.y, custom_theme.accent_primary.z, custom_theme.accent_primary.w};
        custom_theme_json["accent_secondary"] = {custom_theme.accent_secondary.x, custom_theme.accent_secondary.y, custom_theme.accent_secondary.z, custom_theme.accent_secondary.w};
        custom_theme_json["text_primary"] = {custom_theme.text_primary.x, custom_theme.text_primary.y, custom_theme.text_primary.z, custom_theme.text_primary.w};
        custom_theme_json["text_secondary"] = {custom_theme.text_secondary.x, custom_theme.text_secondary.y, custom_theme.text_secondary.z, custom_theme.text_secondary.w};
        custom_theme_json["success_color"] = {custom_theme.success_color.x, custom_theme.success_color.y, custom_theme.success_color.z, custom_theme.success_color.w};
        custom_theme_json["warning_color"] = {custom_theme.warning_color.x, custom_theme.warning_color.y, custom_theme.warning_color.z, custom_theme.warning_color.w};
        custom_theme_json["error_color"] = {custom_theme.error_color.x, custom_theme.error_color.y, custom_theme.error_color.z, custom_theme.error_color.w};
        custom_theme_json["border_color"] = {custom_theme.border_color.x, custom_theme.border_color.y, custom_theme.border_color.z, custom_theme.border_color.w};
        custom_theme_json["window_rounding"] = custom_theme.window_rounding;
        custom_theme_json["frame_rounding"] = custom_theme.frame_rounding;
        custom_theme_json["button_rounding"] = custom_theme.button_rounding;
        settings["custom_theme"] = custom_theme_json;
    }

    // Write to file
    std::ofstream file(filepath);
    file << settings.dump(4);
}

void LoadSettings(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) return;

        json settings;
        file >> settings;

        // Load booleans
        for (const auto& [key, ptr] : bool_vars) {
            if (settings.contains(key)) *ptr = settings[key];
        }

        // Load numerics
        for (const auto& [key, var] : numeric_vars) {
            if (!settings.contains(key)) continue;
            std::visit([&](auto&& arg) { *arg = settings[key]; }, var);
        }

		// Load char arrays
		for (const auto& [key, cfg] : char_arrays) {
			if (settings.contains(key)) {
				std::string str = settings[key];
				strncpy(cfg.first, str.c_str(), cfg.second);
				cfg.first[cfg.second - 1] = '\0'; // Ensure null-termination
			}
		}

        // Load special cases
        if (settings.contains("section_toggles")) {
            auto toggles = settings["section_toggles"].get<std::vector<bool>>();
            std::copy(toggles.begin(), toggles.end(), section_toggles);
        }

		if (settings.contains("section_order_vector")) {
			auto order = settings["section_order_vector"].get<std::vector<int>>();
    
			// HACKY SOLUTION TO ADD IN FUNCTIONS IN CUSTOM POSITONS BY DEFAULT!
			// Add in Bunnyhop Location to older save files
			if (std::find(order.begin(), order.end(), 13) == order.end() && order.size() >= 6) {
				order.insert(order.begin() + 7, 13);  // Insert at index 6 (7th element)
			}
    
			for (size_t i = 0; i < section_amounts && i < order.size(); ++i) {
				section_order[i] = order[i];
			}
		}

        if (settings.contains("text")) text = settings["text"];
        
        screen_width = settings.value("screen_width", screen_width);
        screen_height = settings.value("screen_height", screen_height);
        
        // Load theme data
        current_theme_index = settings.value("current_theme_index", 0);
        show_theme_editor = settings.value("show_theme_editor", false);
        
        // Load custom theme if it exists
        if (settings.contains("custom_theme") && current_theme_index == themes.size()) {
            json custom_theme_json = settings["custom_theme"];
            custom_theme.name = custom_theme_json.value("name", "Custom Theme");
            
            auto load_color = [](const json& j, const std::string& key) -> ImVec4 {
                if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
                    return ImVec4(j[key][0], j[key][1], j[key][2], j[key][3]);
                }
                return ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            };
            
            custom_theme.bg_dark = load_color(custom_theme_json, "bg_dark");
            custom_theme.bg_medium = load_color(custom_theme_json, "bg_medium");
            custom_theme.bg_light = load_color(custom_theme_json, "bg_light");
            custom_theme.accent_primary = load_color(custom_theme_json, "accent_primary");
            custom_theme.accent_secondary = load_color(custom_theme_json, "accent_secondary");
            custom_theme.text_primary = load_color(custom_theme_json, "text_primary");
            custom_theme.text_secondary = load_color(custom_theme_json, "text_secondary");
            custom_theme.success_color = load_color(custom_theme_json, "success_color");
            custom_theme.warning_color = load_color(custom_theme_json, "warning_color");
            custom_theme.error_color = load_color(custom_theme_json, "error_color");
            custom_theme.border_color = load_color(custom_theme_json, "border_color");
            custom_theme.window_rounding = custom_theme_json.value("window_rounding", 8.0f);
            custom_theme.frame_rounding = custom_theme_json.value("frame_rounding", 4.0f);
            custom_theme.button_rounding = custom_theme_json.value("button_rounding", 4.0f);
        }

    } catch (const json::exception& e) {
        std::cerr << "Load error: " << e.what() << '\n';
    }
}

// Define sections

struct Section {
    std::string title;
    std::string description;
    bool optionA; 
    float settingValue; 
};

struct SectionConfig {
    const char *title;
    const char *description;
};

std::vector<Section> sections;

// Title + Description of Sections

constexpr std::array<SectionConfig, section_amounts> SECTION_CONFIGS = {{
    {"Freeze", "Automatically Tab Glitch With a Button"},
    {"Item Desync", "Enable Item Collision (Hold Item Before Pressing)"},
    {"Helicopter High Jump", "Use COM Offset to Catapult Yourself Into The Air by Aligning your Back Angled to the Wall and Jumping and Letting Your Character Turn"},
    {"Speedglitch", "Use COM offset to Massively Increase Midair Speed"},
    {"Item Unequip COM Offset", "Automatically Do a /e dance2 Item COM Offset Where You Unequip the Item"},
    {"Press a Button", "Whenever You Press Your Keybind, it Presses the Other Button for One Frame"},
    {"Wallhop/Rotation", "Automatically Flick and Jump to easily Wallhop On All FPS"},
    {"Walless LHJ", "Lag High Jump Without a Wall by Offsetting COM Downwards or to the Right"},
    {"Item Clip", "Clip through 2-3 Stud Walls Using Gears"},
    {"Laugh Clip", "Automatically Perform a Laugh Clip"},
    {"Wall-Walk", "Walk Across Wall Seams Without Jumping"},
    {"Spam a Key", "Whenever You Press Your Keybind, it Spams the Other Button"},
    {"Ledge Bounce", "Briefly Falls off a Ledge to Then Bounce Off it While Falling"},
    {"Smart Bunnyhop", "Intelligently enables or disables Bunnyhop for any Key"}
}};

// ADD KEYBIND VARIABLES FOR EACH SECTION
static const std::unordered_map<unsigned int, unsigned int *> section_to_key = {
	{0, &vk_mbutton},   {1, &vk_f5},       {2, &vk_xbutton1}, {3, &vk_xkey},
	{4, &vk_f8},        {5, &vk_zkey},     {6, &vk_xbutton2}, {7, &vk_f6},
	{8, &vk_clipkey},   {9, &vk_laughkey}, {10, &vk_wallkey}, {11, &vk_leftbracket},
	{12, &vk_bouncekey}, {13, &vk_bunnyhopkey}};

static void InitializeSections()
{
    sections.clear();
    for (size_t i = 0; i < SECTION_CONFIGS.size(); ++i) {
	sections.push_back({
		SECTION_CONFIGS[i].title, SECTION_CONFIGS[i].description,
		false, // Fallback Option1
		50.0f  // Fallback Option2
	});
    }
}


static unsigned int BindKeyMode(unsigned int currentkey)
{
    static int lastSelectedSection = -1; // Initialize with a value that won't match any valid section

    if (bindingMode) {
		rebindtime = std::chrono::high_resolution_clock::now();
        for (int key = 0; key < 255; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                bindingMode = false;
                std::string currentkeystr = std::format("{:02x}", key); // Convert key into string
                unsigned int currentkeyint = std::stoul(currentkeystr, nullptr, 16); // Convert string into unsigned int
                std::snprintf(KeyBuffer, sizeof(KeyBuffer), "0x%02x", currentkeyint); // Update KeyBuffer text to the key
                return currentkeyint;
            }
        }
    } else { 
        // Check if the selected_section has changed
        if (selected_section != lastSelectedSection) {
            std::snprintf(KeyBuffer, sizeof(KeyBuffer), "0x%02x", currentkey); // Update KeyBuffer for the new section
            lastSelectedSection = selected_section; // Update lastSelectedSection to the current
        }
        
        KeyButtonText = "Click to Bind Key";

        // Don't crash if value is invalid
        try {
			currentkey = std::stoul(KeyBuffer, nullptr, 16);
		} catch (const std::invalid_argument& e) {
			currentkey = 0;
		} catch (const std::out_of_range& e) {
			currentkey = 0;
		}
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedtime = currentTime - rebindtime;

        if (elapsedtime.count() >= 0.2) { // .2 Second Delay between binds
            notbinding = true;
        }
        return currentkey;
    }
}


static unsigned int BindKeyModeAlt(unsigned int currentkey)
{
    static int lastSelectedSection = -1; // Initialize with a value that won't match any valid section

    if (bindingModealt) {
		rebindtime = std::chrono::high_resolution_clock::now();
        for (int key = 0; key < 255; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                bindingModealt = false;
                std::string currentkeystr = std::format("{:02x}", key); // Convert key into string
                unsigned int currentkeyint = std::stoul(currentkeystr, nullptr, 16); // Convert string into unsigned int
                std::snprintf(KeyBufferalt, sizeof(KeyBufferalt), "0x%02x", currentkeyint); // Update KeyBuffer text to your key
                return currentkeyint;
            }
        }
    } else { 
        // Check if the selected_section has changed
        if (selected_section != lastSelectedSection) {
            std::snprintf(KeyBufferalt, sizeof(KeyBufferalt), "0x%02x", currentkey); // Update KeyBuffer for the new section
            lastSelectedSection = selected_section; // Update lastSelectedSection to the current
        }
        
        KeyButtonTextalt = "Click to Bind Key";

		// Don't crash if value is invalid
		try {
			currentkey = std::stoul(KeyBufferalt, nullptr, 16);
		} catch (const std::invalid_argument& e) {
			currentkey = 0;
		} catch (const std::out_of_range& e) {
			currentkey = 0;
		}
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedtime = currentTime - rebindtime;

        if (elapsedtime.count() >= 0.2) { // .2 Second Delay between binds
            notbinding = true;
        }
        return currentkey;
    }
}


static void GetKeyNameFromHex(unsigned int hexKeyCode)
{
    // Clear the buffer
    memset(KeyBufferhuman, 0, 256);

    // Map the virtual key code to a scan code
    UINT scanCode = MapVirtualKey(hexKeyCode, MAPVK_VK_TO_VSC);

	if (bindingMode) { // Turn off Key-Finding while binding
	    return;
	}

    // Attempt to get the readable key name
    if (GetKeyNameTextA(scanCode << 16, KeyBufferhuman, 256) > 0) {
        // Successfully retrieved the key name
        return; // No further action needed
    } else {
        // If GetKeyNameText fails, try to find the VK_ name
        auto it = vkToString.find(hexKeyCode);
        if (it != vkToString.end()) {
            strncpy(KeyBufferhuman, it->second.c_str(), 256);
        } else {
            // If not found, return a default hex representation
            snprintf(KeyBufferhuman, 256, "0x%X", hexKeyCode);
        }
    }
}

static void GetKeyNameFromHexAlt(unsigned int hexKeyCode)
{
    // Clear the buffer
    memset(KeyBufferhumanalt, 0, 256);

    // Map the virtual key code to a scan code
    UINT scanCode = MapVirtualKey(hexKeyCode, MAPVK_VK_TO_VSC);

	if (bindingModealt) { // Turn off Key-Finding while binding
	    return;
	}

    // Attempt to get the readable key name
	if (GetKeyNameTextA(scanCode << 16, KeyBufferhumanalt, 256) > 0) {
        // Successfully retrieved the key name
        return; // No further action needed
    } else {
        // If GetKeyNameText fails, try to find the VK_ name
        auto it = vkToString.find(hexKeyCode);
        if (it != vkToString.end()) {
            strncpy(KeyBufferhumanalt, it->second.c_str(), 256);
        } else {
            // If not found, return a default hex representation
            snprintf(KeyBufferhumanalt, 256, "0x%X", hexKeyCode);
        }
    }
}

UINT ChatKeyCharToVK(const char* input) {
    if (!input || strlen(input) == 0) {
        return 0;
    }

    wchar_t wideChar;
    if (MultiByteToWideChar(CP_ACP, 0, input, 1, &wideChar, 1) == 0) {
        return 0;
    }

    HKL keyboardLayout = GetKeyboardLayout(0);
    
    SHORT vkAndShift = VkKeyScanExW(wideChar, keyboardLayout);
    if (vkAndShift == -1) {
        return 0; // Character not available
    }

    // Extract the virtual key code (low byte)
    return static_cast<UINT>(LOBYTE(vkAndShift));
}

unsigned int vk_chatkey = ChatKeyCharToVK(ChatKeyChar);

// Make the Title Bar Black
static bool SetTitleBarColor(HWND hwnd, COLORREF color) {
    BOOL value = TRUE;
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    if (FAILED(hr)) {
        return false;
    }
    hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
    if (FAILED(hr)) {
        return false;
    }
    return true;
}
// START OF PROGRAM
static void RunGUI()
{
	// Run timers with max precision
    timeBeginPeriod(1);

	// I LOVE THREAD PRIORITY!!!!!!!!!!!!!!!
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		
	// Initialize a basic Win32 window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Roblox Macro Client"), NULL };

	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, _T("Roblox Macro Client"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	SetTitleBarColor(hwnd, RGB(0, 0, 0));
	SetWindowPos(hwnd, NULL, 0, 0, screen_width, screen_height, SWP_NOZORDER | SWP_NOMOVE);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

	// Show the window
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);

    float alphaFraction = windowOpacityPercent / 100.0f;
    BYTE alphaByte = static_cast<BYTE>(alphaFraction * 255.0f);
    SetLayeredWindowAttributes(hwnd, 0, alphaByte, LWA_ALPHA);

	if (ontoptoggle) {
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	HRSRC hRes = FindResource(NULL, TEXT("LSANS_TTF"), RT_RCDATA);
    HGLOBAL hMem = LoadResource(NULL, hRes);
	LPVOID pData = LockResource(hMem);
    DWORD size = SizeofResource(NULL, hRes);
	ImFont *mainfont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(pData, size, 18.0f);

    // Initialize ImGui for Win32 and DirectX 11
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    auto lastTime = std::chrono::high_resolution_clock::now();
    float targetFrameTime = 1.0f / 90.0f;

	InitializeSections();

	MSG msg;
	vk_chatkey = ChatKeyCharToVK(ChatKeyChar);

    // Attach the GUI thread to the input of the main thread
    DWORD mainThreadId = GetWindowThreadProcessId(hwnd, NULL);
    DWORD guiThreadId = GetCurrentThreadId();
    AttachThreadInput(mainThreadId, guiThreadId, TRUE);

    while (running) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;

        if (deltaTime.count() >= targetFrameTime) {
            lastTime = currentTime;

            // Start ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImVec2 display_size = ImGui::GetIO().DisplaySize;

            // Apply current theme
            const Theme& current_theme = (current_theme_index < themes.size()) ? themes[current_theme_index] : custom_theme;
            ApplyTheme(current_theme);

            // Main window
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(display_size, ImGuiCond_Always);
            
            ImGui::Begin("##MainWindow", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNav);

            // Header section
            ImGui::BeginChild("Header", ImVec2(display_size.x - 32, 80), true);
            
            // Title and status
            ImGui::PushFont(mainfont);
            ImGui::TextColored(current_theme.accent_primary, "ROBLOX MACRO UTILITIES");
            ImGui::PopFont();
            
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
            
            // Status indicator
            ImGui::PushStyleColor(ImGuiCol_Text, macrotoggled ? current_theme.success_color : current_theme.error_color);
            ImGui::Text(macrotoggled ? "ACTIVE" : "INACTIVE");
            ImGui::PopStyleColor();
            
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
            
            // Process status
            ImGui::Text("Process: ");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, processFound ? current_theme.success_color : current_theme.error_color);
            ImGui::Text(processFound ? "Connected" : "Not Found");
            ImGui::PopStyleColor();
            
            // Version info
            ImGui::SameLine(ImGui::GetWindowWidth() - 200);
            if (UserOutdated) {
                ImGui::PushStyleColor(ImGuiCol_Text, current_theme.warning_color);
                ImGui::Text("UPDATE AVAILABLE");
                ImGui::PopStyleColor();
            } else {
                ImGui::TextColored(current_theme.text_secondary, "v2.9.92");
            }
            
            // Quick controls
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            
            ImGui::Checkbox("Macro Toggle", &macrotoggled);
            ImGui::SameLine();
            ImGui::Checkbox("Anti-AFK", &antiafktoggle);
            ImGui::SameLine();
            ImGui::Checkbox("Always On Top", &ontoptoggle);
            ImGui::SameLine();
            
            // Opacity slider
            ImGui::Text("Opacity:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            if (ImGui::SliderFloat("##Opacity", &windowOpacityPercent, 20.0f, 100.0f, "%.0f%%")) {
                float alphaFraction = windowOpacityPercent / 100.0f;
                BYTE alphaByte = static_cast<BYTE>(alphaFraction * 255.0f);
                SetLayeredWindowAttributes(hwnd, 0, alphaByte, LWA_ALPHA);
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Save Settings")) {
                SaveSettings("RMCSettings.json");
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Theme Editor")) {
                show_theme_editor = !show_theme_editor;
            }
            
            ImGui::EndChild();

            // Global Settings section
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
            ImGui::BeginChild("GlobalSettings", ImVec2(display_size.x - 32, 120), true);
            
            ImGui::TextColored(current_theme.accent_primary, "GLOBAL SETTINGS");
            ImGui::Separator();
            ImGui::Spacing();
            
            // First row
            ImGui::Text("Roblox Executable:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("##RobloxExe", settingsBuffer, sizeof(settingsBuffer), ImGuiInputTextFlags_CharsNoBlank);
            
            ImGui::SameLine();
            ImGui::Text("Sensitivity:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            if (ImGui::InputText("##Sensitivity", RobloxSensValue, sizeof(RobloxSensValue), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank)) {
                PreviousSensValue = -1;
            }
            
            ImGui::SameLine();
            ImGui::Text("Cam-Fix:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##CamFix", &camfixtoggle) || PreviousSensValue == -1) {
                wallhopupdate = false;
                if (PreviousSensValue != -1) {
                    wallhopupdate = true;
                }
                PreviousSensValue = -1;
                PreviousWallWalkValue = -1;
                
                try {
                    if (wallhopupdate) {
                        float factor = camfixtoggle ? 1.388888889f : 1.0f / 1.388888889f;
                        if (wallhopswitch) {
                            wallhop_dx = std::round(std::stoi(WallhopPixels) * (camfixtoggle ? -factor : factor));
                            wallhop_dy = std::round(std::stoi(WallhopPixels) * (camfixtoggle ? factor : -factor));
                        } else {
                            wallhop_dx = std::round(std::stoi(WallhopPixels) * factor);
                            wallhop_dy = std::round(std::stoi(WallhopPixels) * -factor);
                            sprintf(WallhopPixels, "%d", wallhop_dx);
                        }
                    }
                } catch (...) {}
                
                float CurrentWallWalkValue = atof(RobloxSensValue);
                float baseValue = camfixtoggle ? 500.0f : 360.0f;
                wallwalk_strengthx = -static_cast<int>(std::round((baseValue / CurrentWallWalkValue) * 0.13f));
                wallwalk_strengthy = static_cast<int>(std::round((baseValue / CurrentWallWalkValue) * 0.13f));
                sprintf(RobloxWallWalkValueChar, "%d", wallwalk_strengthx);
                
                float CurrentSensValue = atof(RobloxSensValue);
                try {
                    float baseValue = camfixtoggle ? 500.0f : 360.0f;
                    float multiplier = (359.0f / 360.0f) * (359.0f / 360.0f);
                    RobloxPixelValue = static_cast<int>(std::round((baseValue / CurrentSensValue) * multiplier));
                } catch (...) {}
                
                PreviousSensValue = CurrentSensValue;
                sprintf(RobloxPixelValueChar, "%d", RobloxPixelValue);
                try {
                    chatkey = ChatKeyChar;
                    speed_strengthx = std::stoi(RobloxPixelValueChar);
                    speed_strengthy = -std::stoi(RobloxPixelValueChar);
                } catch (...) {}
            }
            
            // Second row
            ImGui::Text("Chat Override:");
            ImGui::SameLine();
            ImGui::Checkbox("##ChatOverride", &chatoverride);
            
            ImGui::SameLine();
            ImGui::Text("Shift to Control:");
            ImGui::SameLine();
            ImGui::Checkbox("##ShiftSwitch", &shiftswitch);
            
            if (shiftswitch) {
                scancode_shift = 0x1D;
            } else {
                scancode_shift = 0x2A;
            }
            
            ImGui::SameLine();
            ImGui::Text("Chat Key:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ChatKeyGlobal", ChatKeyChar, sizeof(ChatKeyChar), ImGuiInputTextFlags_CharsNoBlank)) {
                if (strlen(ChatKeyChar) > 1) {
                    ChatKeyChar[1] = '\0';
                }
                vk_chatkey = ChatKeyCharToVK(ChatKeyChar);
            }
            
            ImGui::EndChild();

            // Main content area
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
            
            // Two-column layout
            float left_panel_width = display_size.x * 0.35f - 16;
            float right_panel_width = display_size.x - left_panel_width - 32;
            
            // Left panel - Macro list
            ImGui::BeginChild("MacroList", ImVec2(left_panel_width, display_size.y - 240), true);
            
            ImGui::PushFont(mainfont);
            ImGui::TextColored(current_theme.accent_primary, "MACROS");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();
            
            // Macro buttons with modern styling
            for (size_t display_index = 0; display_index < section_amounts; ++display_index) {
                int i = section_order[display_index];
                
                ImGui::PushID(i);
                
                // Button styling based on state
                if (section_toggles[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Button, current_theme.accent_primary);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_theme.accent_secondary);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, current_theme.bg_light);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(current_theme.bg_light.x + 0.05f, current_theme.bg_light.y + 0.05f, current_theme.bg_light.z + 0.05f, 1.0f));
                }
                
                // Calculate button size with proper text fitting
                float buttonWidth = left_panel_width * 0.85f; // Reduce width to 85% of panel
                float padding = 8.0f; // Reduced padding from 16.0f to 8.0f
                float availableWidth = buttonWidth - (padding * 2);
                
                // Calculate text sizes with wrapping
                ImVec2 titleSize = ImGui::CalcTextSize(sections[i].title.c_str());
                ImVec2 descSize = ImGui::CalcTextSize(sections[i].description.c_str());
                
                // Check if text needs wrapping
                std::string wrappedTitle = sections[i].title;
                std::string wrappedDesc = sections[i].description;
                
                if (titleSize.x > availableWidth) {
                    // Wrap title text
                    wrappedTitle = "";
                    std::string currentLine = "";
                    std::istringstream iss(sections[i].title);
                    std::string word;
                    while (iss >> word) {
                        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
                        ImVec2 testSize = ImGui::CalcTextSize(testLine.c_str());
                        if (testSize.x > availableWidth && !currentLine.empty()) {
                            wrappedTitle += currentLine + "\n";
                            currentLine = word;
                        } else {
                            currentLine = testLine;
                        }
                    }
                    wrappedTitle += currentLine;
                    titleSize = ImGui::CalcTextSize(wrappedTitle.c_str());
                }
                
                if (descSize.x > availableWidth) {
                    // Wrap description text
                    wrappedDesc = "";
                    std::string currentLine = "";
                    std::istringstream iss(sections[i].description);
                    std::string word;
                    while (iss >> word) {
                        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
                        ImVec2 testSize = ImGui::CalcTextSize(testLine.c_str());
                        if (testSize.x > availableWidth && !currentLine.empty()) {
                            wrappedDesc += currentLine + "\n";
                            currentLine = word;
                        } else {
                            currentLine = testLine;
                        }
                    }
                    wrappedDesc += currentLine;
                    descSize = ImGui::CalcTextSize(wrappedDesc.c_str());
                }
                
                // Calculate total button height - reduced spacing
                float buttonHeight = titleSize.y + descSize.y + padding * 2 + 4; // Reduced spacing from 8 to 4
                
                if (ImGui::Button("", ImVec2(buttonWidth, buttonHeight))) {
                    selected_section = i;
                }
                
                // Drag and drop
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("DND_SECTION", &display_index, sizeof(int));
                    ImGui::EndDragDropSource();
                }
                
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_SECTION")) {
                        int payload_index = *(const int *)payload->Data;
                        std::swap(section_order[payload_index], section_order[display_index]);
                    }
                    ImGui::EndDragDropTarget();
                }
                
                // Custom text rendering with proper positioning
                ImVec2 buttonPos = ImGui::GetItemRectMin();
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                
                // Render title
                ImVec2 titlePos = ImVec2(buttonPos.x + padding, buttonPos.y + padding);
                drawList->AddText(titlePos, IM_COL32(255, 255, 255, 255), wrappedTitle.c_str());
                
                // Render description - reduced spacing
                ImVec2 descPos = ImVec2(buttonPos.x + padding, titlePos.y + titleSize.y + 2); // Reduced from 8 to 2
                drawList->AddText(descPos, IM_COL32(200, 200, 200, 200), wrappedDesc.c_str());
                
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                
                ImGui::Spacing();
                ImGui::Spacing(); // Reduced from multiple ImGui::Spacing() calls to just one
            }
            
            ImGui::EndChild();
            
            // Right panel - Settings
            ImGui::SameLine();
            ImGui::BeginChild("SettingsPanel", ImVec2(right_panel_width, display_size.y - 240), true);
            
            if (selected_section >= 0 && selected_section < sections.size()) {
                // Section header
                ImGui::PushFont(mainfont);
                ImGui::TextColored(current_theme.accent_primary, "%s", sections[selected_section].title.c_str());
                ImGui::PopFont();
                ImGui::TextColored(current_theme.text_secondary, "%s", sections[selected_section].description.c_str());
                ImGui::Separator();
                ImGui::Spacing();
                
                // Toggle for this section
                ImGui::Checkbox("Enable This Macro", &section_toggles[selected_section]);
                ImGui::Spacing();
                
                // Keybind section
                ImGui::TextColored(current_theme.text_primary, "KEYBIND SETTINGS");
                ImGui::Spacing();
                
                ImGui::Text("Primary Key:");
                ImGui::SameLine();
                
                if (ImGui::Button(KeyButtonText.c_str(), ImVec2(120, 30))) {
                    notbinding = false;
                    bindingMode = true;
                    KeyButtonText = "Press a Key...";
                }
                
                ImGui::SameLine();
                
                // Handle key bindings
                if (section_to_key.count(selected_section)) {
                    unsigned int* key = section_to_key.at(selected_section);
                    *key = BindKeyMode(*key);
                    GetKeyNameFromHex(*key);
                }
                
                ImGui::SetNextItemWidth(150);
                ImGui::InputText("##KeyName", KeyBufferhuman, sizeof(KeyBufferhuman), ImGuiInputTextFlags_ReadOnly);
                
                ImGui::SameLine();
                ImGui::TextColored(current_theme.text_secondary, "(Human Readable)");
                
                ImGui::SetNextItemWidth(80);
                ImGui::InputText("##KeyHex", KeyBuffer, sizeof(KeyBuffer), ImGuiInputTextFlags_CharsNoBlank);
                
                ImGui::SameLine();
                ImGui::TextColored(current_theme.text_secondary, "(Hex)");
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Section-specific settings
                ImGui::TextColored(current_theme.text_primary, "MACRO SETTINGS");
                ImGui::Spacing();
                
                // Render section-specific UI
                RenderSectionSettings(selected_section);
                
            } else {
                ImGui::TextColored(current_theme.text_secondary, "Select a macro from the left panel to configure its settings.");
            }
            
            ImGui::EndChild();
            
            ImGui::End();
            
            // Render theme editor
            RenderThemeEditor();
            
            // Render
            ImGui::Render();
            const float clear_color[] = { 0.08f, 0.08f, 0.12f, 1.00f };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
    
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    AttachThreadInput(mainThreadId, guiThreadId, FALSE);
}

// Helper function to render section-specific settings
static void RenderSectionSettings(int section) {
    const Theme& current_theme = (current_theme_index < themes.size()) ? themes[current_theme_index] : custom_theme;
    
    switch (section) {
        case 0: // Freeze
            ImGui::Text("Freeze Duration (seconds):");
            ImGui::SetNextItemWidth(200);
            ImGui::SliderFloat("##FreezeTime", &maxfreezetime, 0.0f, 9.8f, "%.2f s");
            
            ImGui::Text("Unfreeze Override (ms):");
            ImGui::SetNextItemWidth(200);
            ImGui::InputInt("##FreezeOverride", &maxfreezeoverride);
            
            ImGui::Spacing();
            ImGui::Checkbox("Freeze Outside Roblox", &freezeoutsideroblox);
            ImGui::Checkbox("Toggle Mode (instead of hold)", &isfreezeswitch);
            ImGui::Checkbox("Freeze All Processes", &takeallprocessids);
            break;
            
        case 1: // Item Desync
            ImGui::Text("Gear Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##DesyncSlot", ItemDesyncSlot, sizeof(ItemDesyncSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                desync_slot = std::stoi(ItemDesyncSlot);
            } catch (...) {}
            break;
            
        case 2: // HHJ
            ImGui::Checkbox("Auto Timing", &autotoggle);
            ImGui::Checkbox("Fast Mode (Speedrunning)", &fasthhj);
            break;
            
        case 3: // Speedglitch
            ImGui::Text("Pixel Value (180� turn):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpeedPixels", RobloxPixelValueChar, sizeof(RobloxPixelValueChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                speed_strengthx = std::stoi(RobloxPixelValueChar);
                speed_strengthy = -std::stoi(RobloxPixelValueChar);
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isspeedswitch);
            break;
            
        case 4: // Gear Unequip
            ImGui::Text("Gear Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##SpeedSlot", ItemSpeedSlot, sizeof(ItemSpeedSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                speed_slot = std::stoi(ItemSpeedSlot);
            } catch (...) {}
            
            ImGui::Text("Custom Chat Message:");
            ImGui::SetNextItemWidth(300);
            ImGui::InputText("##CustomText", CustomTextChar, sizeof(CustomTextChar));
            
            ImGui::Text("Chat Key:");
            ImGui::SetNextItemWidth(50);
            ImGui::InputText("##ChatKey", ChatKeyChar, sizeof(ChatKeyChar), ImGuiInputTextFlags_CharsNoBlank);
            
            ImGui::Spacing();
            ImGui::Checkbox("Keep Item Equipped", &unequiptoggle);
            break;
            
        case 5: // Press Key
            ImGui::Text("Secondary Key:");
            ImGui::SameLine();
            if (ImGui::Button((KeyButtonTextalt + "##").c_str(), ImVec2(120, 30))) {
                notbinding = false;
                bindingModealt = true;
                KeyButtonTextalt = "Press a Key...";
            }
            ImGui::SameLine();
            vk_dkey = BindKeyModeAlt(vk_dkey);
            GetKeyNameFromHexAlt(vk_dkey);
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SecondaryKey", KeyBufferhumanalt, sizeof(KeyBufferhumanalt), ImGuiInputTextFlags_ReadOnly);
            break;
            
        case 6: // Wallhop
            ImGui::Text("Flick Pixels:");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##WallhopPixels", WallhopPixels, sizeof(WallhopPixels), ImGuiInputTextFlags_CharsDecimal);
            try {
                wallhop_dx = std::round(std::stoi(WallhopPixels));
                wallhop_dy = -std::round(std::stoi(WallhopPixels));
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Left-Flick Mode", &wallhopswitch);
            ImGui::Checkbox("Auto Jump", &toggle_jump);
            ImGui::Checkbox("Auto Flick-Back", &toggle_flick);
            break;
            
        case 7: // Walless LHJ
            ImGui::Checkbox("Left-Sided LHJ", &wallesslhjswitch);
            break;
            
        case 8: // Item Clip
            ImGui::Text("Item Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##ClipSlot", ItemClipSlot, sizeof(ItemClipSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                clip_slot = std::stoi(ItemClipSlot);
            } catch (...) {}
            
            ImGui::Text("Clip Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##ClipDelay", ItemClipDelay, sizeof(ItemClipDelay), ImGuiInputTextFlags_CharsDecimal);
            try {
                clip_delay = std::stoi(ItemClipDelay);
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isitemclipswitch);
            break;
            
        case 9: // Laugh Clip
            ImGui::Checkbox("Disable S Key (Weaker but less intrusive)", &laughmoveswitch);
            break;
            
        case 10: // Wall-Walk
            ImGui::Text("Wall-Walk Pixels:");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##WallWalkPixels", RobloxWallWalkValueChar, sizeof(RobloxWallWalkValueChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                wallwalk_strengthx = std::stoi(RobloxWallWalkValueChar);
                wallwalk_strengthy = -std::stoi(RobloxWallWalkValueChar);
            } catch (...) {}
            
            ImGui::Text("Flick Delay (�s):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##WallWalkDelay", RobloxWallWalkValueDelayChar, sizeof(RobloxWallWalkValueDelayChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                RobloxWallWalkValueDelay = atof(RobloxWallWalkValueDelayChar);
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Left-Flick Mode", &wallwalktoggleside);
            ImGui::Checkbox("Hold Mode", &iswallwalkswitch);
            break;
            
        case 11: // Spam Key
            ImGui::Text("Key to Spam:");
            ImGui::SameLine();
            if (ImGui::Button((KeyButtonTextalt + "##").c_str(), ImVec2(120, 30))) {
                bindingModealt = true;
                notbinding = false;
                KeyButtonTextalt = "Press a Key...";
            }
            ImGui::SameLine();
            vk_spamkey = BindKeyModeAlt(vk_spamkey);
            GetKeyNameFromHexAlt(vk_spamkey);
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpamKey", KeyBufferhumanalt, sizeof(KeyBufferhumanalt), ImGuiInputTextFlags_ReadOnly);
            
            ImGui::Text("Spam Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpamDelay", SpamDelay, sizeof(SpamDelay), ImGuiInputTextFlags_CharsDecimal);
            try {
                spam_delay = std::stof(SpamDelay);
                real_delay = static_cast<int>((spam_delay * 1000.0f + 0.5f) / 2);
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isspamswitch);
            break;
            
        case 12: // Ledge Bounce
            ImGui::Checkbox("Left-Sided Bounce", &bouncesidetoggle);
            ImGui::Checkbox("Stay Horizontal After Bounce", &bouncerealignsideways);
            ImGui::Checkbox("Auto Hold Movement", &bounceautohold);
            break;
            
        case 13: // Bunnyhop
            ImGui::Text("Bunnyhop Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##BhopDelay", BunnyHopDelayChar, sizeof(BunnyHopDelayChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                BunnyHopDelay = atof(BunnyHopDelayChar);
            } catch (...) {}
            
            ImGui::Spacing();
            ImGui::Checkbox("Smart Mode (Auto-disable in chat)", &bunnyhopsmart);
            break;
    }
}


static void SetWorkingDirectoryToExecutablePath() // Allows non-standard execution for save files
{
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(nullptr, exePath, MAX_PATH)) {
	// Remove the executable name to get the directory
	char *lastSlash = strrchr(exePath, '\\');
	if (lastSlash) {
	    *lastSlash = '\0'; // Terminate the string at the last backslash
	    SetCurrentDirectoryA(exePath);
		}
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	// Setup Keyboard hook for Bhop Only
	HINSTANCE hMod = GetModuleHandle(NULL);
	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);

	// Load Settings
    SetWorkingDirectoryToExecutablePath();
    LoadSettings("RMCSettings.json");

	// Check for any updates
    std::string remoteVersion = GetRemoteVersion();
    if (remoteVersion == "") {
		remoteVersion = "HTTP request for latest version failed!";
    }
	remoteVersion = Trim(remoteVersion);
    std::string localVersion = "2.9.92";

	if (remoteVersion != localVersion) {
		UserOutdated = true;
    }

    if (remoteVersion != localVersion && !UserAcknowledgedV2992) {
		std::wstring remote_version = std::wstring(remoteVersion.begin(), remoteVersion.end());
		std::wstring local_version = std::wstring(localVersion.begin(), localVersion.end());
        std::wstring message = L"Your Version is Outdated! The latest version is: " + remote_version + L". Your version is: " + local_version + L". \nDo you understand this? If you press yes, this won't show up again.";
        
        int result = MessageBox(NULL, 
            message.c_str(), 
            L"Update Notification", 
            MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON1 | MB_APPLMODAL);

        if (result == IDYES) {
            UserAcknowledgedV2992 = true;  // No more notifications
        }
    }

	// Setup suspension
	const HMODULE hNtdll = GetModuleHandleA("ntdll");
	NtSuspendProcess pfnSuspend = reinterpret_cast<NtSuspendProcess>(GetProcAddress(hNtdll, "NtSuspendProcess"));
	NtResumeProcess pfnResume = reinterpret_cast<NtResumeProcess>(GetProcAddress(hNtdll, "NtResumeProcess"));
	

	std::thread actionThread(Speedglitchloop); // Start a separate thread for item desync loop, lets functions run alongside
	std::thread actionThread2(ItemDesyncLoop);
	std::thread actionThread3(SpeedglitchloopHHJ);
	std::thread actionThread4(SpamKeyLoop);
	std::thread actionThread5(ItemClipLoop);
	std::thread actionThread6(WallWalkLoop);
	std::thread actionThread7(BhopLoop);
	std::thread guiThread(RunGUI);
	MSG msg;

	std::vector<HANDLE> hProcess = GetProcessHandles(GetProcessIdByName(takeallprocessids), PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME);
	std::vector<HWND> rbxhwnd = FindWindowByProcessHandle(hProcess); // SET ROBLOX WINDOW HWND RAHHHHH

	// These variables are used for "one-click" functionalies for macros, so you can press a key and it runs every time that key is pressed (without overlapping itself)
	bool isdesync = false;
	bool isSuspended = false; 
	bool islhj = false;
	bool ispressd = false;
	bool iswallhop = false;
	bool isspeedglitch = false;
	bool isunequipspeed = false;
	bool HHJ = false;
	bool isspam = false;
	bool isclip = false;
	bool iswallwalk = false;
	bool islaugh = false;
	bool isbounce = false;
	bool isbhop = false;
	bool isafk = false;
	bool bhoplocked = false;
	auto lastPressTime = std::chrono::steady_clock::now();
	auto lastProcessCheck = std::chrono::steady_clock::now();
	static const float targetFrameTime = 1.0f / 90.0f; // Targeting 90 FPS
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!done) {
    {
		// Freeze
		if ((macrotoggled && notbinding && section_toggles[0])) {
			bool isMButtonPressed = GetAsyncKeyState(vk_mbutton) & 0x8000;

			if (isfreezeswitch) {  // Toggle mode
				if (isMButtonPressed && !wasMButtonPressed && (freezeoutsideroblox || IsForegroundWindowProcess(hProcess))) {  // Detect button press edge
					isSuspended = !isSuspended;  // Toggle the freeze state
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, isSuspended);

					if (isSuspended) {
						suspendStartTime = std::chrono::steady_clock::now();  // Start the timer
					}
				}
			} else {  // Hold mode
				if (isMButtonPressed && (freezeoutsideroblox || IsForegroundWindowProcess(hProcess))) {
					if (!isSuspended) {
						SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);  // Freeze on hold
						isSuspended = true;
						suspendStartTime = std::chrono::steady_clock::now();  // Start the timer
					}
				} else if (isSuspended) {
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);  // Unfreeze on release
					isSuspended = false;
				}
			}

			// Common timer logic for both toggle and hold modes
			if (isSuspended) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - suspendStartTime).count();

				if (elapsed >= (maxfreezetime * 1000)) {
					// Unsuspend for 50 ms
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);
					std::this_thread::sleep_for(std::chrono::milliseconds(maxfreezeoverride));
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);

					// Reset the timer
					suspendStartTime = std::chrono::steady_clock::now();
				}
			}

			// Update the previous state
			wasMButtonPressed = isMButtonPressed;
		}

		// Item Desync Macro with anti-idiot design
		if ((GetAsyncKeyState(vk_f5) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[1]) {
			if (!isdesync) {
				isdesyncloop = true;
				isdesync = true;
			}
		} else {
			isdesync = false;
			isdesyncloop = false;
		}

		// PressKey
		if ((GetAsyncKeyState(vk_zkey) & 0x8000) && macrotoggled && notbinding && section_toggles[5]) {
			if (!ispressd) {
				if (vk_zkey == vk_dkey) {
					ReleaseKeyBinded(vk_zkey);
				}

				HoldKeyBinded(vk_dkey);
				std::this_thread::sleep_for(std::chrono::milliseconds(6));
				ReleaseKeyBinded(vk_dkey);
				ispressd = true;
			}
		} else {
			ispressd = false;
		}

		// Wallhop
		if ((GetAsyncKeyState(vk_xbutton2) & 0x8000) && macrotoggled && notbinding && section_toggles[6]) {
			if (!iswallhop) {

				if (toggle_jump) {
					HoldKey(0x39);
				}

				if (wallhopswitch) {
					MoveMouse(-wallhop_dx, 0);
				} else {
					MoveMouse(wallhop_dx, 0);
				}

				if (toggle_flick) {
					if (wallhopswitch) {
						std::this_thread::sleep_for(std::chrono::milliseconds(75));
						MoveMouse(-wallhop_dy, 0);
					} else {
						std::this_thread::sleep_for(std::chrono::milliseconds(75));
						MoveMouse(wallhop_dy, 0);
					}
				}

				if (toggle_jump) {
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					ReleaseKey(0x39);
				}

				iswallhop = true;

				}
		} else {
			iswallhop = false;
		}

		// Walless LHJ (REQUIRES COM OFFSET AND .5 STUDS OF A FOOT ON A PLATFORM)
		if ((GetAsyncKeyState(vk_f6) & 0x8000) && macrotoggled && notbinding && section_toggles[7]) {
			if (!islhj) {
				if (wallesslhjswitch) {
					HoldKey(0x1E);
				} else {
					HoldKey(0x20);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				HoldKey(0x39);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (wallesslhjswitch) {
					ReleaseKey(0x1E);
				} else {
					ReleaseKey(0x20);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				HoldKey(scancode_shift);
				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				ReleaseKey(scancode_shift);
				ReleaseKey(0x39);
				islhj = true;
			}
		} else {
			islhj = false;
		}

		// Speedglitch
		if ((GetAsyncKeyState(vk_xkey) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[3]) {
			if (!isspeedglitch) {
				isspeed = !isspeed;
				isspeedglitch = true;
			}
		} else {
			isspeedglitch = false;
			if (isspeedswitch) {
				isspeed = false;
			}
		}

		// Gear Unequip Speed
		if ((GetAsyncKeyState(vk_f8) & 0x8000) && macrotoggled && notbinding && section_toggles[4]) {
			if (!isunequipspeed) {
				if (chatoverride) {
					HoldKey(0x35);
				} else {
					PasteText(chatkey);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (chatoverride) {
					ReleaseKey(0x35);
				}

				if (strlen(CustomTextChar) >= 1) {
					PasteText(CustomTextChar);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					HoldKey(0x1C);

					std::this_thread::sleep_for(std::chrono::milliseconds(35));
					ReleaseKey(0x1C);
					isunequipspeed = true;
					continue;
				}

				PasteText(text);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				HoldKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(35));
				ReleaseKey(0x1C);
				if (selected_dropdown == 2) {
				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds(65));
				}

				if (selected_dropdown == 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(815));
				}
				if (selected_dropdown == 1) {
					std::this_thread::sleep_for(std::chrono::milliseconds(175));
				}
				HoldKey(speed_slot + 1);
				ReleaseKey(speed_slot + 1);
				std::this_thread::sleep_for(std::chrono::milliseconds(4));
				if (!unequiptoggle) {
					HoldKey(speed_slot + 1);
				}
				ReleaseKey(speed_slot + 1);
				isunequipspeed = true;
			}
		} else {
			isunequipspeed = false;
		}

		// Helicopter High jump
		if ((GetAsyncKeyState(vk_xbutton1) & 0x8000) && macrotoggled && notbinding && section_toggles[2]) {
			if (!HHJ) {

				if (autotoggle) { // Auto-Key-Timer
					HoldKey(0x39);
					std::this_thread::sleep_for(std::chrono::milliseconds(550));
					HoldKey(0x11);
					std::this_thread::sleep_for(std::chrono::milliseconds(68));
				}

				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);
				if (!fasthhj) {
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(200));

				if (autotoggle) { // Auto-Key-Timer
					ReleaseKey(0x39);
					ReleaseKey(0x11);
				}

				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);
				std::this_thread::sleep_for(std::chrono::milliseconds(8));
				HoldKey(scancode_shift);
				std::this_thread::sleep_for(std::chrono::milliseconds(17));
				isHHJ = true;
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				ReleaseKey(scancode_shift);
				std::this_thread::sleep_for(std::chrono::milliseconds(243));
				isHHJ = false;
				HHJ = true;
			}
		} else {
			HHJ = false;
		}

		// Spamkey
		if ((GetAsyncKeyState(vk_leftbracket) & 0x8000) && macrotoggled && notbinding && section_toggles[11]) {
			if (!isspam) {
				isspamloop = !isspamloop;
				isspam = true;
			}
		} else {
			isspam = false;
			if (isspamswitch) {
				isspamloop = false;
			}
		}

		// Laughkey
		if ((GetAsyncKeyState(vk_laughkey) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[9]) {
			if (!islaugh) {
				if (chatoverride) {
					HoldKey(0x35);
				} else {
					PasteText(chatkey);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (chatoverride) {
					ReleaseKey(0x35);
				}

				PasteText("/e laugh");
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				HoldKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(35));
				ReleaseKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(248));

				HoldKey(0x39); // Jump
				HoldKey(scancode_shift);

				if (!laughmoveswitch) {
					HoldKey(0x1F); // S
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(25));
				ReleaseKey(0x39);
				ReleaseKey(scancode_shift);
				ReleaseKey(0x1C);
				std::this_thread::sleep_for(std::chrono::milliseconds(25));

				if (!laughmoveswitch) {
					ReleaseKey(0x1F); // S
				}

				islaugh = true;
			}
		} else {
			islaugh = false;
		}


		// Ledge Bounce
		if ((GetAsyncKeyState(vk_bouncekey) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[12]) {
			if (!isbounce) {
				int turn90 = (180 / atof(RobloxSensValue));
				int skey = 0x1F;
				int dkey = 0x20;
				int wkey = 0x11;
				if (bouncesidetoggle) {
					turn90 = -turn90;
					dkey = 0x1E;
				}

				MoveMouse(-turn90, 0);  // Left
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				HoldKey(skey); // Hold S
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				ReleaseKey(skey);     // Release S
				HoldKey(dkey);        // Hold D
				MoveMouse(turn90, 0); // Right
				std::this_thread::sleep_for(std::chrono::microseconds(3030));
				ReleaseKey(dkey); // Release D

				if (!bouncerealignsideways && !bounceautohold) {
				} else {
					HoldKey(0x11); // Hold W
				}

				if (bouncerealignsideways) {
					MoveMouse(turn90, 0); // Right
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					ReleaseKey(wkey);      // Release W
					MoveMouse(-turn90, 0); // Left
					if (bounceautohold) {
						HoldKey(dkey); // Hold D
					}
				} else {
					MoveMouse(turn90, 0); // Right
				}

				isbounce = true;
			}
		} else {
			isbounce = false;
		}


		// Item Clip
		if ((GetAsyncKeyState(vk_clipkey) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[8]) {
			if (!isclip) {
				isitemloop = !isitemloop;
				isclip = true;
			}
		} else {
			isclip = false;
			if (isitemclipswitch) {
				isitemloop = false;
			}
		}


		// WallWalk
		if ((GetAsyncKeyState(vk_wallkey) & 0x8000) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[10]) {
			if (!iswallwalk) {
				iswallwalkloop = !iswallwalkloop;
				iswallwalk = true;
			}
		} else {
			iswallwalk = false;
			if (iswallwalkswitch) {
				iswallwalkloop = false;
			}
		}

		// Bunnyhop / Bhop
		if (bunnyhopsmart) {
			// Smart bunnyhop mode with chat lock
			if (!bhoplocked && g_isVk_BunnyhopHeldDown.load(std::memory_order_relaxed) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[13]) 
			{
				if (!isbhop) {
					isbhoploop = true;
					isbhop = true;
				}
			} else {
				if (isbhop) {
					isbhoploop = false;
					isbhop = false;
				}
        
				// Check for chat key press to lock
				if (GetAsyncKeyState(vk_chatkey) & 0x8000) {
					bhoplocked = true;
				}
			}
    
			// Check for lock release conditions (ENTER or Mouse1)
			if (bhoplocked && (GetAsyncKeyState(VK_RETURN) & 0x8000 || GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
				bhoplocked = false;
			}
		} 
		else {
			// Normal behavior when bunnyhopsmart is false
			if (g_isVk_BunnyhopHeldDown.load(std::memory_order_relaxed) && IsForegroundWindowProcess(hProcess) && macrotoggled && notbinding && section_toggles[13]) 
			{
				if (!isbhop) {
					isbhoploop = true;
					isbhop = true;
				}
			} else {
				if (isbhop) {
					isbhoploop = false;
					isbhop = false;
				}
				if (!g_isVk_BunnyhopHeldDown.load(std::memory_order_relaxed)) {
					isbhoploop = false;
					isbhop = false;
				}
			}
		}

		// Every second, check if roblox continues to exist.
		auto currentTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastProcessCheck).count() >= 1) {
			if (true) { // Holy crap, adding in this useless statement fixes false positive detections for some reason???
				hProcess = GetProcessHandles(GetProcessIdByName(takeallprocessids), PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME);
				rbxhwnd = FindWindowByProcessHandle(hProcess);
			}
			lastProcessCheck = std::chrono::steady_clock::now();
		}

		// Reset anti-afk if you press any keys
		isafk = true;
		for (int i = 0; i < 256; ++i) {
			if (GetAsyncKeyState(i) & 0x8000) { // Check if any key is pressed
				isafk = false;
				break;
			}
		}

		// Message handler for this thread for the keyboard hook only
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		// Anti AFK (MUST STAY AT THE LOWEST PART OF THE LIST!!!)
		if (!isafk && IsForegroundWindowProcess(hProcess)) {
			// W is pressed, update the last press time to now
			lastPressTime = std::chrono::steady_clock::now();
		} else {
			if (processFound && antiafktoggle && isafk) {
				// Check if 15 minutes have passed
				auto afktime = std::chrono::steady_clock::now();
				auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(afktime - lastPressTime).count();
				if (elapsedMinutes >= 15) {
					HWND originalHwnd = GetForegroundWindow();
					INPUT input = {0};
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					if (IsForegroundWindowProcess(hProcess) == 0) { // Extremely long process to simulate going to roblox and typing .

						HWND windowhwnd = FindNewestProcessWindow(rbxhwnd);
						SetWindowPos(windowhwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(windowhwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						Sleep(1000);
						SendInput(1, &input, sizeof(INPUT));
						input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
						Sleep(50);
						SendInput(1, &input, sizeof(INPUT));
						Sleep(500);
						if (chatoverride) {
							HoldKey(0x35);
						} else {
							PasteText(chatkey);
						}
						if (chatoverride) {
							Sleep(20);
							ReleaseKey(0x35);
						}
						Sleep(20);
						HoldKey(0x34);
						Sleep(20);
						ReleaseKey(0x34);
						Sleep(20);
						HoldKey(0x1C);
						Sleep(20);
						ReleaseKey(0x1C);
						Sleep(500);
						SetWindowPos(originalHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(originalHwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						Sleep(1000);
						SetForegroundWindow(originalHwnd);
						lastPressTime = std::chrono::steady_clock::now();
					}
					if (IsForegroundWindowProcess(hProcess) == 1) {
						if (chatoverride) {
							HoldKey(0x35);
						} else {
							PasteText(chatkey);
						}
						if (chatoverride) {
							Sleep(20);
							ReleaseKey(0x35);
						}
						Sleep(20);
						HoldKey(0x34);
						Sleep(20);
						ReleaseKey(0x34);
						Sleep(20);
						HoldKey(0x1C);
						Sleep(20);
						ReleaseKey(0x1C);
						Sleep(500);
						lastPressTime = std::chrono::steady_clock::now();
					}
				}
			}
		}
	}

	// Automatically turn off these 4 if you leave roblox window (so it isn't annoying)
	if (!IsForegroundWindowProcess(hProcess)) {
		isbhoploop = false;
		iswallwalkloop = false;
		isitemloop = false;
		isdesyncloop = false;
		isspeed = false;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(50)); // Delay between main code loop (so your cpu doesn't die instantly)

}

	// Cleanup on exit
	SaveSettings("RMCSettings.json");
	if (g_keyboardHook) {
		UnhookWindowsHookEx(g_keyboardHook);
		g_keyboardHook = NULL;
	}
	guiThread.join();
	CleanupDeviceD3D();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	DestroyWindow(hwnd);
	timeEndPeriod(1);
	exit(0);

	return 0;
}

// Helper function to apply a theme
static void ApplyTheme(const Theme& theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Apply modern styling
    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(8, 6);
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 12;
    style.GrabMinSize = 8;
    style.WindowRounding = theme.window_rounding;
    style.ChildRounding = theme.window_rounding * 0.75f;
    style.FrameRounding = theme.frame_rounding;
    style.PopupRounding = theme.window_rounding * 0.75f;
    style.ScrollbarRounding = theme.frame_rounding;
    style.GrabRounding = theme.button_rounding;
    style.TabRounding = theme.frame_rounding;

    // Colors
    style.Colors[ImGuiCol_WindowBg] = theme.bg_dark;
    style.Colors[ImGuiCol_ChildBg] = theme.bg_medium;
    style.Colors[ImGuiCol_PopupBg] = theme.bg_medium;
    style.Colors[ImGuiCol_Border] = theme.border_color;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = theme.bg_light;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(theme.bg_light.x + 0.05f, theme.bg_light.y + 0.05f, theme.bg_light.z + 0.05f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = theme.accent_primary;
    style.Colors[ImGuiCol_TitleBg] = theme.bg_dark;
    style.Colors[ImGuiCol_TitleBgActive] = theme.bg_dark;
    style.Colors[ImGuiCol_TitleBgCollapsed] = theme.bg_dark;
    style.Colors[ImGuiCol_MenuBarBg] = theme.bg_medium;
    style.Colors[ImGuiCol_ScrollbarBg] = theme.bg_medium;
    style.Colors[ImGuiCol_ScrollbarGrab] = theme.accent_primary;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = theme.accent_secondary;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_CheckMark] = theme.accent_primary;
    style.Colors[ImGuiCol_SliderGrab] = theme.accent_primary;
    style.Colors[ImGuiCol_SliderGrabActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_Button] = theme.accent_primary;
    style.Colors[ImGuiCol_ButtonHovered] = theme.accent_secondary;
    style.Colors[ImGuiCol_ButtonActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_Header] = theme.accent_primary;
    style.Colors[ImGuiCol_HeaderHovered] = theme.accent_secondary;
    style.Colors[ImGuiCol_HeaderActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_Separator] = theme.border_color;
    style.Colors[ImGuiCol_SeparatorHovered] = theme.accent_primary;
    style.Colors[ImGuiCol_SeparatorActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_ResizeGrip] = theme.accent_primary;
    style.Colors[ImGuiCol_ResizeGripHovered] = theme.accent_secondary;
    style.Colors[ImGuiCol_ResizeGripActive] = theme.accent_secondary;
    style.Colors[ImGuiCol_Tab] = theme.bg_light;
    style.Colors[ImGuiCol_TabHovered] = theme.accent_primary;
    style.Colors[ImGuiCol_TabActive] = theme.accent_primary;
    style.Colors[ImGuiCol_TabUnfocused] = theme.bg_light;
    style.Colors[ImGuiCol_TabUnfocusedActive] = theme.accent_primary;
    style.Colors[ImGuiCol_Text] = theme.text_primary;
    style.Colors[ImGuiCol_TextDisabled] = theme.text_secondary;
    style.Colors[ImGuiCol_NavHighlight] = theme.accent_primary;
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
}

// Theme editor UI
static void RenderThemeEditor() {
    if (!show_theme_editor) return;
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Theme Editor", &show_theme_editor, ImGuiWindowFlags_NoCollapse)) {
        // Theme selector
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SELECT THEME");
        ImGui::Separator();
        std::vector<const char*> theme_names;
        for (const auto& theme : themes) theme_names.push_back(theme.name.c_str());
        theme_names.push_back("Custom Theme");
        if (ImGui::Combo("Theme", &current_theme_index, theme_names.data(), theme_names.size())) theme_modified = true;
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "CUSTOMIZE COLORS");
        ImGui::Separator();
        Theme* editing_theme = (current_theme_index < themes.size()) ? &themes[current_theme_index] : &custom_theme;
        if (editing_theme) {
            ImGui::BeginChild("ColorEditor", ImVec2(0, 300), true);
            ImGui::Text("Background Colors");
            ImGui::ColorEdit4("Dark Background", (float*)&editing_theme->bg_dark);
            ImGui::ColorEdit4("Medium Background", (float*)&editing_theme->bg_medium);
            ImGui::ColorEdit4("Light Background", (float*)&editing_theme->bg_light);
            ImGui::Spacing();
            ImGui::Text("Accent Colors");
            ImGui::ColorEdit4("Primary Accent", (float*)&editing_theme->accent_primary);
            ImGui::ColorEdit4("Secondary Accent", (float*)&editing_theme->accent_secondary);
            ImGui::Spacing();
            ImGui::Text("Text Colors");
            ImGui::ColorEdit4("Primary Text", (float*)&editing_theme->text_primary);
            ImGui::ColorEdit4("Secondary Text", (float*)&editing_theme->text_secondary);
            ImGui::Spacing();
            ImGui::Text("Status Colors");
            ImGui::ColorEdit4("Success", (float*)&editing_theme->success_color);
            ImGui::ColorEdit4("Warning", (float*)&editing_theme->warning_color);
            ImGui::ColorEdit4("Error", (float*)&editing_theme->error_color);
            ImGui::Spacing();
            ImGui::Text("Border Color");
            ImGui::ColorEdit4("Border", (float*)&editing_theme->border_color);
            ImGui::EndChild();
            ImGui::Spacing();
            ImGui::Text("Rounding Settings");
            ImGui::SliderFloat("Window Rounding", &editing_theme->window_rounding, 0.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("Frame Rounding", &editing_theme->frame_rounding, 0.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("Button Rounding", &editing_theme->button_rounding, 0.0f, 20.0f, "%.1f");
            ImGui::Spacing();
            if (current_theme_index == themes.size()) {
                ImGui::Text("Custom Theme Name");
                static char theme_name[256];
                strncpy(theme_name, editing_theme->name.c_str(), sizeof(theme_name) - 1);
                if (ImGui::InputText("##ThemeName", theme_name, sizeof(theme_name))) editing_theme->name = theme_name;
            }
            ImGui::Spacing();
            if (ImGui::Button("Apply Theme", ImVec2(120, 30))) { ApplyTheme(*editing_theme); theme_modified = false; }
            ImGui::SameLine();
            if (ImGui::Button("Reset to Default", ImVec2(120, 30))) {
                if (current_theme_index < themes.size()) themes[current_theme_index] = themes[0];
                else custom_theme = themes[0];
                ApplyTheme(*editing_theme); theme_modified = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Settings", ImVec2(120, 30))) { SaveSettings("RMCSettings.json"); theme_modified = false; }
            ImGui::Spacing();
            ImGui::Text("Preview");
            ImGui::Separator();
            ImGui::BeginChild("Preview", ImVec2(0, 100), true);
            ApplyTheme(*editing_theme);
            ImGui::Text("Sample Text");
            ImGui::Button("Sample Button");
            ImGui::SameLine();
            ImGui::Checkbox("Sample Checkbox", &show_theme_editor);
            ImGui::SameLine();
            ImGui::SliderFloat("Sample Slider", &editing_theme->window_rounding, 0.0f, 20.0f);
            ImGui::EndChild();
        }
    }
    ImGui::End();
}
