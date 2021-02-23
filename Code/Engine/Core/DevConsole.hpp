#pragma once

#include <string>
#include <vector>
#include "Engine/Core/Rgba8.hpp"

struct AABB2;
class BitmapFont;
class InputSystem;
class Camera;
class RenderContext;
class Clock;
class Timer;

struct ColoredLine
{
	Rgba8 color;
	std::string text;

	ColoredLine( Rgba8 inColor, std::string inText );
};

class DevConsole
{
public:
	DevConsole(InputSystem* input);
	~DevConsole();

	void Startup();
	void BeginFrame() {}
	void Update();
	void EndFrame() {}
	void Shutdown();

    void ProcessInput();
	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString );
	void PrintError(std::string const& devConsoleError);
	void Render( RenderContext* renderer) const;

	void ClearInput();

	void SetIsOpen(bool isOpen);
	bool IsOpen() const { return m_isOpen; }

private:
	int m_caretPos = 0;
    Timer* m_cursorTimer = nullptr;
	float m_lineHeight = 1.f;
	std::string m_inputLine;

	int m_selectPosCenter = -1;
	int m_selectDeltaPos = 0;

	std::vector<std::string> m_choiceCommands;
	bool m_showChoices = true;
	int m_chosenCommandIdx = -1;

	Clock* m_clock = nullptr;
	Timer* m_timer = nullptr;
	InputSystem* m_input = nullptr;
	Camera* m_camera = nullptr;
	bool m_isOpen = false;
	std::vector<ColoredLine> m_strings; //never delete for future dumping

	std::vector<std::string> m_lastCommands;
	int m_lastCommandIdx = -1;

	void EnterCommand(std::string const& command);
	void UpdateCommandHistory(std::string const& newCommand);
	void UpdateChoiceCommands();

	void HandleInputEvent();

	void ClearSelection();

	void DrawCursor(RenderContext* renderer) const;
	void RenderChoiceCommands(RenderContext* renderer) const;
};