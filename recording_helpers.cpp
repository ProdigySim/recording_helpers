#include <cstdlib>
#include "convar_sm_l4d.h"
#include "eiface.h"
#include "icvar.h"
#include "tier1/iconvar.h"
//#include "memutils.h"
#include "input.h"
#include "thirdperson_patch.h"

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

class RecordingHelpers: public IServerPluginCallbacks, public IConCommandBaseAccessor
{
public:
	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientFullyConnect( edict_t *pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );

	// added with version 3 of the interface.
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict  );	

	// IConCommandBaseAccessor
	virtual bool RegisterConCommandBase( ConCommandBase *pVar );

};


RecordingHelpers g_RecordingHelpersPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(RecordingHelpers, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_RecordingHelpersPlugin );

ICvar * g_pCvar = NULL;
IInput * g_pInput = NULL;


// Find the global IInput instance (CInput actually)
IInput * GetGlobalIInput();
// Remove FCVAR_DEVELOPMENTONLY from all cvars
void RemoveDevFlags();

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void RecordingHelpers::GameFrame( bool simulating )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool RecordingHelpers::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	g_pCvar = reinterpret_cast<ICvar *>(interfaceFactory(CVAR_INTERFACE_VERSION,NULL));
	
	if(g_pCvar == NULL)
	{
		Warning("RecordingHelpers: Failed to get Cvar interface.\n");
		return false;
	}

	DevMsg("Found g_pCVar at %08x\n", g_pCvar);
	
	g_pInput = GetGlobalIInput();

	DevMsg("Found g_pInput at %08x\n", g_pInput);

	PatchCInputPCZChecks(g_pInput);

	DevMsg("Patched Input Checks\n");

	RemoveDevFlags();

	ConVar_Register(0, this);

	DevMsg("Registered CVars and Commands\n");

	Msg("RecordingHelpers Loaded Successfully.\n");
	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void RecordingHelpers::Unload( void )
{
	UnpatchCInputPCZChecks(g_pInput);
	ConVar_Unregister( );
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void RecordingHelpers::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void RecordingHelpers::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *RecordingHelpers::GetPluginDescription( void )
{
	return "RecordingHelpers 0.1, ProdigySim";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void RecordingHelpers::LevelInit( char const *pMapName )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void RecordingHelpers::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void RecordingHelpers::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void RecordingHelpers::ClientActive( edict_t *pEntity )
{
}

void RecordingHelpers::ClientFullyConnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void RecordingHelpers::ClientDisconnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void RecordingHelpers::ClientPutInServer( edict_t *pEntity, char const *playername )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void RecordingHelpers::SetCommandClient( int index )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void RecordingHelpers::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT RecordingHelpers::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT RecordingHelpers::ClientCommand( edict_t *pEntity, const CCommand &args )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT RecordingHelpers::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void RecordingHelpers::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{
}
void RecordingHelpers::OnEdictAllocated( edict_t *edict )
{
}
void RecordingHelpers::OnEdictFreed( const edict_t *edict  )
{
}

bool RecordingHelpers::RegisterConCommandBase( ConCommandBase *pVar )
{
	pVar->SetNext(NULL);
	g_pCvar->RegisterConCommand(pVar);
	return true;
}

IInput * GetGlobalIInput()
{
	ConCommand * pCmdThirdPersonShoulder = g_pCvar->FindCommand("thirdpersonshoulder");
	if(pCmdThirdPersonShoulder == NULL)
	{
		Warning("Couldn't find thirdpersonshoulder command.\n");
		return NULL;
	}
	char * pAddr = (char*)pCmdThirdPersonShoulder->GetCallback();
	if(pAddr == NULL)
	{
		Warning("Couldn't read ThirdPersonShoulder callback\n");
		return NULL;
	}

	// First instruction of this command is mov ecx, offset input (g_pInput)
	// so to bytes into it is the IInput *
	return **(IInput***)(pAddr + 2);
}

void RemoveDevFlags()
{
	ICvar::Iterator iter(g_pCvar); 
	for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	{  
		ConCommandBase *cmd = iter.Get();
		cmd->RemoveFlags(FCVAR_DEVELOPMENTONLY);
	}
}

