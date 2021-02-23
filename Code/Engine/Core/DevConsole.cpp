#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Game/EngineBuildPreferences.hpp"

#include <mutex>


static std::string sCommanHistoryFile = "Data/Console/CommandHistory.txt";
static std::string sCommanLogFile = "data/console/log.txt";
static const char sDelimiter = 0x08;
static std::mutex sPrintStringMutex;

//////////////////////////////////////////////////////////////////////////
COMMAND(help, "print all console commands",eEventFlag::EVENT_CONSOLE) 
{
	UNUSED(args);

	std::vector<EventSubscription*> consoleEvents;
	g_theEvents->GetEventsFromFlag(eEventFlag::EVENT_CONSOLE, consoleEvents);
	for (int idx = 0; idx < consoleEvents.size(); idx++) {
		EventSubscription* subscription = consoleEvents[idx];
		g_theConsole->PrintString(Rgba8(0,200,0), Stringf("  %s:  %s", subscription->eventName,subscription->description));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
DevConsole::DevConsole(InputSystem* input)
	:m_input(input)
{
}

//////////////////////////////////////////////////////////////////////////
DevConsole::~DevConsole()
{
	delete m_camera;
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::Startup()
{
	m_clock = new Clock();
	m_timer = new Timer();
	m_cursorTimer = new Timer();
	m_cursorTimer->SetTimerSeconds(m_clock, 2.0);

	m_camera = new Camera();
	Vec2 halfDim = .5f * Vec2(177.f, 100.f);
	m_camera->SetOrthoView(-halfDim, halfDim);
	m_camera->SetProjectionOrthographic(100.f);

	m_lineHeight = halfDim.y / 30.f;

#ifdef ENGINE_CONSOLE_LOG
	size_t fileSize = 0;
	void* commandHistory = FileReadToNewBuffer(sCommanHistoryFile, &fileSize);
	std::string history((const char*)commandHistory, fileSize);
	if (fileSize > 0) {
		m_lastCommands = SplitStringOnDelimiter(history,sDelimiter);
		m_lastCommandIdx = (int)m_lastCommands.size() - 1;
	}

	delete[] commandHistory;
#endif // ENGINE_CONSOLE_LOG
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::Update()
{
	if (!IsOpen()) {
		return;
	}

	size_t oldInputLength = m_inputLine.size();

	HandleInputEvent();
	ProcessInput();
	UpdateChoiceCommands();

	size_t newInputLength = m_inputLine.size();
	if (oldInputLength != newInputLength) {
		m_showChoices = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::Shutdown()
{
#ifdef ENGINE_CONSOLE_LOG
    if (m_strings.size() > 0) {
        std::string log;
        for (ColoredLine& l : m_strings) {
            log += l.text;
            log += "\n";
        }
        if (!FileWriteToDisk(sCommanLogFile, &log[0], log.size())) {
            ERROR_AND_DIE("fail to save console log");
        }
	}
	m_strings.clear();

	delete m_camera;
	m_camera = nullptr;
	
	if (m_lastCommands.size() > 0) {
		std::string history = CombineStringsWithDelimiter(m_lastCommands, sDelimiter);
		if (!FileWriteToDisk(sCommanHistoryFile, &history[0], history.size())) {
			ERROR_AND_DIE("Fail to save console command history");
		}
		m_lastCommands.clear();
	}
#endif // ENGINE_CONSOLE_LOG	
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::ProcessInput()
{
	std::string newInput = m_input->GetCharacters();
	m_inputLine.insert(m_caretPos,newInput);
	int newCaretPos = m_caretPos + (int)newInput.size();
	if (newInput.size() > 0)
	{
		m_caretPos = newCaretPos;
		m_cursorTimer->Reset();
	}
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	ColoredLine newLine( textColor, devConsolePrintString );
	sPrintStringMutex.lock();
	m_strings.push_back( newLine );
	sPrintStringMutex.unlock();
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::PrintError(std::string const& devConsoleError)
{
	ColoredLine newLine(Rgba8::RED, devConsoleError);
    sPrintStringMutex.lock();
    m_strings.push_back(newLine);
    SetIsOpen(true);
    sPrintStringMutex.unlock();
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::Render( RenderContext* renderer) const
{
	if (!IsOpen())
	{
		return;
	}

	renderer->BeginCamera(m_camera);
	renderer->DisableDepth();

	BitmapFont* font = renderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");

	AABB2 camBounds = m_camera->GetBounds();
	Vec2 camMins = camBounds.mins;

	//background
    renderer->BindDiffuseTexture((Texture*)nullptr);
    renderer->DrawAABB2D(camBounds, Rgba8(0, 0, 0, 200));

	float camHeight = camBounds.maxs.y - camMins.y;
	int lineNum = static_cast<int>(camHeight / m_lineHeight);

	sPrintStringMutex.lock();
	std::vector<ColoredLine> strings = m_strings;
	sPrintStringMutex.unlock();

	int coloredLineID = (int)strings.size() - 1;
	Vec2 textBottomLeft = camMins+Vec2(0.f,m_lineHeight);
	camBounds.mins = textBottomLeft;
	std::vector<Vertex_PCU> textVerts;

	for( int lineID = 0; lineID < lineNum && coloredLineID>-1; )
	{
		ColoredLine thisLine = strings[coloredLineID];
		if( font != nullptr )
		{
			int linesConsumed = font->AddVertsForTextInBox2D( textVerts, camBounds, m_lineHeight, 
				thisLine.text, thisLine.color, .8f, ALIGN_BOTTOM_LEFT );
			lineID += linesConsumed;
		}
		else//use default simple triangle font
		{
			AppendTextTriangles2D( textVerts, thisLine.text, textBottomLeft, m_lineHeight,
				thisLine.color );
			lineID++;
		}
		textBottomLeft.y = camMins.y + ((float)lineID+1.2f) * m_lineHeight;
		camBounds.mins = textBottomLeft;
		coloredLineID--;
	}

	if( font != nullptr ){
		font->AddVertsForText2D(textVerts, camMins, m_lineHeight, m_inputLine);
		renderer->BindDiffuseTexture( font->GetTexture() );
	}
	else {
		AppendTextTriangles2D(textVerts, m_inputLine, camMins, m_lineHeight, Rgba8::WHITE);
	}
	renderer->DrawVertexArray( textVerts );

	RenderChoiceCommands(renderer);
	DrawCursor(renderer);

	//Draw selection
	if (m_selectPosCenter >= 0) {
        int startPos = m_selectPosCenter;
        int length = m_selectDeltaPos;
        if (m_selectDeltaPos < 0) {
            startPos = m_selectPosCenter + m_selectDeltaPos;
			length = -m_selectDeltaPos;
        }

		Vec2 selectMins = camMins + Vec2(startPos * m_lineHeight, 0.f);
		Vec2 selectMaxs = camMins + Vec2(m_lineHeight * (startPos + length), m_lineHeight);
		renderer->DrawAABB2D(AABB2(selectMins, selectMaxs), Rgba8(255, 255, 255, 100));
	}

	renderer->EndCamera(m_camera);
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::ClearInput()
{
	m_lastCommandIdx = -1;
	m_inputLine.clear();
	m_input->ClearCharacter();
	m_caretPos = 0;
	ClearSelection();
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::SetIsOpen( bool isOpen )
{
	bool prevOpen = m_isOpen;
	m_isOpen = isOpen;
	
	if(prevOpen!=m_isOpen){
		ClearInput();
        if (m_isOpen) {
            m_input->PushMouseOptions(eMousePositionMode::MOUSE_ABSOLUTE, true, true);
        }
        else {
            m_input->PopMouseOptions();
        }
	}
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::EnterCommand(std::string const& command)
{
	PrintString(Rgba8::BLUE, "> "+command);
	bool called = g_theEvents->FireEvent(command, eEventFlag::EVENT_CONSOLE);
	if (!called) {
		PrintString(Rgba8::RED, "Command execution not successful in console");
	}
	else {
		UpdateCommandHistory(command);
	}
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateCommandHistory(std::string const& newCommand)
{
	Strings commandContent = SplitStringOnDelimiter(Trim(newCommand), ' ');
	std::string command = commandContent[0];

	for (std::vector<std::string>::iterator iter = m_lastCommands.begin(); iter!=m_lastCommands.end(); iter++) {
		if (*iter == command) {
			m_lastCommands.erase(iter);
			break;
		}
	}
	m_lastCommands.push_back(command);
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::UpdateChoiceCommands()
{
	m_choiceCommands.clear();
	size_t inputLength = m_inputLine.size();
	size_t commandLength = 0;
	for (size_t i = 0; i < m_lastCommands.size(); i++) {
		std::string& s = m_lastCommands[i];
		if (s.compare(0, inputLength, m_inputLine) == 0) {
			commandLength = s.size();
			m_choiceCommands.push_back(s);
		}
	}

	if (m_choiceCommands.size() == 1 && commandLength == inputLength) {
		m_choiceCommands.clear();
	}

	if (m_choiceCommands.size() < 1 || m_inputLine.size() < 3) {
		m_showChoices = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::HandleInputEvent()
{
	//filter chars
	if (m_input->IsKeyDown(KEY_TILDE) || m_input->IsKeyDown(KEY_BACKSPACE) || m_input->IsKeyDown(KEY_ENTER)) {
		m_input->ClearCharacter();
	}

	//~ close console
	if (m_input->WasKeyJustPressed(KEY_TILDE))
	{
		m_input->SetKeyButtonState(KEY_TILDE, false);
		SetIsOpen(false);
		return;
	}

	//ESC event in console
	if (m_input->WasKeyJustPressed(KEY_ESC))
	{
		if (m_showChoices)
		{
			m_showChoices = false;
			m_chosenCommandIdx = -1;
		}
        else if (!m_inputLine.empty())
        {
			ClearInput();
        }
		else if (IsOpen())
		{
			SetIsOpen(false);
			m_input->SetKeyButtonState(KEY_ESC, false);
		}
	}

	//command history
	if (m_input->WasKeyJustPressed(KEY_UPARROW))
	{
		if (m_showChoices && m_choiceCommands.size()>0)	{	//choices up
			int lastIdx = (int)m_choiceCommands.size() - 1;
			m_chosenCommandIdx = m_chosenCommandIdx == lastIdx? lastIdx : m_chosenCommandIdx + 1;
		}
		else {
			if (m_lastCommandIdx <= 0) {
				m_lastCommandIdx = (int)m_lastCommands.size() - 1;
			}
			else {
				m_lastCommandIdx -= 1;
			}
			m_inputLine = m_lastCommands[m_lastCommandIdx];
			m_caretPos = (int)m_inputLine.size();
		}
	}
	else if (m_input->WasKeyJustPressed(KEY_DOWNARROW)) {
        if (m_showChoices && m_choiceCommands.size() > 0 && m_chosenCommandIdx > -1)
        {
			m_chosenCommandIdx -= 1;
        }
		else {
			m_lastCommandIdx += 1;
			if (m_lastCommandIdx >= (int)m_lastCommands.size()) {
				m_lastCommandIdx = (int)m_lastCommands.size();
				m_inputLine.clear();
			}
			else {
				m_inputLine = m_lastCommands[m_lastCommandIdx];
			}
			m_caretPos = (int)m_inputLine.size();
		}
	}

	//delete
	if (m_input->IsKeyDown(KEY_DELETE))	{
		bool shouldDo = false;
		if (m_input->WasKeyJustPressed(KEY_DELETE)) {
			m_timer->SetTimerSeconds(m_clock, 1.0);
			shouldDo = true;
		}
		if (m_timer->IsRunning()&&m_timer->HasElapsed()) {
			shouldDo = true;
		}
        if (shouldDo && m_caretPos < (int)m_inputLine.size()) {
            m_inputLine.erase(m_caretPos, 1);
        }
		m_cursorTimer->Reset();
	}

	//backspace
	if (m_input->IsKeyDown(KEY_BACKSPACE)) {
		bool shouldDo = false;
		if (m_input->WasKeyJustPressed(KEY_BACKSPACE)) {
			m_timer->SetTimerSeconds(m_clock, 1.0);
			shouldDo = true;
		}
		if (m_timer->IsRunning() && m_timer->HasElapsed()) {
			shouldDo = true;
		}
        if (shouldDo && m_caretPos > 0) {
            m_caretPos -= 1;
            m_inputLine.erase(m_caretPos, 1);
        }
		m_cursorTimer->Reset();
	}

	//enter command
	if (m_input->WasKeyJustPressed(KEY_ENTER)) {
		if (m_showChoices && m_chosenCommandIdx > -1 && m_choiceCommands.size() > 0) {
			int idx = (int)m_choiceCommands.size() - 1 - m_chosenCommandIdx;
			m_inputLine = m_choiceCommands[idx];
			m_caretPos = (int)m_inputLine.size();
			m_chosenCommandIdx = -1;
			m_showChoices = false;
		}
		else {
			EnterCommand(m_inputLine);
			ClearInput();
		}
	}

	//text selection
	if (m_input->WasKeyJustPressed(KEY_ALT)) {
		m_selectPosCenter = m_caretPos;
		m_selectDeltaPos = 0;
	}
    if (m_input->IsKeyDown(KEY_ALT)) {
		if (m_input->WasKeyJustPressed(KEY_LEFTARROW)) {
			m_selectDeltaPos -= 1;
		}
		if (m_input->WasKeyJustPressed(KEY_RIGHTARROW)) {
			m_selectDeltaPos += 1;
		}
    }
	else {
        //cursor movement
		if (m_input->IsKeyDown(KEY_LEFTARROW)) {
			bool shouldDo = false;
			if (m_input->WasKeyJustPressed(KEY_LEFTARROW))
			{
				m_timer->SetTimerSeconds(m_clock, 1.0);
				shouldDo = true;
			}
            if (m_timer->IsRunning() && m_timer->HasElapsed()) {
                shouldDo = true;
            }
            if (shouldDo && m_caretPos > 0) {
                m_caretPos -= 1;
            }
			m_cursorTimer->Reset();
		}
		if (m_input->IsKeyDown(KEY_RIGHTARROW)) {
			bool shouldDo = false;
			if (m_input->WasKeyJustPressed(KEY_RIGHTARROW))
            {
                m_timer->SetTimerSeconds(m_clock, 1.0);
                shouldDo = true;
            }
            if (m_timer->IsRunning() && m_timer->HasElapsed()) {
                shouldDo = true;
            }
			if (shouldDo && m_caretPos < (int)m_inputLine.size()) {
				m_caretPos += 1;
			}
			m_cursorTimer->Reset();
		}
	}
	m_selectDeltaPos = Clamp(m_selectDeltaPos, -m_selectPosCenter, (int)m_inputLine.size() - m_selectPosCenter);

	//paste from clipboard
	if (m_input->WasHotKeyJustReleased(eHotKey::PASTE)) {
		std::string clipboard = m_input->GetClipboardContent();
		m_inputLine.insert(m_caretPos, clipboard);
		m_caretPos += (int)clipboard.size();
		ClearSelection();
	}

	//copy to clipboard
	if (m_input->WasHotkeyJustPressed(eHotKey::COPY) || m_input->WasHotkeyJustPressed(eHotKey::CUT))
	{
		if (m_selectPosCenter < 0) {
			return;
		}

        int startPos = m_selectPosCenter;
        int length = m_selectDeltaPos;
        if (m_selectDeltaPos < 0) {
            startPos = m_selectPosCenter + m_selectDeltaPos;
			length = -m_selectDeltaPos;
        }

        std::string selected = m_inputLine.substr(startPos, length);

		if (m_input->WasHotkeyJustPressed(eHotKey::COPY)) {
			m_input->SetClipboardContent(selected);
		}
		else{
			m_input->SetClipboardContent(selected);
			m_inputLine.erase(startPos, length);
			m_caretPos = startPos;
			ClearSelection();
		}
	}

	//clear timer
    if (m_input->WasKeyJustReleased(KEY_DELETE) || 
		m_input->WasKeyJustReleased(KEY_BACKSPACE) ||
		m_input->WasKeyJustReleased(KEY_LEFTARROW) ||
		m_input->WasKeyJustReleased(KEY_RIGHTARROW)) {
        m_timer->Stop();
    }
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::ClearSelection()
{
	m_selectDeltaPos = 0;
	m_selectPosCenter = -1;
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::DrawCursor(RenderContext* renderer) const
{
	if (m_cursorTimer->HasElapsed()) {
		double leftTime = m_cursorTimer->GetElapsedSeconds() - 2.0;
		m_cursorTimer->Reset();
		m_cursorTimer->m_startSeconds -= leftTime;
	}

    //cursor draw
	Rgba8 color = Rgba8::WHITE;
	color.a = m_cursorTimer->GetElapsedSeconds() > 1.0 ? 0 : 255;
    float halfHeight = m_lineHeight * .5f;
    float halfWidth = m_lineHeight * .05f;
    Vec2 halfDim = Vec2(halfWidth, halfHeight);
    Vec2 cursorCenter = Vec2((float)m_caretPos * m_lineHeight, m_lineHeight * .5f) + m_camera->GetBounds().mins;
    renderer->BindDiffuseTexture((Texture*)nullptr);
    renderer->DrawAABB2D(AABB2(cursorCenter - halfDim, cursorCenter + halfDim), color);
}

//////////////////////////////////////////////////////////////////////////
void DevConsole::RenderChoiceCommands(RenderContext* renderer) const
{
    size_t choiceCount = m_choiceCommands.size();
	if (m_inputLine.size() < 3 || choiceCount < 1 || !m_showChoices) {
		return;
	}

	//draw background
	AABB2 bounds = m_camera->GetBounds();
	Vec2 dimensions = bounds.GetDimensions() * Vec2(.25f, 0.f);
	dimensions.y = (float)choiceCount * m_lineHeight * 1.2f;
	Vec2 bottomLeft = bounds.mins + Vec2(0.f, m_lineHeight * 1.2f);
	renderer->BindDiffuseTexture((Texture*)nullptr);
	renderer->DrawAABB2D(AABB2(bottomLeft, bottomLeft + dimensions), Rgba8(120, 120, 120, 220));
	//draw choices
	BitmapFont* font = renderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	std::vector<Vertex_PCU> verts;
	for (size_t i = 0; i < choiceCount; i++) {
		Rgba8 color = (int)i == m_chosenCommandIdx ? Rgba8(255, 100, 0) : Rgba8(200, 200, 0);
		std::string choice = m_choiceCommands[choiceCount - i - 1];
		font->AddVertsForText2D(verts, bottomLeft, m_lineHeight, choice, color, 0.9f, .1f);
		bottomLeft.y += m_lineHeight * 1.2f;
	}
	renderer->BindDiffuseTexture(font->GetTexture());
	renderer->DrawVertexArray(verts);
}

//////////////////////////////////////////////////////////////////////////
ColoredLine::ColoredLine( Rgba8 inColor, std::string inText )
	:color(inColor)
	,text(inText)
{
}
