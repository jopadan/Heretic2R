//
// m_imp_shared.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "g_local.h" //mxd

typedef enum AnimID_e
{
	ANIM_DIE,
	ANIM_FLY1,
	ANIM_FLYBACK1,
	ANIM_HOVER1,
	ANIM_FIREBALL,
	ANIM_DIVE_GO,
	ANIM_DIVE_LOOP,
	ANIM_DIVE_END,
	ANIM_DIVE_OUT,
	ANIM_PAIN1,
	ANIM_TUMBLE,
	ANIM_PERCH,
	ANIM_TAKEOFF,
	ANIM_DUP,
	ANIM_DDOWN,

	NUM_ANIMS
} AnimID_t;

typedef enum SoundID_e
{
	SND_GIB,
	SND_FLAP,
	SND_SCREAM,
	SND_DIVE,
	SND_DEATH,
	SND_HIT,
	SND_ATTACK,
	SND_FIZZLE,
	SND_FBHIT,

	NUM_SOUNDS
} SoundID_t;

extern const animmove_t imp_move_die1;
extern const animmove_t imp_move_fly1;
extern const animmove_t imp_move_flyback;
extern const animmove_t imp_move_hover1;
extern const animmove_t imp_move_fireball;
extern const animmove_t imp_move_dive_go;
extern const animmove_t imp_move_dive_loop;
extern const animmove_t imp_move_dive_end;
extern const animmove_t imp_move_dive_out;
extern const animmove_t imp_move_pain1;
extern const animmove_t imp_move_tumble;
extern const animmove_t imp_move_takeoff;
extern const animmove_t imp_move_perch;
extern const animmove_t imp_move_dup;
extern const animmove_t imp_move_ddown;

extern void imp_ai_fly(edict_t* self, float forward_offset, float right_offset, float up_offset);
extern void imp_ai_perch(edict_t* self);
extern void imp_ai_hover(edict_t* self, float distance);

extern void imp_flap_noise(edict_t* self);
extern void imp_dive_noise(edict_t* self);

extern void imp_dive_loop(edict_t* self);
extern void imp_check_dodge(edict_t* self);
extern void imp_dead(edict_t* self);
extern void imp_hit(edict_t* self, float stop_swoop);
extern void imp_pause(edict_t* self);
extern void imp_flyback(edict_t* self);
extern void imp_fix_angles(edict_t* self);

extern void move_imp_hover(edict_t* self);
extern void move_imp_fly(edict_t* self);
extern void imp_dive_move(edict_t* self);
extern void imp_tumble_move(edict_t* self);

extern void imp_fireball(edict_t* self);