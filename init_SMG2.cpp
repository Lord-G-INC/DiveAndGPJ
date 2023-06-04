#include "revolution.h"
#include "kamek/hooks.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/GamePadUtil.h"

namespace {
    extern "C" {
        // Upper two functions are not in the symbol map and need to be added seperately.
        extern bool trySquatJump__5MarioFv(int); // 0x8039B1B0
        extern bool isSlipPolygon__11MarioModuleCFPC8Triangle(int); // 0x803EF7C0
        extern void tryJump__5MarioFv(int);
    }
    int thisIsRequiredForSomeReason;
    bool isDiveAlready = false;
    bool isGPJ = false;

    void checkIfDiveAllowed () {
        if (isGPJ) 
            isGPJ = false; // Reset GPJ
        if (!MR::isOnGroundPlayer() && !isDiveAlready && MR::testSubPadTriggerC(0)) {
            trySquatJump__5MarioFv(thisIsRequiredForSomeReason);
            isDiveAlready = true;
        } else if (MR::isOnGroundPlayer() && isDiveAlready) 
            isDiveAlready = false; // Reset Dive
    }
    void getCurrentVar (int a1) {
        // This variable changes depending on the stage, but is required in the jumping functions.
        thisIsRequiredForSomeReason = a1;
        if (true) // To b instead of bl
            isSlipPolygon__11MarioModuleCFPC8Triangle(a1);
    }
    // Shoutouts to Evanbowl
    float tripleJumpValue = 32.0f;
    asm float setJumpHeight() {
        lis r3, isGPJ@ha
        lbz r0, isGPJ@l(r3)
        cmpwi r0, 0
        beq doDefaultJump
        lis r3, tripleJumpValue@ha
        lfs f1, tripleJumpValue@l(r3)    
        blr
    doDefaultJump:
        fmr f1, f30
        blr
    }
    asm void doGPJAnime () {
        lis r3, isGPJ@ha
        lbz r0, isGPJ@l(r3)
        cmpwi r0, 0
        beq doDefaultJump
        li r0, 2
        blr
    doDefaultJump:
        lwz r0, 0x42C(r27)
        blr
    }

    void doGPJHeight () {
        isGPJ = true;
        tryJump__5MarioFv(thisIsRequiredForSomeReason);
    }
    kmCall(0x8039E124, doGPJHeight);
    kmCall(0x803998A4, doGPJAnime);
    kmCall(0x803997DC, setJumpHeight); // SMG1: 0x802E2178
    kmCall(0x803B254C, getCurrentVar);
    kmBranch(0x80388DA0, checkIfDiveAllowed);
    
    // Change Camera Reset Button to D-Pad Down
    kmWrite32(0x8011E058, 0x4BF0C319); // bl MR::testCorePadTriggerDown()
}
