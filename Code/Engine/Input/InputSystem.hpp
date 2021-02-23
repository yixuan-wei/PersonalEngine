#pragma once

#include <string>
#include <stack>
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/MouseOptions.hpp"

class Window;

constexpr int MAX_XBOX_CONTROLLERS = 4;
constexpr int MAX_KEYBOARD_BUTTONS = 256;

//KEY CODE
extern const unsigned char MOUSE_LBUTTON;
extern const unsigned char MOUSE_MBUTTON;
extern const unsigned char MOUSE_RBUTTON;
extern const unsigned char KEY_TILDE;
extern const unsigned char KEY_PLUS;
extern const unsigned char KEY_MINUS;
extern const unsigned char KEY_ESC;
extern const unsigned char KEY_ENTER;
extern const unsigned char KEY_SPACEBAR;
extern const unsigned char KEY_BACKSPACE;
extern const unsigned char KEY_DELETE;
extern const unsigned char KEY_SHIFT;
extern const unsigned char KEY_ALT;
extern const unsigned char KEY_UPARROW;
extern const unsigned char KEY_LEFTARROW;
extern const unsigned char KEY_DOWNARROW;
extern const unsigned char KEY_RIGHTARROW;
extern const unsigned char KEY_F1;
extern const unsigned char KEY_F2;
extern const unsigned char KEY_F3;
extern const unsigned char KEY_F4;
extern const unsigned char KEY_F5;
extern const unsigned char KEY_F6;
extern const unsigned char KEY_F7;
extern const unsigned char KEY_F8;
extern const unsigned char KEY_F9;
extern const unsigned char KEY_F10;
extern const unsigned char KEY_F11;
extern const unsigned char KEY_F12;
extern const unsigned char KEY_LEFTBRACKET;
extern const unsigned char KEY_RIGHTBRACKET;
extern const unsigned char KEY_BACKSLASH;
extern const unsigned char KEY_COMMA;
extern const unsigned char KEY_PERIOD;
extern const unsigned char KEY_COLON;
extern const unsigned char KEY_QUOTE;
extern const unsigned char CHAR_COPY;
extern const unsigned char CHAR_CUT;
extern const unsigned char CHAR_PASTE;

enum eHotKey {
	COPY = 0,
	CUT,
	PASTE,

	NUM_HOTKEYS
};

//////////////////////////////////////////////////////////////////////////
class InputSystem
{
	friend class Window;

public:
	InputSystem() = default;
	~InputSystem() = default;

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	Vec2 GetMouseNormalizedClientPos() const;
	float GetMouseWheelScrollValue() const { return m_mouseScrollValue; }

	void UpdateMouseButtonState( bool leftButtonDown, bool middleButtonDown, bool rightButtonDown );
	void AddMouseWheelScrollAmount( float scrollAmount );

	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	void ClearCharacter();
	void PushCharacter(char const& chr);
	char const* GetCharacters() const;

	void PushMouseOptions(eMousePositionMode mode, bool isVisible, bool isClipped);
	void PopMouseOptions();

	Vec2 GetMouseRelativeMove() const { return m_mouseRelativeMove; }

	void SetClipboardContent(std::string const& newContent);
	bool WasHotkeyJustPressed(eHotKey hotkey) const;
	bool WasHotKeyJustReleased(eHotKey hotkey) const;
	std::string GetClipboardContent() const { return m_clipboard; }

	void HandleKeyDown( unsigned char keyCode );
	void HandleKeyRelease( unsigned char keyCode );
	void SetKeyButtonState( unsigned char keyCode, bool state );

	const XboxController& GetXboxController( int controllerID );
	void                  SetVibrationValue( int controllerID, float leftMotorValue, float rightMotorValue );

private:
	std::string m_clipboard;
	std::string m_characters;
	KeyButtonState m_hotkeys[NUM_HOTKEYS];

	std::stack<MouseOptions> m_mouseOptions;
	bool m_windowActivated = true;

	float m_mouseScrollValue = 0.f;
	Vec2 m_mouseNomalizedClientPos = Vec2::ZERO;
	Vec2 m_mouseRelativeMove = Vec2::ZERO;
	KeyButtonState m_keyboardButtons[MAX_KEYBOARD_BUTTONS];

	XboxController m_controllers[MAX_XBOX_CONTROLLERS] =
	{
		XboxController( 0 ),
		XboxController( 1 ),
		XboxController( 2 ),
		XboxController( 3 )
	};

	void UpdateMouse(Vec2 const& newPos);
	void UpdateHotkey(eHotKey hotkey, bool newState );

	void HideSystemCursor();
	void ClipSystemCursor();
	void SetCursorMode(eMousePositionMode mode);
};
