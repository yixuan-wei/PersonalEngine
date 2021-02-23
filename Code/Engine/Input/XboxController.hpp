#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"


enum eXboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,

	NUM_XBOX_BUTTONS
};


class XboxController
{
	friend class InputSystem;
public:
	explicit XboxController(int controllerID);
	~XboxController() = default;

	bool                  IsConnected() const      { return m_isConnected; }
	int                   GetControllerID() const  { return m_controllerID; }
	float                 GetLeftTrigger() const   { return m_leftTriggerValue; }
	float                 GetRightTrigger() const  { return m_rightTriggerValue; }
	const AnalogJoystick& GetLeftJoystick() const  { return m_leftJoystick; }
	const AnalogJoystick& GetRightJoystick() const { return m_rightJoystick; }
	const KeyButtonState& GetButtonState( eXboxButtonID buttonID ) const;

	void                  SetVibrationValue( float leftMotorValue, float rightMotorValue );

private:
	void Update();
	void Reset();

	void UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY );
	void UpdateTrigger( float& triggerValue, unsigned char rawValue );
	void UpdateButton( eXboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag );

private:
	const int      m_controllerID  = -1;
	bool           m_isConnected   = false;
	AnalogJoystick m_leftJoystick  = AnalogJoystick( 0.3f, 0.95f );
	AnalogJoystick m_rightJoystick = AnalogJoystick( 0.3f, 0.95f );
	KeyButtonState m_buttonStates[NUM_XBOX_BUTTONS];
	float          m_leftTriggerValue  = 0.f; //[0,1]
	float          m_rightTriggerValue = 0.f; //[0,1]
	float          m_leftMotorSpeed = 0.f;//[0,1], left is shaking low frequency
	float          m_rightMotorSpeed = 0.f;//[0,1], right is buzzing high frequency
};