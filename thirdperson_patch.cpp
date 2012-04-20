/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * Thirdperson Patch
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
#include "thirdperson_patch.h"
#include "memutils.h"


typedef void (*FuncRvEv)(void);
// Function returns bool expects void
typedef bool (*Func_RbEv)(void);
// IInput member func returns int expects int
typedef union {
	int (IInput::*pFunc)(int);
	FuncRvEv pVoidFunc;
	void *pVoid;
	BYTE *pByte;
} IInput_Func_RiEi;
// IInput member func returns void expects void
typedef union  {
	void (IInput::*pFunc)(void);
	FuncRvEv pVoidFunc;
	void * pVoid;
	BYTE * pByte;
} IInput_Func_RvEv;



static IInput_Func_RvEv g_fpCInput_CAM_Think = {NULL};
static IInput_Func_RiEi g_fpCInput_CAM_IsThirdPerson = {NULL};
static IInput_Func_RvEv g_fpCInput_CAM_ToThirdPersonShoulder = {NULL};

static IInput_Func_RiEi g_fpHaxed_CAM_IsThirdPerson = {NULL};
static IInput_Func_RvEv g_fpHaxed_CAM_ToThirdPersonShoulder = {NULL};
static IInput_Func_RvEv g_fpHaxed_CAM_Think = {NULL};

static CInput_vtable g_HaxedVtable;
static CInput_vtable * g_OriginalVtable = NULL;


static IInput_Func_RiEi GenerateCAM_IsThirdPersonFunc(IInput_Func_RiEi origCAM_IsThirdPerson)
{
	static const int PCZCheckOffset = 3;
	IInput_Func_RiEi newFunc;
	
	newFunc.pByte = g_MemUtils.CloneFunction(origCAM_IsThirdPerson.pByte);
	if(newFunc.pByte == NULL) 
	{
		Warning("Couldn't clone CAM_IsThirdPerson function.\n");
		return newFunc;
	}

	if(newFunc.pByte[PCZCheckOffset] != 0xE8)
	{
		Warning("PCZ Check offset in CAM_IsThirdPerson incorrect.\n");
		goto fail;
	}

	// Patch out the PCZCheck in our new function.
	newFunc.pByte[PCZCheckOffset] = '\x33'; // XOR m32,m32
	newFunc.pByte[PCZCheckOffset+1] = '\xC0'; // eax, eax
	// 3 byte NOP
	newFunc.pByte[PCZCheckOffset+2] = '\x0F';
	newFunc.pByte[PCZCheckOffset+3] = '\x1F';
	newFunc.pByte[PCZCheckOffset+4] = '\x00';

	return newFunc;

fail:
	free(newFunc.pByte);
	newFunc.pByte=NULL;
	return newFunc;
}

static ConVar * c_thirdpersonshoulder = NULL;

static bool checkCThirdPersonShoulder()
{
	return c_thirdpersonshoulder->GetBool();
}

static IInput_Func_RvEv GenerateCAM_ToThirdPersonShoulderFunc(IInput_Func_RvEv origCAM_ToThirdPersonShoulder)
{
	static const int PCZCheckOffset = 10;
	IInput_Func_RvEv newFunc;
	
	newFunc.pByte = g_MemUtils.CloneFunction(origCAM_ToThirdPersonShoulder.pByte);
	if(newFunc.pByte == NULL) 
	{
		Warning("Couldn't clone CAM_ToThirdPersonShoulder function.\n");
		return newFunc;
	}
	
	if(newFunc.pByte[PCZCheckOffset] != 0xE8)
	{
		Warning("PCZ Check offset in CAM_ToThirdPersonShoulder incorrect.\n");
		goto fail;
	}
	// Patch out the PCZCheck in our new function.
	newFunc.pByte[PCZCheckOffset] = '\x33'; // XOR m32,m32
	newFunc.pByte[PCZCheckOffset+1] = '\xC0'; // eax, eax
	// 3 byte NOP
	newFunc.pByte[PCZCheckOffset+2] = '\x0F';
	newFunc.pByte[PCZCheckOffset+3] = '\x1F';
	newFunc.pByte[PCZCheckOffset+4] = '\x00';

	return newFunc;

fail:
	free(newFunc.pByte);
	newFunc.pByte=NULL;
	return newFunc;
}

static IInput_Func_RvEv GenerateCAM_ThinkFunc(IInput_Func_RvEv origCAM_Think)
{
	static const int PCZCheckFuncOffset = 152;
	IInput_Func_RvEv newFunc = {NULL};
	c_thirdpersonshoulder = g_pCvar->FindVar("c_thirdpersonshoulder");
	if(c_thirdpersonshoulder = NULL)
	{
		Warning("Couldn't find c_thirdpersonshoulder cvar.\n");
		return newFunc;
	}

	newFunc.pByte = g_MemUtils.CloneFunction(origCAM_Think.pByte);
	if(newFunc.pByte == NULL)
	{
		Warning("Couldn't clone CAM_Think function.\n");
		return newFunc;
	}

	if(newFunc.pByte[PCZCheckFuncOffset] != 0xE8)
	{
		Warning("PCZ Check Func offset in CAM_Think incorrect.\n");
		goto fail;
	}

	// win32 has a subfunction that checks Hasplayercontrolledzombies + c_thirdpersonshoulder var.
	// We replace this with our function which just checks c_thirdpersonshoulder

	// Calculate call jump offset
	int jumpOffset = g_MemUtils.GetCallOrJumpRelOffset(&newFunc.pByte[PCZCheckFuncOffset], reinterpret_cast<BYTE*>(checkCThirdPersonShoulder));

	// Replace it
	*reinterpret_cast<int*>(&newFunc.pByte[PCZCheckFuncOffset+1]) = jumpOffset;

	return newFunc;
fail:
	free(newFunc.pByte);
	newFunc.pByte=NULL;
	return newFunc;
}

void PatchCInputPCZChecks(IInput *input)
{
	CInput_vtable ** pInputInstance = reinterpret_cast<CInput_vtable**>(input);
	CInput_vtable * pInputVtable = *pInputInstance;

	// Copy the old vtable over to our vtable as a starting point
	memcpy(&g_HaxedVtable, pInputVtable, sizeof(CInput_vtable));

	// Copy old function pointers from vtable
	g_fpCInput_CAM_Think.pFunc = pInputVtable->IInput.CAM_Think;
	g_fpCInput_CAM_IsThirdPerson.pFunc = pInputVtable->IInput.CAM_IsThirdPerson;
	g_fpCInput_CAM_ToThirdPersonShoulder.pFunc = pInputVtable->IInput.CAM_ToThirdPersonShoulder;

	// Dev output checks
	DevMsg("CInput::CAM_Think(): %08x\n", g_fpCInput_CAM_Think);
	DevMsg("CInput::CAM_IsThirdPerson(): %08x\n", g_fpCInput_CAM_IsThirdPerson);
	DevMsg("CInput::CAM_ToThirdPersonShoulder(): %08x\n", g_fpCInput_CAM_ToThirdPersonShoulder);
	
	// Backup original vtable
	g_OriginalVtable = pInputVtable;

	// Start customizing our vtable
	
	g_fpHaxed_CAM_IsThirdPerson = GenerateCAM_IsThirdPersonFunc(g_fpCInput_CAM_IsThirdPerson);
	if(g_fpHaxed_CAM_IsThirdPerson.pVoid == NULL) goto fail;
	g_HaxedVtable.IInput.CAM_IsThirdPerson = g_fpHaxed_CAM_IsThirdPerson.pFunc;

	g_fpHaxed_CAM_ToThirdPersonShoulder = GenerateCAM_ToThirdPersonShoulderFunc(g_fpCInput_CAM_ToThirdPersonShoulder);
	if(g_fpHaxed_CAM_ToThirdPersonShoulder.pVoid == NULL) goto fail;
	g_HaxedVtable.IInput.CAM_ToThirdPersonShoulder = g_fpHaxed_CAM_ToThirdPersonShoulder.pFunc;

	g_fpHaxed_CAM_Think = GenerateCAM_ThinkFunc(g_fpCInput_CAM_Think);
	if(g_fpHaxed_CAM_Think.pVoid == NULL) goto fail;
	g_HaxedVtable.IInput.CAM_Think = g_fpHaxed_CAM_Think.pFunc;

	// Dev output checks
	DevMsg("Haxed CAM_IsThirdPerson(): %08x\n", g_fpHaxed_CAM_IsThirdPerson.pVoid);
	DevMsg("Haxed CAM_ToThirdPersonShoulder(): %08x\n", g_fpHaxed_CAM_ToThirdPersonShoulder.pVoid);
	DevMsg("Haxed CAM_Think(): %08x\n", g_fpHaxed_CAM_Think.pVoid);

	// Override the global CInput instance's vtable with our haxed one
	*pInputInstance = &g_HaxedVtable;

	return;

fail:
	Warning("Failed to Patch Thirdperson function calls. Bailing out.\n");
	UnpatchCInputPCZChecks(input);
}

void UnpatchCInputPCZChecks(IInput *input)
{
	CInput_vtable ** pInputInstance = reinterpret_cast<CInput_vtable**>(input);
	if(g_OriginalVtable != NULL)
	{
		*pInputInstance = g_OriginalVtable;
	}

	if(g_fpHaxed_CAM_IsThirdPerson.pVoid != NULL)
	{
		free(g_fpHaxed_CAM_IsThirdPerson.pVoid);
	}

	if(g_fpHaxed_CAM_ToThirdPersonShoulder.pVoid != NULL)
	{
		free(g_fpHaxed_CAM_ToThirdPersonShoulder.pVoid);
	}

	if(g_fpHaxed_CAM_Think.pVoid != NULL)
	{
		free(g_fpHaxed_CAM_Think.pVoid);
	}
}


// Can't do this because of CEG.
// Hence curse words in rest of code.
/*
const char g_sCInput_CamToThirdPersonShoulder_pattern[] = "\x56\x6A\xFF\xE8\x2A\x2A\x2A\x2A\x8B\xF0\xE8\x2A\x2A\x2A\x2A\x84\xC0\x74\x07\xA0\x2A\x2A\x2A\x2A\xEB\x0C\xA1\x2A\x2A\x2A\x2A\x83\x78\x30\x00\x0F\x95\xC0\x84\xC0\xB9";
const int g_iPatchPCZCheckOffset = 10;
unsigned char g_sPCZBackup[5];
const char g_sPCZPatch[] = "\x31\xC0\x90\x90\x90";

bool PatchThirdpersonShoulderPCZCheck(char * client)
{
	char *pAddr=NULL;
	pAddr = (char*)g_MemUtils.FindPattern(client-0x300000, client+0x300000, g_sCInput_CamToThirdPersonShoulder_pattern, sizeof(g_sCInput_CamToThirdPersonShoulder_pattern)-1);
	if(pAddr == NULL)
	{
		Warning("Couldn't find thirdpersonshoulder function!\n");
		return false;
	}
	
	pAddr+=g_iPatchPCZCheckOffset;
	memcpy(g_sPCZBackup, pAddr, 5);
	DevMsg("Old Patch: %02x%02x%02x%02x%02x\n", g_sPCZBackup[0], g_sPCZBackup[1], g_sPCZBackup[2], g_sPCZBackup[3], g_sPCZBackup[4]);

	if(!g_MemUtils.SetMemPatchable(pAddr,5))
	{
		Warning("Failed to set memory patchable in thirdpersonshoulder function!\n");
		return false;
	}
	memcpy(pAddr, g_sPCZPatch, 5);

	return true;
};
*/