#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>

extern HWND g_hWnd;

const unsigned char MOUSE_LBUTTON = VK_LBUTTON;
const unsigned char MOUSE_MBUTTON = VK_MBUTTON;
const unsigned char MOUSE_RBUTTON = VK_RBUTTON;
const unsigned char KEY_TILDE = VK_OEM_3;
const unsigned char KEY_PLUS = VK_OEM_PLUS;
const unsigned char KEY_MINUS = VK_OEM_MINUS;
const unsigned char KEY_ESC = VK_ESCAPE;
const unsigned char KEY_ENTER = VK_RETURN;
const unsigned char KEY_SPACEBAR = VK_SPACE;
const unsigned char KEY_BACKSPACE = VK_BACK;
const unsigned char KEY_SHIFT = VK_SHIFT;
const unsigned char KEY_ALT = VK_MENU;
const unsigned char KEY_DELETE = VK_DELETE;
const unsigned char KEY_UPARROW = VK_UP;
const unsigned char KEY_LEFTARROW = VK_LEFT;
const unsigned char KEY_DOWNARROW = VK_DOWN;
const unsigned char KEY_RIGHTARROW = VK_RIGHT;
const unsigned char KEY_F1 = VK_F1;
const unsigned char KEY_F2 = VK_F2;
const unsigned char KEY_F3 = VK_F3;
const unsigned char KEY_F4 = VK_F4;
const unsigned char KEY_F5 = VK_F5;
const unsigned char KEY_F6 = VK_F6;
const unsigned char KEY_F7 = VK_F7;
const unsigned char KEY_F8 = VK_F8;
const unsigned char KEY_F9 = VK_F9;
const unsigned char KEY_F10 = VK_F10;
const unsigned char KEY_F11 = VK_F11;
const unsigned char KEY_F12 = VK_F12;
const unsigned char KEY_LEFTBRACKET = VK_OEM_4;
const unsigned char KEY_BACKSLASH = VK_OEM_5;
const unsigned char KEY_RIGHTBRACKET = VK_OEM_6;
const unsigned char KEY_COMMA = VK_OEM_COMMA;
const unsigned char KEY_PERIOD = VK_OEM_PERIOD;
const unsigned char KEY_COLON = VK_OEM_1;
const unsigned char KEY_QUOTE = VK_OEM_7;
const unsigned char CHAR_COPY = 0x03;
const unsigned char CHAR_CUT = 0x18;
const unsigned char CHAR_PASTE = 0x16;

//////////////////////////////////////////////////////////////////////////
void InputSystem::Startup()
{
	for( int buttonID = 0; buttonID < MAX_KEYBOARD_BUTTONS; buttonID++ )
	{
		m_keyboardButtons[buttonID] = KeyButtonState();
    }
    for (int idx = 0; idx < NUM_HOTKEYS; idx++) {
        m_hotkeys[idx] = KeyButtonState();
    }
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::BeginFrame()
{
	for( int controllerID = 0; controllerID < MAX_XBOX_CONTROLLERS; controllerID++ )
	{
		m_controllers[controllerID].Update();
	}
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::EndFrame()
{
	for( int buttonID = 0; buttonID < MAX_KEYBOARD_BUTTONS; buttonID++ )
	{
		m_keyboardButtons[buttonID].Update();
	}
    for (int idx = 0; idx < NUM_HOTKEYS; idx++) {
        m_hotkeys[idx].UpdateStatus(false);
    }

	ClearCharacter();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::Shutdown()
{
	for( int controllerID = 0; controllerID < MAX_XBOX_CONTROLLERS; controllerID++ )
	{
		m_controllers[controllerID].Reset();
	}
	for( int buttonID = 0; buttonID < MAX_KEYBOARD_BUTTONS; buttonID++ )
	{
		m_keyboardButtons[buttonID].Reset();
	}
	for (int idx = 0; idx < NUM_HOTKEYS; idx++) {
		m_hotkeys[idx].Reset();
	}
}

//////////////////////////////////////////////////////////////////////////
Vec2 InputSystem::GetMouseNormalizedClientPos() const
{
	return m_mouseNomalizedClientPos;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::UpdateMouseButtonState( bool leftButtonDown, bool middleButtonDown, bool rightButtonDown )
{
	m_keyboardButtons[MOUSE_LBUTTON].m_isPressed = leftButtonDown;
	m_keyboardButtons[MOUSE_MBUTTON].m_isPressed = middleButtonDown;
	m_keyboardButtons[MOUSE_RBUTTON].m_isPressed = rightButtonDown;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::AddMouseWheelScrollAmount( float scrollAmount )
{
	m_mouseScrollValue += scrollAmount;
}

//////////////////////////////////////////////////////////////////////////
bool InputSystem::IsKeyDown( unsigned char keyCode ) const
{
	return m_keyboardButtons[static_cast<int>(keyCode)].IsPressed();
}

//////////////////////////////////////////////////////////////////////////
bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	return m_keyboardButtons[static_cast<int>(keyCode)].WasJustPressed();
}

//////////////////////////////////////////////////////////////////////////
bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
	return m_keyboardButtons[static_cast<int>(keyCode)].WasJustReleased();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::ClearCharacter()
{
	m_characters.clear();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::PushCharacter(char const& chr)
{
	if (chr == CHAR_COPY) {
		UpdateHotkey(eHotKey::COPY, true);
	}
	else if (chr == CHAR_CUT) {
		UpdateHotkey(eHotKey::CUT, true);
	}
	else if (chr == CHAR_PASTE) {
		UpdateHotkey(eHotKey::PASTE, true);
	}
	else if(chr!='\b' && chr!='\x1b'){
		m_characters.push_back(chr);
	}
}

//////////////////////////////////////////////////////////////////////////
char const* InputSystem::GetCharacters() const
{
	return m_characters.c_str();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::PushMouseOptions(eMousePositionMode mode, bool isVisible, bool isClipped)
{
	MouseOptions newOptions(mode, isVisible, isClipped);
	m_mouseOptions.push(newOptions);
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::PopMouseOptions()
{
	m_mouseOptions.pop();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::SetClipboardContent(std::string const& newContent)
{
	m_clipboard = newContent;
}

//////////////////////////////////////////////////////////////////////////
bool InputSystem::WasHotkeyJustPressed(eHotKey hotkey) const
{
	return m_hotkeys[hotkey].WasJustPressed();
}

//////////////////////////////////////////////////////////////////////////
bool InputSystem::WasHotKeyJustReleased(eHotKey hotkey) const
{
	return m_hotkeys[hotkey].WasJustReleased();
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::HandleKeyDown( unsigned char keyCode )
{
	m_keyboardButtons[static_cast<int>(keyCode)].m_isPressed = true;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::HandleKeyRelease( unsigned char keyCode )
{
	m_keyboardButtons[static_cast<int>(keyCode)].m_isPressed = false;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::SetKeyButtonState( unsigned char keyCode, bool state )
{
	KeyButtonState& thisButton = m_keyboardButtons[static_cast<int>(keyCode)];
	thisButton.UpdateStatus( state );
}

//////////////////////////////////////////////////////////////////////////
const XboxController& InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[controllerID];
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::SetVibrationValue( int controllerID, float leftMotorValue, float rightMotorValue )
{
	if( controllerID < 4 )
	{
		m_controllers[controllerID].SetVibrationValue( leftMotorValue, rightMotorValue );
	}
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::UpdateMouse(Vec2 const& newPos)
{
	m_mouseNomalizedClientPos = newPos;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::UpdateHotkey(eHotKey hotkey, bool newState) 
{
	m_hotkeys[hotkey].m_isPressed = newState;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::HideSystemCursor()
{
	m_mouseOptions.top().isVisible = false;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::ClipSystemCursor()
{
	m_mouseOptions.top().isClipped = true;
}

//////////////////////////////////////////////////////////////////////////
void InputSystem::SetCursorMode(eMousePositionMode mode)
{
	m_mouseOptions.top().moveMode = mode;
}
