//
// m_priestess_shared.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "g_local.h"

typedef enum AnimID_e
{
	ANIM_STAND1,
	ANIM_ATTACK1_GO,
	ANIM_ATTACK1_LOOP,
	ANIM_ATTACK1_END,
	ANIM_ATTACK2,
	ANIM_BACKUP,
	ANIM_DEATH,
	ANIM_IDLE,
	ANIM_JUMP,
	ANIM_PAIN,
	ANIM_IDLE_POSE,
	ANIM_POSE_TRANS,
	ANIM_SHIELD_GO,
	ANIM_SHIELD_END,
	ANIM_DODGE_LEFT,
	ANIM_DODGE_RIGHT,
	ANIM_WALK,
	ANIM_JUMP_FORWARD,
	ANIM_JUMP_BACK,
	ANIM_JUMP_RIGHT,
	ANIM_JUMP_LEFT,
	ANIM_JUMP_POUNCE,
	ANIM_POUNCE_ATTACK,
	ANIM_ATTACK3_GO,
	ANIM_ATTACK3_LOOP,
	ANIM_ATTACK3_END,
	ANIM_JUMP_ATTACK,

	NUM_ANIMS
} AnimID_t;

typedef enum SoundID_e
{
	SND_PAIN1,
	SND_PAIN2,

	// You win.
	SND_FALL,

	// Attack 1 - 3 balls spell.
	SND_3BALLATK,
	SND_BALLHIT,

	// Attack 2 - whirling staff.
	SND_WHIRL,
	SND_SWIPE,
	SND_SWIPEHIT,
	SND_SWIPEMISS,
	SND_SWIPEWALL,

	// Bug minefield.
	SND_BUGS,		// Fire a bug out.
	SND_BUGBUZZ,	// Bug buzzes in place.
	SND_BUGHIT,		// Bug explodes.

	// Ray of light.
	SND_ZAP, // Earthquake, lightning, rays of light.
	SND_ZAPHIT, // Earthquake, lightning, rays of light.

	// Homing missiles.
	SND_HOMINGATK,
	SND_HOMINGHIT,

	// Teleport in & out.
	SND_TPORT_IN,
	SND_TPORT_OUT,

	NUM_SOUNDS
} SoundID_t;

extern const animmove_t priestess_move_stand1;
extern const animmove_t priestess_move_attack1_go;
extern const animmove_t priestess_move_attack1_loop;
extern const animmove_t priestess_move_attack1_end;
extern const animmove_t priestess_move_attack2;
extern const animmove_t priestess_move_backup;
extern const animmove_t priestess_move_death;
extern const animmove_t priestess_move_idle;
extern const animmove_t priestess_move_jump;
extern const animmove_t priestess_move_pain;
extern const animmove_t priestess_move_idle_pose;
extern const animmove_t priestess_move_pose_trans;
extern const animmove_t priestess_move_shield_go;
extern const animmove_t priestess_move_shield_end;
extern const animmove_t priestess_move_dodge_left;
extern const animmove_t priestess_move_dodge_right;
extern const animmove_t priestess_move_walk;
extern const animmove_t priestess_move_jump_forward;
extern const animmove_t priestess_move_jump_back;
extern const animmove_t priestess_move_jump_right;
extern const animmove_t priestess_move_jump_left;
extern const animmove_t priestess_move_jump_pounce;
extern const animmove_t priestess_move_pounce_attack;
extern const animmove_t priestess_move_attack3_go;
extern const animmove_t priestess_move_attack3_loop;
extern const animmove_t priestess_move_attack3_end;
extern const animmove_t priestess_move_jump_attack;

extern void priestess_fire1(edict_t* self, float pitch_ofs, float yaw_ofs, float roll_ofs);

extern void priestess_jump_forward(edict_t* self);
extern void priestess_jump_back(edict_t* self);
extern void priestess_jump_right(edict_t* self);
extern void priestess_jump_left(edict_t* self);

extern void priestess_strike(edict_t* self, float damage);

extern void priestess_pounce_attack(edict_t* self);
extern void priestess_idle(edict_t* self);
extern void priestess_pounce(edict_t* self);
extern void priestess_move(edict_t* self, float vf, float vr, float vu);
extern void priestess_pause(edict_t* self);

extern void priestess_attack3_loop(edict_t* self);
extern void priestess_attack3_loop_fire(edict_t* self);

extern void priestess_jump_attack(edict_t* self);

extern void priestess_teleport_go(edict_t* self);
extern void priestess_teleport_end(edict_t* self);
extern void priestess_teleport_move(edict_t* self);
extern void priestess_teleport_return(edict_t* self);
extern void priestess_teleport_self_effects(edict_t* self);

extern void priestess_attack1_pause(edict_t* self);
extern void priestess_dead(edict_t* self);
extern void priestess_stop_alpha(edict_t* self);

extern void priestess_delta_alpha(edict_t* self, float amount);