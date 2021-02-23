#include "Engine/Platform/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Rgba8.hpp"

#include "ThirdParty/imgui/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

static TCHAR const* WND_CLASS_NAME = L"Simple Window Class";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	Window* window = (Window*)::GetWindowLongPtr(windowHandle,GWLP_USERDATA);

	//IMGUI
	ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam);

	switch( wmMessageCode )
	{
		case WM_CREATE:
		{
			Window* wind = (Window*)lParam;
			::SetWindowLongPtr( windowHandle, GWLP_USERDATA, (LONG_PTR)wind );
			break;
		}

		case WM_ACTIVATE:
		{
			bool inactive = (LOWORD(wParam) == WA_INACTIVE);
			if (window->GetInputSystem()) {
				window->UpdateForWindowActivation(!inactive);
			}
			break;
		}

		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:
		{
			window->HandleQuitRequested();
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        {
            bool leftButtonDown = wParam & MK_LBUTTON;
            bool middleButtonDown = wParam & MK_MBUTTON;
            bool rightButtonDown = wParam & MK_RBUTTON;
            window->GetInputSystem()->UpdateMouseButtonState(leftButtonDown, middleButtonDown, rightButtonDown);
			break;
        }

        case WM_MOUSEWHEEL:
        {
            short scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            float scrollAmount = (float)scrollDelta / (float)WHEEL_DELTA;
            window->GetInputSystem()->AddMouseWheelScrollAmount(scrollAmount);
			break;
        }

        case WM_CHAR:
        {
            char character = (char)wParam;
            window->GetInputSystem()->PushCharacter(character);
            break;
        }

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = static_cast<unsigned char>(wParam);
			window->GetInputSystem()->HandleKeyDown( asKey );
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char)wParam;
			window->GetInputSystem()->HandleKeyRelease( asKey );
			break;
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return ::DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////
static void RegisterWindowClass()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	::memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = ::GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL; //TODO: icon here
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	::RegisterClassEx( &windowClassDescription );
}

//////////////////////////////////////////////////////////////////////////
static void UnregisterWindowClass()
{
	::UnregisterClass( WND_CLASS_NAME, GetModuleHandle( NULL ) );
}

//////////////////////////////////////////////////////////////////////////
Window::Window()
	:m_hwnd(nullptr)
{
	RegisterWindowClass();
}

//////////////////////////////////////////////////////////////////////////
Window::~Window()
{
	Close();
	UnregisterWindowClass();
}

//////////////////////////////////////////////////////////////////////////
void Window::SetInputSystem( InputSystem* inputSystem )
{
	m_inputSystem = inputSystem;
	m_inputSystem->PushMouseOptions(eMousePositionMode::MOUSE_ABSOLUTE, true, false);
	UpdateMouse();
}

//////////////////////////////////////////////////////////////////////////
bool Window::Open( std::string const& title, float clientAspect, float maxClientFractionOfDesktop )
{
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED; //TODO full-screen here
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	::GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	::AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[1024];
	::MultiByteToWideChar( GetACP(), 0, title.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		WND_CLASS_NAME,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE)::GetModuleHandle(NULL),
		NULL );

	::SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this );

	if( hwnd == nullptr )
		return false;

    ::ShowWindow(hwnd, SW_SHOW);
    ::SetForegroundWindow(hwnd);
    ::SetFocus(hwnd);

	HCURSOR cursor = ::LoadCursor( NULL, IDC_ARROW );
	::SetCursor( cursor );

	::SetActiveWindow((HWND)m_hwnd);

	m_hwnd = (void*)hwnd;
	m_clientWidth = (unsigned int)clientWidth;
	m_clientHeight = (unsigned int)clientHeight;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Window::Close()
{

	HWND hwnd = (HWND)m_hwnd;
	if( NULL == hwnd )
		return;

	::DestroyWindow( hwnd );
	m_hwnd = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
void Window::BeginFrame()
{
	//windows message pump
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = ::PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		::TranslateMessage( &queuedMessage );
		::DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}

//////////////////////////////////////////////////////////////////////////
void Window::EndFrame()
{
	if (!m_inputSystem->m_windowActivated) {
		return;
	}

	UpdateMouse();
    //update clipboard
    if (m_inputSystem->WasHotKeyJustReleased(eHotKey::COPY) || m_inputSystem->WasHotKeyJustReleased(eHotKey::CUT)) {
        WriteToClipboard();
    }
    if (m_inputSystem->WasHotKeyJustReleased(eHotKey::PASTE)) {
        ReadFromClipboard();
    }
}

//////////////////////////////////////////////////////////////////////////
void Window::HandleQuitRequested()
{
	m_isQuiting = true;
}

//////////////////////////////////////////////////////////////////////////
unsigned int Window::GetClientWidth()
{
	return m_clientWidth;
}

//////////////////////////////////////////////////////////////////////////
unsigned int Window::GetClientHeight()
{
	return m_clientHeight;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Window::GetMouseNormalizedClientPos() const
{
    POINT mousePos;
    ::GetCursorPos(&mousePos);
    ::ScreenToClient((HWND)m_hwnd, &mousePos);
    Vec2 mouseClientPos((float)mousePos.x, (float)mousePos.y);

    RECT clientRect;
    ::GetClientRect((HWND)m_hwnd, &clientRect);
    AABB2 clientBounds((float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom);
    Vec2 mouseNormalizedClientPos;
    mouseNormalizedClientPos = clientBounds.GetUVForPoint(mouseClientPos);
    mouseNormalizedClientPos.y = 1.f - mouseNormalizedClientPos.y;
	return mouseNormalizedClientPos;
}

//////////////////////////////////////////////////////////////////////////
IntVec2 Window::GetCenter() const
{
    RECT clientRect;
    ::GetClientRect((HWND)m_hwnd, &clientRect);
	int centerX = ((int)clientRect.left + (int)clientRect.right) / 2;
	int centerY = ((int)clientRect.top + (int)clientRect.bottom) / 2;
	return IntVec2(centerX,centerY);
}

//////////////////////////////////////////////////////////////////////////
void Window::UpdateMouse()
{
	MouseOptions mouseOptions = m_inputSystem->m_mouseOptions.top();
    if (m_lastMousePosMode != mouseOptions.moveMode) {
        m_lastMousePosMode = mouseOptions.moveMode;
        switch (m_lastMousePosMode) {
        case eMousePositionMode::MOUSE_RELATIVE:
        {
            UpdateRelativeMouse();
            break;
        }
        }
    }

    //update cursor
    UpdateForCursor();

	switch (m_lastMousePosMode) {
	case eMousePositionMode::MOUSE_ABSOLUTE: UpdateAbsoluteMouse(); break;
	case eMousePositionMode::MOUSE_RELATIVE: UpdateRelativeMouse(); break;
	}    
}

//////////////////////////////////////////////////////////////////////////
void Window::UpdateAbsoluteMouse()
{
	Vec2 mouseNormalizedClientPos = GetMouseNormalizedClientPos();

	m_inputSystem->m_mouseRelativeMove = mouseNormalizedClientPos - m_inputSystem->m_mouseNomalizedClientPos;
	m_inputSystem->UpdateMouse(mouseNormalizedClientPos);
}

//////////////////////////////////////////////////////////////////////////
void Window::UpdateRelativeMouse()
{
    Vec2 positionThis = GetMouseNormalizedClientPos();
    m_inputSystem->m_mouseRelativeMove = positionThis - m_inputSystem->m_mouseNomalizedClientPos;
    // remap relative movement ()

    IntVec2 windowCenter = GetCenter();
    ::SetCursorPos(windowCenter.x, windowCenter.y);

    // one little trick... without - will cause drift (maybe)
    m_inputSystem->m_mouseNomalizedClientPos = GetMouseNormalizedClientPos();
}

//////////////////////////////////////////////////////////////////////////
void Window::UpdateForCursor()
{
	MouseOptions mouseOptions = m_inputSystem->m_mouseOptions.top();
	if (m_lastShowCursor != mouseOptions.isVisible) {
		m_lastShowCursor = mouseOptions.isVisible;
		if (m_lastShowCursor) {
			while (::ShowCursor(true) < 0) {}
		}
		else {
			while (::ShowCursor(false) >= 0) {}
		}
	}

	if (m_lastClipCursor != mouseOptions.isClipped) {
		m_lastClipCursor = mouseOptions.isClipped;
		if (m_lastClipCursor) {
            RECT clientRect;
            ::GetClientRect((HWND)m_hwnd, &clientRect);
			::MapWindowPoints((HWND)m_hwnd,NULL,(LPPOINT)&clientRect, (sizeof(RECT) / sizeof(POINT)));
            ::ClipCursor(&clientRect);
		}
		else {
			::ClipCursor(NULL);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Window::UpdateForWindowActivation(bool activated)
{
	m_inputSystem->m_windowActivated = activated;
	if (!activated) {
		m_inputSystem->PushMouseOptions(eMousePositionMode::MOUSE_ABSOLUTE, true, false);
	}
	else {
		m_inputSystem->PopMouseOptions();
	}

	UpdateForCursor();
}

//////////////////////////////////////////////////////////////////////////
void Window::ReadFromClipboard()
{
	m_inputSystem->m_clipboard.clear();
	if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard((HWND)m_hwnd)) {
		return;
	}

	HGLOBAL hglobal = GetClipboardData(CF_TEXT);
	if (hglobal == NULL) {
		CloseClipboard();
		g_theConsole->PrintError("fail to get clipboard data");
		return;
	}

    const char* pszText = static_cast<const char*>(GlobalLock(hglobal));
	if (pszText != nullptr) {
		m_inputSystem->m_clipboard = pszText;
	}

    GlobalUnlock(hglobal);
    CloseClipboard();
}

//////////////////////////////////////////////////////////////////////////
void Window::WriteToClipboard()
{
	if (!OpenClipboard((HWND)m_hwnd) || !EmptyClipboard()) {
		return;
	}

	std::string content = m_inputSystem->m_clipboard;
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, content.size() + 1);
    if (!hg) {
        CloseClipboard();
		g_theConsole->PrintError("Fail to write to clipboard");
        return;
    }
	char* pText = (char*)GlobalLock(hg);
	if (pText == nullptr) {
		GlobalFree(hg);
		CloseClipboard();
		return;
	}

    memcpy(pText, content.c_str(), content.size() + 1);
    GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
}


