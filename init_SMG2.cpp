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
    float setJumpHeight () {
        // The Triple Jump value is technically a little over 32, but it would get rounded down anyway.
        if (isGPJ) 
            return 32.0f;
        else 
            return 0.0f; // Replaced by kmWrites
    }
    void doGPJAnime () {
        if (isGPJ) 
            asm("nop"); // Replaced by kmWrite
        else 
            asm("nop"); // Replaced by kmWrite
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
    
    // Inline Assembly and CodeWarrior are archenemies, so instead a few kmWrites will have to do.
    
    // Else clause in setJumpHeight
    kmWrite32(0x807FCF64, 0x60000000); // nop
    kmWrite32(0x807FCF68, 0xFC20F090); // fmr f1, f30
    
    // nops in doGPJAnime
    kmWrite32(0x807FCF80, 0x38000002); // li r0, 2
    kmWrite32(0x807FCF88, 0x801B042C); // lwz r0, 0x42C(r27)

    // Change Camera Reset Button to D-Pad Down
    kmWrite32(0x8011E058, 0x4BF0C319); // bl MR::testCorePadTriggerDown()
}