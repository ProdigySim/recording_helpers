//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

/* Adapted from hl2sdk-l4d2 and hl2sdk-swarm, and RE of l4d2 client bin 
	See game/client/iinput.h */

#ifndef IINPUT_H
#define IINPUT_H

#include "mathlib\mathlib.h"
#include "inputsystem\ButtonCode.h"
#include "mathlib/vector.h"


class bf_write;
class bf_read;
class C_BaseCombatWeapon;
class CUserCmd;
struct kbutton_t;

struct CameraThirdData_t
{
	float	m_flPitch;
	float	m_flYaw;
	float	m_flDist;
	float	m_flLag;
	Vector	m_vecHullMin;
	Vector	m_vecHullMax;
};


class IInput
{
public:

	virtual	void		Init_All( void ) = 0;
	virtual void		Shutdown_All( void ) = 0;

	virtual int			GetButtonBits( int ) = 0; // L4D2, Swarm has bool

	virtual void		CreateMove ( int sequence_number, float input_sample_frametime, bool active ) = 0;
	virtual void		ExtraMouseSample( float frametime, bool active ) = 0;

	// These 4 from swarm, not l4d2
	virtual bool		WriteUsercmdDeltaToBuffer( int nSlot, bf_write *buf, int from, int to, bool isnewcommand ) = 0;
	virtual void		EncodeUserCmdToBuffer( int nSlot, bf_write& buf, int slot ) = 0;
	virtual void		DecodeUserCmdFromBuffer( int nSlot, bf_read& buf, int slot ) = 0;
	
	virtual CUserCmd	*GetUserCmd( int nSlot, int sequence_number ) = 0;

	virtual void		MakeWeaponSelection( C_BaseCombatWeapon *weapon ) = 0;
	
	virtual float		KeyState ( kbutton_t *key ) = 0;
	virtual int			KeyEvent( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding ) = 0;
	virtual kbutton_t	*FindKey( const char *name ) = 0;

	virtual void		ControllerCommands( void ) = 0;

	virtual bool		ControllerModeActive() = 0; 
	
	virtual void		Joystick_Advanced( bool bSilent ) = 0; // Swarm

	virtual void		Joystick_SetSampleTime( float frametime ) = 0;
	virtual void		IN_SetSampleTime( float frametime ) = 0;

	virtual void		AccumulateMouse( int nSlot ) = 0; // swarm

	virtual void		ActivateMouse( void ) = 0;
	virtual void		DeactivateMouse( void ) = 0;

	virtual void		ClearStates( void ) = 0;
	virtual float		GetLookSpring( void ) = 0;

	virtual void		GetFullscreenMousePos( int *mx, int *my, int *unclampedx = 0, int *unclampedy = 0 ) = 0;
	virtual void		SetFullscreenMousePos( int mx, int my ) = 0;
	virtual void		ResetMouse( void ) = 0;
	virtual	float		GetLastForwardMove( void ) = 0;

	virtual void		CAM_Think( void ) = 0;
	virtual int			CAM_IsThirdPerson( int nSlot = -1 ) = 0;
	virtual void		CAM_GetCameraOffset( Vector& ofs ) = 0;
	virtual void		CAM_ToThirdPerson(void) = 0;
	virtual void		CAM_ToFirstPerson(void) = 0;
	virtual void		CAM_ToThirdPersonShoulder(void) = 0; // swarm
	virtual void		CAM_StartMouseMove(void) = 0;
	virtual void		CAM_EndMouseMove(void) = 0;
	virtual void		CAM_StartDistance(void) = 0;
	virtual void		CAM_EndDistance(void) = 0;
	virtual int			CAM_InterceptingMouse( void ) = 0;
	virtual	void		CAM_Command( int command ) = 0; // swarm

	virtual void		CAM_ToOrthographic() = 0;
	virtual	bool		CAM_IsOrthographic() const = 0;
	virtual	void		CAM_OrthographicSize( float& w, float& h ) const = 0;

	virtual void		LevelInit( void ) = 0;

	virtual void		ClearInputButton( int bits ) = 0;

	virtual	void		CAM_SetCameraThirdData( CameraThirdData_t *pCameraData, const QAngle &vecCameraOffset ) = 0;
	virtual void		CAM_CameraThirdThink( void ) = 0;
};

class CInput : public IInput
{
public: // CInput adds
	virtual	void		CheckPaused( CUserCmd *cmd ) = 0;
	virtual	void		CheckSplitScreenMimic( int nSlot, CUserCmd *cmd, CUserCmd *pPlayer0Command ) = 0;
protected:
	virtual void		Init_Camera( void ) = 0;
	virtual void		ApplyMouse( int nSlot, QAngle& viewangles, CUserCmd *cmd, float mouse_x, float mouse_y ) = 0;
	/* Bunch of joystick commands are missing here */
	virtual void		GetWindowCenter( int&x, int& y ) = 0;
};

struct IInput_iface
{
	void (IInput::*Init_All)( void );
	void (IInput::*Shutdown_All)( void );

	int (IInput::*GetButtonBits)( int ); // L4D2, Swarm has bool

	void (IInput::*CreateMove)( int sequence_number, float input_sample_frametime, bool active );
	void (IInput::*ExtraMouseSample)( float frametime, bool active );

	// These 4 from swarm, not l4d2
	bool (IInput::*WriteUsercmdDeltaToBuffer)( int nSlot, bf_write *buf, int from, int to, bool isnewcommand );
	void (IInput::*EncodeUserCmdToBuffer)( int nSlot, bf_write& buf, int slot );
	void (IInput::*DecodeUserCmdFromBuffer)( int nSlot, bf_read& buf, int slot );
	
	CUserCmd * (IInput::*GetUserCmd)( int nSlot, int sequence_number );

	void (IInput::*MakeWeaponSelection)( C_BaseCombatWeapon *weapon );
	
	float (IInput::*KeyState)( kbutton_t *key );
	int (IInput::*KeyEvent)( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding );
	kbutton_t * (IInput::*FindKey)( const char *name );

	void (IInput::*ControllerCommands)( void );

//	bool (IInput::*ControllerModeActive)( void ); // Function removed again in 2118 for whatever reason.
	
	void (IInput::*Joystick_Advanced)( bool bSilent ); // Swarm

	void (IInput::*Joystick_SetSampleTime)( float frametime );
	void (IInput::*IN_SetSampleTime)( float frametime );

	void (IInput::*AccumulateMouse)( int nSlot ); // swarm

	void (IInput::*ActivateMouse)( void );
	void (IInput::*DeactivateMouse)( void );

	void (IInput::*ClearStates)( void );
	float (IInput::*GetLookSpring)( void );

	void (IInput::*GetFullscreenMousePos)( int *mx, int *my, int *unclampedx, int *unclampedy );
	void (IInput::*SetFullscreenMousePos)( int mx, int my );
	void (IInput::*ResetMouse)( void );
	float (IInput::*GetLastForwardMove)( void );

	void (IInput::*CAM_Think)( void );
	int (IInput::*CAM_IsThirdPerson)( int nSlot );
	void (IInput::*CAM_GetCameraOffset)( Vector& ofs );
	void (IInput::*CAM_ToThirdPerson)(void);
	void (IInput::*CAM_ToFirstPerson)(void);
	void (IInput::*CAM_ToThirdPersonShoulder)(void); // swarm
	void (IInput::*CAM_StartMouseMove)(void);
	void (IInput::*CAM_EndMouseMove)(void);
	void (IInput::*CAM_StartDistance)(void);
	void (IInput::*CAM_EndDistance)(void);
	int (IInput::*CAM_InterceptingMouse)( void );
	void (IInput::*CAM_Command)( int command ); // swarm

	void (IInput::*CAM_ToOrthographic)();
	bool (IInput::*CAM_IsOrthographic)() const;
	void (IInput::*CAM_OrthographicSize)( float& w, float& h ) const;

	void (IInput::*LevelInit)( void );

	void (IInput::*ClearInputButton)( int bits );

	void (IInput::*CAM_SetCameraThirdData)( CameraThirdData_t *pCameraData, const QAngle &vecCameraOffset );
	void (IInput::*CAM_CameraThirdThink)( void );
};

struct CInput_iface
{
	void (CInput::*CheckPaused)( CUserCmd *cmd );
	void (CInput::*CheckSplitScreenMimic)( int nSlot, CUserCmd *cmd, CUserCmd *pPlayer0Command );
	void (CInput::*Init_Camera)( void );
	void (CInput::*ApplyMouse)( int nSlot, QAngle& viewangles, CUserCmd *cmd, float mouse_x, float mouse_y );
	/* Bunch of joystick commands are missing here */
	void (CInput::*GetWindowCenter)( int&x, int& y );
};

struct CInput_vtable
{
	IInput_iface IInput;
	CInput_iface CInput;

};

#endif // IINPUT_H