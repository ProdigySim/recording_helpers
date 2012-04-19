#ifndef THIRDPERSON_PATCH_H
#define THIRDPERSON_PATCH_H

#include "input.h"

// Patch out the PCZ checks in CInput Thirdperson code
void PatchCInputPCZChecks(IInput *input);
// Undo patching of CInput Thirdperson code
void UnpatchCInputPCZChecks(IInput *input);



// attempt to patch the PCZ check out of Thirdpersonshoulder command
//bool PatchThirdpersonShoulderPCZCheck(char * client);

#endif