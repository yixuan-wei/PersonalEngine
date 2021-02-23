#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0")

//////////////////////////////////////////////////////////////////////////
XboxController::XboxController( int controllerID )
	:m_controllerID(controllerID)
{

}

//////////////////////////////////////////////////////////////////////////
const KeyButtonState& XboxController::GetButtonState( eXboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

//////////////////////////////////////////////////////////////////////////
void XboxController::SetVibrationValue( float leftMotorValue, float rightMotorValue )
{
	m_leftMotorSpeed = Clamp( leftMotorValue, 0.f, 1.f );
	m_rightMotorSpeed = Clamp( rightMotorValue, 0.f, 1.f );
}

//////////////////////////////////////////////////////////////////////////
void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );
	
	if( errorStatus == ERROR_SUCCESS )
	{
		m_isConnected = true;
		XINPUT_GAMEPAD& gamepad = xboxControllerState.Gamepad;

		UpdateJoystick( m_leftJoystick,     gamepad.sThumbLX, gamepad.sThumbLY );
		UpdateJoystick( m_rightJoystick,    gamepad.sThumbRX, gamepad.sThumbRY );

		UpdateTrigger( m_leftTriggerValue,  gamepad.bLeftTrigger );
		UpdateTrigger( m_rightTriggerValue, gamepad.bRightTrigger );

		UpdateButton( XBOX_BUTTON_ID_A,          gamepad.wButtons, XINPUT_GAMEPAD_A );
		UpdateButton( XBOX_BUTTON_ID_B,          gamepad.wButtons, XINPUT_GAMEPAD_B );
		UpdateButton( XBOX_BUTTON_ID_X,          gamepad.wButtons, XINPUT_GAMEPAD_X );
		UpdateButton( XBOX_BUTTON_ID_Y,          gamepad.wButtons, XINPUT_GAMEPAD_Y );
		UpdateButton( XBOX_BUTTON_ID_BACK,       gamepad.wButtons, XINPUT_GAMEPAD_BACK );
		UpdateButton( XBOX_BUTTON_ID_START,      gamepad.wButtons, XINPUT_GAMEPAD_START );
		UpdateButton( XBOX_BUTTON_ID_LSHOULDER,  gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER );
		UpdateButton( XBOX_BUTTON_ID_RSHOULDER,  gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER );
		UpdateButton( XBOX_BUTTON_ID_LTHUMB,     gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB );
		UpdateButton( XBOX_BUTTON_ID_RTHUMB,     gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB );
		UpdateButton( XBOX_BUTTON_ID_DPAD_RIGHT, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT );
		UpdateButton( XBOX_BUTTON_ID_DPAD_UP,    gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP );
		UpdateButton( XBOX_BUTTON_ID_DPAD_LEFT,  gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT );
		UpdateButton( XBOX_BUTTON_ID_DPAD_DOWN,  gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN );

		//set vibration
        XINPUT_VIBRATION vInfo;
        vInfo.wLeftMotorSpeed = static_cast<WORD>(65535.f * m_leftMotorSpeed); // accepting unsigned short, left is shaking low frequency
        vInfo.wRightMotorSpeed = static_cast<WORD>(65535.f * m_rightMotorSpeed); //0-65535, right is buzzing high frequency
        XInputSetState(m_controllerID, &vInfo);
        
	}
	else if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		Reset();
		m_isConnected = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void XboxController::Reset()
{
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	for( int buttonID = 0; buttonID < NUM_XBOX_BUTTONS; buttonID++ )
	{
		m_buttonStates[buttonID].Reset();
	}
	m_leftTriggerValue = 0.f;
	m_rightTriggerValue = 0.f;
	m_leftMotorSpeed = 0.f;
	m_rightMotorSpeed = 0.f;
}

//////////////////////////////////////////////////////////////////////////
void XboxController::UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY )
{
	float rawNormalizedX = RangeMapFloat( -32768.f, 32767.f, -1.f, 1.f, static_cast<float>(rawX) );
	float rawNormalizedY = RangeMapFloat( -32768.f, 32767.f, -1.f, 1.f, static_cast<float>(rawY) );
	joystick.UpdatePosition( rawNormalizedX, rawNormalizedY );
}

//////////////////////////////////////////////////////////////////////////
void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	triggerValue = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, static_cast<float>(rawValue) );
}

//////////////////////////////////////////////////////////////////////////
void XboxController::UpdateButton( eXboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag )
{
	bool buttonStatus = buttonFlags & buttonFlag;
	m_buttonStates[buttonID].UpdateStatus( buttonStatus );
}
