#include "Engine/Input/KeyButtonState.hpp"

//////////////////////////////////////////////////////////////////////////
void KeyButtonState::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}

//////////////////////////////////////////////////////////////////////////
void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

//////////////////////////////////////////////////////////////////////////
bool KeyButtonState::WasJustPressed() const
{
	if( !m_wasPressedLastFrame && m_isPressed )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool KeyButtonState::WasJustReleased() const
{
	if( m_wasPressedLastFrame && !m_isPressed )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
void KeyButtonState::Update()
{
	m_wasPressedLastFrame = m_isPressed;
}
