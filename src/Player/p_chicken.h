//
// p_chicken.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "p_types.h"

// Dummy anim to catch sequence leaks.
extern panimmove_t chickenp_move_dummy;

extern panimmove_t chickenp_move_stand;
extern panimmove_t chickenp_move_stand1;
extern panimmove_t chickenp_move_stand2;
extern panimmove_t chickenp_move_walk;
extern panimmove_t chickenp_move_run;
extern panimmove_t chickenp_move_back;
extern panimmove_t chickenp_move_runb;
extern panimmove_t chickenp_move_strafel;
extern panimmove_t chickenp_move_strafer;
extern panimmove_t chickenp_move_jump;
extern panimmove_t chickenp_move_wjump;
extern panimmove_t chickenp_move_rjump;
extern panimmove_t chickenp_move_rjumpb;
extern panimmove_t chickenp_move_jump_loop;
extern panimmove_t chickenp_move_attack;
extern panimmove_t chickenp_move_jump_flap;
extern panimmove_t chickenp_move_runattack;
extern panimmove_t chickenp_move_swim_idle;
extern panimmove_t chickenp_move_swim;

void PlayerChickenBite(playerinfo_t* info);
void PlayerChickenJump(playerinfo_t* info); //mxd. Returned int in original version.

void PlayerChickenFlap(playerinfo_t* info);
void PlayerChickenCheckFlap(playerinfo_t* info);

void PlayerChickenAssert(playerinfo_t* info); //mxd. Named ChickenAssert in original version.
void PlayerChickenCluck(const playerinfo_t* info, float force);

void PlayerChickenStepSound(const playerinfo_t* info, float value); //mxd. Named ChickenStepSound in original version.