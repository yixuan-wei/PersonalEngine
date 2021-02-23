#pragma once

#include <string>

class InputSystem;
struct Vec2;
struct IntVec2;
enum eMousePositionMode : int;

class Window
{
public:
	Window();
	~Window();

	// SD2.A01
	void SetInputSystem( InputSystem* inputSystem );

	// SD2.A01
	bool Open( std::string const& title, float clientAspect = 16.f/9.f, float ratioHeight = .9f );
	void Close();

	// SD2.A01
	void BeginFrame(); // process window messages
	void EndFrame();

	void HandleQuitRequested();

	void UpdateForWindowActivation(bool activated);

	bool IsQuiting() const { return m_isQuiting; }
	unsigned int GetClientWidth();
	unsigned int GetClientHeight();

	InputSystem* GetInputSystem() const { return m_inputSystem; }

public:
	void* m_hwnd = nullptr;

private:
	unsigned int m_clientWidth=0;
	unsigned int m_clientHeight=0;
	bool m_isQuiting = false;
	// input system the window feeds into
	InputSystem* m_inputSystem = nullptr;

    eMousePositionMode m_lastMousePosMode;
	bool m_lastShowCursor = true;
	bool m_lastClipCursor = false;

	// You may need to add more to make things work; 
	Vec2 GetMouseNormalizedClientPos() const;
	IntVec2 GetCenter() const;

	void UpdateMouse();
	void UpdateAbsoluteMouse();
	void UpdateRelativeMouse();
	void UpdateForCursor();

	void ReadFromClipboard();
	void WriteToClipboard();
};