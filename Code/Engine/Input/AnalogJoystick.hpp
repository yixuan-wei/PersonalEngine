#pragma once
#include "Engine/Math/Vec2.hpp"

class AnalogJoystick
{
	friend class XboxController;

public:
	explicit AnalogJoystick( float innerDeadZoneFraction, float outterDeadZoneFraction );

	Vec2  GetPosition() const               { return m_correctedPosition; }
	Vec2  GetRawPosition() const            { return m_rawPosition; }
	float GetMagnitude() const              { return m_correctedMagnitude; }
	float GetAngleDegrees() const           { return m_correctedDegrees; }
	float GetInnerDeadZoneFraction() const  { return m_innerDeadZoneFraction; }
	float GetOutterDeadZoneFraction() const { return m_outterDeadZoneFraction; }

	void  Reset();

private:
	void UpdatePosition( float rawNormalizedX, float rawNormalizedY );

private:
	const float m_innerDeadZoneFraction;
	const float m_outterDeadZoneFraction;
	Vec2        m_rawPosition        = Vec2( 0.0f, 0.0f ); //x, y in[-1,1]
	Vec2        m_correctedPosition  = Vec2( 0.0f, 0.0f ); //x, y in[-1,1]
	float       m_correctedDegrees   = 0.f;
	float       m_correctedMagnitude = 0.f;
};