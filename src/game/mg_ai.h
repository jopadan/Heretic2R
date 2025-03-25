//
// mg_ai.h
//
// Copyright 2025 mxd
//

#pragma once

#include "q_shared.h"

extern trace_t MG_MoveStep(edict_t* self, vec3_t move, qboolean relink);
extern qboolean MG_MoveToGoal(edict_t* self, float dist);
extern qboolean MG_SwimFlyToGoal(edict_t* self, float dist);
extern float MG_ChangeWhichYaw(edict_t* self, qboolean ideal_yaw);
extern float MG_ChangeYaw(edict_t* self);
extern float MG_FaceGoal(edict_t* self, qboolean do_turn);
extern trace_t MG_WalkMove(edict_t* self, float yaw, float dist);
extern qboolean MG_BoolWalkMove(edict_t* self, float yaw, float dist);
extern void MG_CheckEvade(edict_t* self);
extern void mg_ai_charge(edict_t* self, float dist);
extern void MG_PostDeathThink(edict_t* self);
extern void MG_CheckLanded(edict_t* self, float next_anim);
extern void MG_InAirMove(edict_t* self, float forward_speed, float up_speed, float right_speed);
extern void MG_ApplyJump(edict_t* self);
extern void MG_NoBlocking(edict_t* self);
extern qboolean MG_GetTargOrg(edict_t* self, vec3_t target_origin);
extern qboolean EqualAngle(float angle1, float angle2, float leniency);

extern qboolean MG_IsAheadOf(const edict_t* self, const edict_t* other);
extern qboolean MG_IsVisiblePos(const edict_t* self, const vec3_t pos);
extern qboolean MG_IsInforntPos(const edict_t* self, const vec3_t pos);
extern void MG_AI_Run(edict_t* self, float dist);