//
// m_ogle_shared.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "g_local.h"

typedef enum AnimID_e
{
	ANIM_WALK1,
	ANIM_PUSH1,
	ANIM_PUSH2,
	ANIM_PUSH3,
	ANIM_STAND1,
	ANIM_WORK1,
	ANIM_WORK2,
	ANIM_WORK3,
	ANIM_WORK4,
	ANIM_WORK5,
	ANIM_PAIN1,
	ANIM_PAIN2,
	ANIM_PAIN3,
	ANIM_REST1_TRANS,
	ANIM_REST1_WIPE,
	ANIM_REST1,
	ANIM_REST2_WIPE,
	ANIM_REST3_WIPE,
	ANIM_REST4_TRANS,
	ANIM_REST4_TRANS2,
	ANIM_REST4,
	ANIM_CELEBRATE1,
	ANIM_CELEBRATE2,
	ANIM_CELEBRATE3_TRANS,
	ANIM_CELEBRATE3,
	ANIM_CELEBRATE4_TRANS,
	ANIM_CELEBRATE4,
	ANIM_CELEBRATE5_TRANS,
	ANIM_CELEBRATE5,
	ANIM_CHARGE1,
	ANIM_CHARGE2,
	ANIM_CHARGE3,
	ANIM_CHARGE4,
	ANIM_CHARGE5,
	ANIM_ATTACK1,
	ANIM_ATTACK2,
	ANIM_ATTACK3,
	ANIM_DEATH1,
	ANIM_DEATH2,

	ANIM_C_ACTION1,
	ANIM_C_ACTION2,
	ANIM_C_ACTION3,
	ANIM_C_ACTION4,
	ANIM_C_ACTION5,
	ANIM_C_ACTION6,
	ANIM_C_ACTION7,
	ANIM_C_ACTION8,
	ANIM_C_ACTION9,
	ANIM_C_ACTION10,
	ANIM_C_ACTION11,
	ANIM_C_ACTION12,
	ANIM_C_ACTION13,
	ANIM_C_ACTION14,
	ANIM_C_ACTION15,
	ANIM_C_ATTACK1,
	ANIM_C_ATTACK2,
	ANIM_C_ATTACK3,
	ANIM_C_DEATH1,
	ANIM_C_DEATH2,
	ANIM_C_GIB1,
	ANIM_C_IDLE1,
	ANIM_C_IDLE2,
	ANIM_C_IDLE3,
	ANIM_C_IDLE4,
	ANIM_C_IDLE5,
	ANIM_C_IDLE6,
	ANIM_C_PAIN1,
	ANIM_C_PAIN2,
	ANIM_C_PAIN3,
	ANIM_C_THINKAGAIN,
	ANIM_C_TRANS1,
	ANIM_C_TRANS2,
	ANIM_C_TRANS3,
	ANIM_C_TRANS4,
	ANIM_C_TRANS5,
	ANIM_C_TRANS6,
	ANIM_C_WALK1,
	ANIM_C_WALK2,
	ANIM_C_WALK3,
	ANIM_C_WALK4,

	NUM_ANIMS
} AnimID_t;

typedef enum SoundID_e
{
	SND_PICK1,
	SND_PICK2,
	SND_SPIKE1,
	SND_SPIKE2,
	SND_HAMMER1,
	SND_HAMMER2,
	SND_PICK_FLESH,
	SND_HAMMER_FLESH,
	SND_WIPE_BROW,
	SND_ENRAGE1,
	SND_ENRAGE2,
	SND_DEATH,
	SND_CHEER1,
	SND_CHEER2,
	SND_CHEER3,

	// Singing.
	SND_CHORUS1,
	SND_CHORUS2,
	SND_CHORUS3,
	SND_SOLO1,
	SND_SOLO2,

	SND_PAIN1,
	SND_PAIN2,

	NUM_SOUNDS
} SoundID_t;

extern const animmove_t ogle_move_walk1;
extern const animmove_t ogle_move_push1;
extern const animmove_t ogle_move_push2;
extern const animmove_t ogle_move_push3;
extern const animmove_t ogle_move_work1;
extern const animmove_t ogle_move_work2;
extern const animmove_t ogle_move_work3;
extern const animmove_t ogle_move_work4;
extern const animmove_t ogle_move_work5;
extern const animmove_t ogle_move_pain1;
extern const animmove_t ogle_move_pain2;
extern const animmove_t ogle_move_rest1;
extern const animmove_t ogle_move_stand1;
extern const animmove_t ogle_move_rest1_wipe;
extern const animmove_t ogle_move_rest2_wipe;
extern const animmove_t ogle_move_rest3_wipe;
extern const animmove_t ogle_move_rest1_trans;
extern const animmove_t ogle_move_rest4_trans;
extern const animmove_t ogle_move_rest4_trans2;
extern const animmove_t ogle_move_rest4;
extern const animmove_t ogle_move_pain3;
extern const animmove_t ogle_move_celebrate1;
extern const animmove_t ogle_move_celebrate2;
extern const animmove_t ogle_move_celebrate3_trans;
extern const animmove_t ogle_move_celebrate3;
extern const animmove_t ogle_move_celebrate4_trans;
extern const animmove_t ogle_move_celebrate4;
extern const animmove_t ogle_move_celebrate5_trans;
extern const animmove_t ogle_move_celebrate5;
extern const animmove_t ogle_move_charge1;
extern const animmove_t ogle_move_charge2;
extern const animmove_t ogle_move_charge3;
extern const animmove_t ogle_move_charge4;
extern const animmove_t ogle_move_charge5;
extern const animmove_t ogle_move_attack1;
extern const animmove_t ogle_move_attack2;
extern const animmove_t ogle_move_attack3;
extern const animmove_t ogle_move_death1;
extern const animmove_t ogle_move_death2;

extern const animmove_t ogle_c_move_action1;
extern const animmove_t ogle_c_move_action2;
extern const animmove_t ogle_c_move_action3;
extern const animmove_t ogle_c_move_action4;
extern const animmove_t ogle_c_move_action5;
extern const animmove_t ogle_c_move_action6;
extern const animmove_t ogle_c_move_action7;
extern const animmove_t ogle_c_move_action8;
extern const animmove_t ogle_c_move_action9;
extern const animmove_t ogle_c_move_action10;
extern const animmove_t ogle_c_move_action11;
extern const animmove_t ogle_c_move_action12;
extern const animmove_t ogle_c_move_action13;
extern const animmove_t ogle_c_move_action14;
extern const animmove_t ogle_c_move_action15;
extern const animmove_t ogle_c_move_attack1;
extern const animmove_t ogle_c_move_attack2;
extern const animmove_t ogle_c_move_attack3;
extern const animmove_t ogle_c_move_death1;
extern const animmove_t ogle_c_move_death2;
extern const animmove_t ogle_c_move_idle1;
extern const animmove_t ogle_c_move_idle2;
extern const animmove_t ogle_c_move_idle3;
extern const animmove_t ogle_c_move_idle4;
extern const animmove_t ogle_c_move_idle5;
extern const animmove_t ogle_c_move_idle6;
extern const animmove_t ogle_c_move_pain1;
extern const animmove_t ogle_c_move_pain2;
extern const animmove_t ogle_c_move_pain3;
extern const animmove_t ogle_c_move_trans1;
extern const animmove_t ogle_c_move_trans2;
extern const animmove_t ogle_c_move_trans3;
extern const animmove_t ogle_c_move_trans4;
extern const animmove_t ogle_c_move_trans5;
extern const animmove_t ogle_c_move_trans6;
extern const animmove_t ogle_c_move_walk1;
extern const animmove_t ogle_c_move_walk2;
extern const animmove_t ogle_c_move_walk3;
extern const animmove_t ogle_c_move_walk4;

extern void ogle_pick_dust(edict_t* self);
extern void ogle_celebrate(edict_t* self);
extern void ogle_strike(edict_t* self);
extern void ogle_pause(edict_t* self);
extern void ogle_rest(edict_t* self);
extern void ogle_push(edict_t* self, float distance);

#define BPN_GRANDDADDY	0 //TODO: unused.
#define BPN_DADDY		1 //TODO: unused.
#define BPN_TORSO		2 //TODO: unused.
#define BPN_NAIL		4
#define BPN_PICK		8
#define BPN_RUPARM		16
#define BPN_RLEG		32 //TODO: unused.
#define BPN_R4ARM		64
#define BPN_LLEG		128 //TODO: unused.
#define BPN_LUPARM		256
#define BPN_L4ARM		512
#define BPN_HAMMER		1024
#define BPN_HANDLE		2048