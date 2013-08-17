/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * Recording Helpers
 * Copyright (C) 2012 ProdigySim
 * All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, the authors give you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 */
#include "recording_helpers.h"
#ifdef L4D2
#include "thirdperson_patch.h"
#endif

RecordingHelpers g_RecordingHelpersPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(RecordingHelpers, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_RecordingHelpersPlugin );

ICvar * g_pCvar = NULL;

// Remove FCVAR_DEVELOPMENTONLY from all cvars
void RemoveDevFlags();

#ifdef L4D2
IInput * g_pInput = NULL;
// Find the global IInput instance (CInput actually)
IInput * GetGlobalIInput();
#endif

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

#ifdef L4D2
	DevMsg("Found g_pCVar at %08x\n", g_pCvar);
	
	g_pInput = GetGlobalIInput();

	DevMsg("Found g_pInput at %08x\n", g_pInput);

	PatchCInputPCZChecks(g_pInput);

	DevMsg("Patched Input Checks\n");
#endif
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
#ifdef L4D2
	UnpatchCInputPCZChecks(g_pInput);
#endif
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
	return "RecordingHelpers 0.5, ProdigySim";
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

#ifdef L4D2
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
#endif

void RemoveDevFlags()
{
	ICvar::Iterator iter(g_pCvar); 
	for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	{  
		ConCommandBase *cmd = iter.Get();
		cmd->RemoveFlags(FCVAR_DEVELOPMENTONLY);
	}
}

