//
// g_combat.h
//
// Copyright 2025 mxd
//

#pragma once

#include "g_local.h"

extern gitem_armor_t silver_armor_info;
extern gitem_armor_t gold_armor_info;

extern void Killed(edict_t* target, edict_t* inflictor, edict_t* attacker, int damage, vec3_t point, int mod);
extern qboolean CanDamage(const edict_t* target, const edict_t* inflictor);
extern void T_Damage(edict_t* target, edict_t* inflictor, edict_t* attacker, vec3_t p_dir, vec3_t p_point, vec3_t p_normal, int damage, int knockback, int dflags, int mod);
extern void T_DamageRadius(edict_t* inflictor, edict_t* attacker, const edict_t* ignore, float radius, float maxdamage, float mindamage, int dflags, int mod);
extern void T_DamageRadiusFromLoc(vec3_t origin, edict_t* inflictor, edict_t* attacker, const edict_t* ignore, float radius, float maxdamage, float mindamage, int dflags, int mod);