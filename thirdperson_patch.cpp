#include "thirdperson_patch.h"

static void (IInput::*g_fpCInput_CAM_Think)(void) = NULL;
static int (IInput::*g_fpCInput_CAM_IsThirdPerson)(int) = NULL;
static void (IInput::*g_fpCInput_CAM_ToThirdPersonShoulder)(void) = NULL;
static CInput_vtable g_HaxedVtable;
static CInput_vtable * g_OriginalVtable = NULL;

class CInputFucked
{
public:
	void CAM_Think( void )
	{
		(reinterpret_cast<IInput*>(this)->*g_fpCInput_CAM_Think)();
	}
	int CAM_IsThirdPerson( int nSlot = -1 )
	{
		return (reinterpret_cast<IInput*>(this)->*g_fpCInput_CAM_IsThirdPerson)(nSlot);
	}
	void CAM_ToThirdPersonShoulder(void)
	{
		DevMsg("Sufficiently Haxed!!!\n");
		(reinterpret_cast<IInput*>(this)->*g_fpCInput_CAM_ToThirdPersonShoulder)();
	}

};

void PatchCInputPCZChecks(IInput *input)
{
	CInput_vtable ** pInputInstance = reinterpret_cast<CInput_vtable**>(input);
	CInput_vtable * pInputVtable = *pInputInstance;

	// Copy the old vtable over to our vtable as a starting point
	memcpy(&g_HaxedVtable, pInputVtable, sizeof(CInput_vtable));

	// Copy old function pointers from vtable
	g_fpCInput_CAM_Think = pInputVtable->IInput.CAM_Think;
	g_fpCInput_CAM_IsThirdPerson = pInputVtable->IInput.CAM_IsThirdPerson;
	g_fpCInput_CAM_ToThirdPersonShoulder = pInputVtable->IInput.CAM_ToThirdPersonShoulder;

	// Dev output checks
	DevMsg("CInput::CAM_Think(): %08x\n", g_fpCInput_CAM_Think);
	DevMsg("CInput::CAM_IsThirdPerson(): %08x\n", g_fpCInput_CAM_IsThirdPerson);
	DevMsg("CInput::CAM_ToThirdPersonShoulder(): %08x\n", g_fpCInput_CAM_ToThirdPersonShoulder);
	
	// Backup original vtable
	g_OriginalVtable = pInputVtable;

	// Start customizing our vtable
	g_HaxedVtable.IInput.CAM_Think = reinterpret_cast<void (IInput::*)(void)>(&CInputFucked::CAM_Think);
	g_HaxedVtable.IInput.CAM_IsThirdPerson = reinterpret_cast<int (IInput::*)(int)>(&CInputFucked::CAM_IsThirdPerson);
	g_HaxedVtable.IInput.CAM_ToThirdPersonShoulder = reinterpret_cast<void (IInput::*)(void)>(&CInputFucked::CAM_ToThirdPersonShoulder);

	// Dev output checks
	DevMsg("CInputFucked::CAM_Think(): %08x\n", g_HaxedVtable.IInput.CAM_Think);
	DevMsg("CInputFucked::CAM_IsThirdPerson(): %08x\n", g_HaxedVtable.IInput.CAM_IsThirdPerson);
	DevMsg("CInputFucked::CAM_ToThirdPersonShoulder(): %08x\n", g_HaxedVtable.IInput.CAM_ToThirdPersonShoulder);

	// Override the global CInput instance's vtable with our haxed one
	*pInputInstance = &g_HaxedVtable;
}

void UnpatchCInputPCZChecks(IInput *input)
{
	CInput_vtable ** pInputInstance = reinterpret_cast<CInput_vtable**>(input);
	*pInputInstance = g_OriginalVtable;
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