#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
AnalogJoystick::AnalogJoystick( float innerDeadZoneFraction, float outterDeadZoneFraction )
	: m_innerDeadZoneFraction(innerDeadZoneFraction)
	, m_outterDeadZoneFraction(outterDeadZoneFraction)
{

}

//////////////////////////////////////////////////////////////////////////
void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2::ZERO;
	m_correctedPosition = Vec2::ZERO;
	m_correctedDegrees = 0.f;
	m_correctedMagnitude = 0.f;
}

//////////////////////////////////////////////////////////////////////////
void AnalogJoystick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_rawPosition = Vec2( rawNormalizedX, rawNormalizedY );	

	m_correctedDegrees = m_rawPosition.GetAngleDegrees();

	float rawMagnitude = m_rawPosition.GetLength();
	rawMagnitude = Clamp( rawMagnitude, m_innerDeadZoneFraction, m_outterDeadZoneFraction );
	m_correctedMagnitude = RangeMapFloat( m_innerDeadZoneFraction, m_outterDeadZoneFraction, 0.f, 1.f, rawMagnitude );

	m_correctedPosition = Vec2::MakeFromPolarDegrees( m_correctedDegrees, m_correctedMagnitude );	
}
