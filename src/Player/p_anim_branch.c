//
// p_anim_branch.c
//
// Copyright 1998 Raven Software
//

#include "Player.h"
#include "p_types.h"
#include "p_actions.h"
#include "p_anims.h"
#include "g_items.h"
#include "p_main.h"
#include "p_weapon.h"
#include "FX.h" //TODO: remove
#include "Vector.h"
#include "m_player.h"
#include "p_utility.h" //mxd

static const vec3_t footmins = { -1.0f, -1.0f, 0.0f };
static const vec3_t footmaxs = { 1.0f,  1.0f, 1.0f };

qboolean CheckFall(const playerinfo_t* info)
{
#define FALL_MINHEIGHT	34

	vec3_t endpos;
	VectorCopy(info->origin, endpos);
	endpos[2] -= FALL_MINHEIGHT;

	trace_t trace;
	P_Trace(info, info->origin, info->mins, info->maxs, endpos, &trace); //mxd

	return (trace.fraction == 1.0f); //TODO: '>= 1' in original version. Can trace.fraction be > 1?
}

qboolean CheckUncrouch(const playerinfo_t* info)
{
	vec3_t v;
	VectorCopy(info->origin, v);
	v[2] += 25.0f - info->maxs[2];

	trace_t trace;
	P_Trace(info, info->origin, info->mins, info->maxs, v, &trace); //mxd

	return (trace.fraction == 1.0f && !trace.startsolid && !trace.allsolid);
}

static qboolean CheckCreep(const playerinfo_t* info, const int dir)
{
	vec3_t vf;

	// Scan out and down from the player.
	vec3_t startpos;
	VectorCopy(info->origin, startpos);

	// Ignore the pitch of the player, we only want the yaw.
	const vec3_t ang = { 0.0f, info->angles[YAW], 0.0f };
	AngleVectors(ang, vf, NULL, NULL);

	// Trace ahead about one step (dir is 1 for forward, -1 for back).
	VectorMA(info->origin, (float)(dir * CREEP_STEPDIST), vf, startpos);

	// Account for stepheight.
	vec3_t mins;
	VectorCopy(info->mins, mins);
	mins[2] += CREEP_MAXFALL;

	// Trace forward to see if the path is clear.
	trace_t trace;
	P_Trace(info, info->origin, mins, info->maxs, startpos, &trace); //mxd

	// If it is...
	if (trace.fraction == 1.0f)
	{
		// Move the endpoint down the maximum amount.
		vec3_t endpos;
		VectorCopy(startpos, endpos);
		endpos[2] += info->mins[2] - CREEP_MAXFALL;

		// Trace down.
		P_Trace(info, startpos, mins, info->maxs, endpos, &trace);

		return (trace.fraction < 1.0f && !trace.startsolid && !trace.allsolid);
	}

	// Creep would take us off a ledge.
	return false;
}

static int CheckSlopedStand(const playerinfo_t* info) //TODO: it would be nice to use inverse kinematics for this...
{
	trace_t leftfoot;
	trace_t rightfoot;
	vec3_t lspotmax;
	vec3_t lspotmin;
	vec3_t rspotmax;
	vec3_t rspotmin;
	vec3_t forward;
	vec3_t right;

	const vec3_t player_facing = { 0.0f, info->angles[YAW], 0.0f };
	AngleVectors(player_facing, forward, right, NULL);

	// Get player origin.
	VectorCopy(info->origin, lspotmax);
	VectorCopy(info->origin, rspotmax);

	// Magic number calc for foot placement.
	VectorMA(lspotmax, -9.8f, right, lspotmax);
	VectorMA(lspotmax, 7.2f, forward, lspotmax);

	VectorMA(rspotmax, 10.5f, right, rspotmax);
	VectorMA(rspotmax, -2.6f, forward, rspotmax);

	VectorCopy(lspotmax, lspotmin);
	VectorCopy(rspotmax, rspotmin);

	// Go half player height below player.
	lspotmin[2] += info->mins[2] * 2.0f;
	rspotmin[2] += info->mins[2] * 2.0f;

	P_Trace(info, rspotmax, footmins, footmaxs, rspotmin, &rightfoot); //mxd
	if (rightfoot.fraction == 1.0f && !rightfoot.startsolid && !rightfoot.allsolid)
		return ASEQ_LSTAIR16;

	P_Trace(info, lspotmax, footmins, footmaxs, lspotmin, &leftfoot); //mxd
	if (leftfoot.fraction == 1.0f && !leftfoot.startsolid && !leftfoot.allsolid)
		return ASEQ_RSTAIR16;

	const float footdiff = rightfoot.endpos[2] - leftfoot.endpos[2];

	if (footdiff >= 13.0f)		// Right foot 13 or more higher.
		return ASEQ_RSTAIR16;

	if (footdiff >= 9.0f)		// Right foot 9 or more higher.
		return ASEQ_RSTAIR12;

	if (footdiff >= 5.0f)		// Right foot 8 or more higher.
		return ASEQ_RSTAIR8;

	if (footdiff >= 2.0f)		// Right foot 2 or more higher.
		return ASEQ_RSTAIR4;

	if (footdiff >= -2.0f)		// Flat.
		return ASEQ_NONE;

	if (footdiff >= -5.0f)		// Left foot 4 or less higher.
		return ASEQ_LSTAIR4;

	if (footdiff >= -9.0f)		// Left foot 8 or less higher.
		return ASEQ_LSTAIR8;

	if (footdiff >= -13.0f)		// Left foot 12 or less higher.
		return ASEQ_LSTAIR12;

	return ASEQ_LSTAIR16;
}

PLAYER_API qboolean BranchCheckDismemberAction(const playerinfo_t* info, const int weapon)
{
	const qboolean have_left_arm =  !(info->flags & PLAYER_FLAG_NO_LARM); //mxd
	const qboolean have_right_arm = !(info->flags & PLAYER_FLAG_NO_RARM); //mxd

	// No arm, no shot.
	switch (weapon)
	{
		case ITEM_WEAPON_FLYINGFIST:
		case ITEM_WEAPON_HELLSTAFF:
		case ITEM_WEAPON_SWORDSTAFF:
			return have_right_arm;

		case ITEM_WEAPON_MAGICMISSILE:
		case ITEM_WEAPON_MACEBALLS:
			return (info->powerup_timer > info->leveltime ? have_right_arm : have_left_arm); // Powered up is right arm, non-powered is left.

		default:
			return (have_left_arm && have_right_arm); // Any other weapon will need both hands.
	}
}

#pragma region ========================== CHICKEN ANIMATION LOGIC ==========================

// Decide if we have just fallen off something, or we are falling down.
int ChickenBranchLwrStanding(const playerinfo_t* info)
{
	assert(info);

	// Since the chicken always runs through this, make sure we don't check for falling in the water!
	if (info->groundentity == NULL && info->waterlevel < 2 && CheckFall(info))
		return ASEQ_FALL;

	// Decide what we should be doing now.
	if (info->waterlevel >= 2)
		return (info->seqcmd[ACMDL_FWD] ? ASEQ_USWIMF_GO : ASEQ_NONE); //TODO: ASEQ_USWIMF_GO is unused, chicken can't move underwater (but can jump)... 

	if (info->seqcmd[ACMDU_ATTACK])
		return (info->seqcmd[ACMDL_FWD] ? ASEQ_RUNF : ASEQ_WSWORD_SPIN);

	if (info->seqcmd[ACMDL_JUMP])
	{
		if (info->seqcmd[ACMDL_WALK_F])
			return ASEQ_JUMPFWD_WGO;

		if (info->seqcmd[ACMDL_RUN_F])
			return ASEQ_JUMPFWD_RGO;

		if (info->seqcmd[ACMDL_BACK] || info->seqcmd[ACMDL_RUN_B])
			return ASEQ_JUMPBACK_SGO;

		return ASEQ_JUMPUP;
	}

	if (info->seqcmd[ACMDL_WALK_F])
		return ASEQ_WALKF_GO;

	if (info->seqcmd[ACMDL_RUN_F])
		return ASEQ_RUNF_GO;

	if (info->seqcmd[ACMDL_RUN_B])
		return ASEQ_WSTRAFE_LEFT;

	if (info->seqcmd[ACMDL_BACK])
		return ASEQ_WALKB;

	if (info->seqcmd[ACMDL_STRAFE_L])
		return ASEQ_STRAFEL;

	if (info->seqcmd[ACMDL_STRAFE_R])
		return ASEQ_STRAFER;

	// If we've reached this point, we are still idling - so decide if which one we want to do.
	switch (info->irand(info, 0, 5))
	{
		case 0: return ASEQ_IDLE_LOOKR;
		case 1: return ASEQ_IDLE_LOOKL;
		default: return ASEQ_NONE;
	}
}

// This allows the chicken to interrupt itself - if its idling.
int ChickenBranchidle(playerinfo_t* info)
{
	// Do we need to attack?
	if (info->seqcmd[ACMDU_ATTACK])
	{
		PlayerAnimSetLowerSeq(info, ASEQ_WSWORD_SPIN);
		return ASEQ_WSWORD_SPIN;
	}

	const qboolean do_jump = info->seqcmd[ACMDL_JUMP]; //mxd

	if (info->seqcmd[ACMDL_WALK_F])
	{
		//mxd. Do forward-jump or walk forward.
		const int seq = (do_jump ? ASEQ_JUMPFWD_WGO : ASEQ_WALKF_GO);
		PlayerAnimSetLowerSeq(info, seq);

		return seq;
	}

	if (info->seqcmd[ACMDL_RUN_F])
	{
		//mxd. Do forward-jump or run forward.
		const int seq = (do_jump ? ASEQ_JUMPFWD_RGO : ASEQ_RUNF_GO);
		PlayerAnimSetLowerSeq(info, seq);

		return seq;
	}

	if (info->seqcmd[ACMDL_RUN_B])
	{
		//mxd. Do backward-jump or run backward.
		const int seq = (do_jump ? ASEQ_JUMPFLIPL : ASEQ_WSTRAFE_LEFT);
		PlayerAnimSetLowerSeq(info, seq);

		return seq;
	}

	if (info->seqcmd[ACMDL_BACK])
	{
		//mxd. Do backward-jump or walk backward.
		const int seq = (do_jump ? ASEQ_JUMPBACK : ASEQ_WALKB);
		PlayerAnimSetLowerSeq(info, seq);

		return seq;
	}

	if (info->seqcmd[ACMDL_STRAFE_L])
	{
		// Strafing left.
		PlayerAnimSetLowerSeq(info, ASEQ_STRAFEL);
		return ASEQ_STRAFEL;
	}

	if (info->seqcmd[ACMDL_STRAFE_R])
	{
		// Strafing right.
		PlayerAnimSetLowerSeq(info, ASEQ_STRAFER);
		return ASEQ_STRAFER;
	}

	if (info->seqcmd[ACMDL_JUMP])
	{
		// Jumping in place.
		PlayerAnimSetLowerSeq(info, ASEQ_JUMPUP);
		return ASEQ_JUMPUP;
	}

	return ASEQ_NONE;
}

#pragma endregion

int BranchLwrStanding(playerinfo_t* info)
{
	if (info->deadflag != DEAD_NO)
		return ASEQ_NONE;

	assert(info);

	// Special move.
	if (info->advancedstaff && info->seqcmd[ACMDL_ACTION] && info->seqcmd[ACMDU_ATTACK] &&
		info->pers.weaponready == WEAPON_READY_SWORDSTAFF && BranchCheckDismemberAction(info, ITEM_WEAPON_SWORDSTAFF))
	{
		return ASEQ_WSWORD_LOWERDOWNSTAB;
	}

	// Check to update the idles.
	if (info->lowerseq != ASEQ_STAND && info->lowerseq != ASEQ_IDLE_READY)
		info->idletime = info->leveltime;

	// Check for a fall.
	if (info->groundentity == NULL && info->waterlevel < 2 && CheckFall(info))
		return ASEQ_FALL;

	// Check for a jump (but not while in lava or slime).
	if (info->seqcmd[ACMDL_JUMP] && !(info->watertype & (CONTENTS_SLIME | CONTENTS_LAVA)))
	{
		if (info->seqcmd[ACMDL_FWD])
			return ASEQ_JUMPFWD_SGO;

		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_JUMPBACK_SGO;

		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_JUMPLEFT_SGO;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_JUMPRIGHT_SGO;

		return ASEQ_JUMPSTD_GO;
	}

	// Check for a crouch.
	if (info->seqcmd[ACMDL_CROUCH])
	{
		if (info->seqcmd[ACMDL_FWD])
		{
			info->maxs[2] = 4.0f;
			return ASEQ_ROLLDIVEF_W;
		}

		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_ROLL_B;

		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_ROLL_L;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_ROLL_R;

		return ASEQ_CROUCH_GO;
	}

	// FORWARD

	// Check for a walk speed start.
	if (info->seqcmd[ACMDL_WALK_F])
		return ASEQ_WALKF_GO;

	// Check for a run start.
	if (info->seqcmd[ACMDL_RUN_F])
		return ASEQ_RUNF_GO;

	// Check for creep start.
	if (info->seqcmd[ACMDL_CREEP_F])
		return (CheckCreep(info, 1) ? ASEQ_CREEPF : ASEQ_STAND);

	// BACKWARD

	// Check for a creepback.
	if (info->seqcmd[ACMDL_CREEP_B])
		return (CheckCreep(info, -1) ? ASEQ_CREEPB : ASEQ_STAND);

	// Check for a walk back.
	if (info->seqcmd[ACMDL_WALK_B])
		return ASEQ_WALKB;

	// Check for a backspring.
	if (info->seqcmd[ACMDL_RUN_B] && (!info->seqcmd[ACMDL_STRAFE_L] && !info->seqcmd[ACMDL_STRAFE_R]))
	{
		if (!info->seqcmd[ACMDU_ATTACK] && info->upperidle && !info->dmflags)
			return ASEQ_JUMPSPRINGBGO;

		return ASEQ_WALKB;
	}

	// STRAFES

	// Check for a strafe left.
	if (info->seqcmd[ACMDL_STRAFE_L])
	{
		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_WSTRAFEB_LEFT;

		if (info->pcmd.buttons & BUTTON_RUN)
			return ASEQ_DASH_LEFT_GO;

		return ASEQ_STRAFEL;
	}

	// Check for a strafe right.
	if (info->seqcmd[ACMDL_STRAFE_R])
	{
		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_WSTRAFEB_RIGHT;

		if (info->pcmd.buttons & BUTTON_RUN)
			return ASEQ_DASH_RIGHT_GO;

		return ASEQ_STRAFER;
	}

	// Check for action being held.
	if (info->seqcmd[ACMDL_ACTION])
	{
		if (info->upperidle && PlayerActionCheckPushLever(info))
		{
			info->target = NULL;
			if (!(info->fmnodeinfo[MESH__BOWACTV].flags & FMNI_NO_DRAW))
				return ASEQ_PUSHLEVERRIGHT;

			return ASEQ_PUSHLEVERLEFT;
		}

		if (info->upperidle && PlayerActionCheckPushButton(info))
		{
			info->target = NULL;
			return ASEQ_PUSHBUTTON_GO;
		}

		if (PlayerActionCheckPuzzleGrab(info)) // Are you near a puzzle piece? Then try to take it
			return ASEQ_TAKEPUZZLEPIECE;

		if (PlayerActionUsePuzzle(info)) // Trying to use a puzzle piece
			return ASEQ_NONE; // Need anim to use puzzle piece

		if (info->targetEnt != NULL && PlayerActionCheckRopeGrab(info, 0)) // Climb a rope?
		{
			P_Sound(info, SND_PRED_ID32, CHAN_VOICE, "player/ropegrab.wav", 0.75f); //mxd
			return ASEQ_CLIMB_ON;
		}

		if (info->upperidle && PlayerActionCheckJumpGrab(info, 0))
			return ASEQ_JUMPSTD_GO;
	}

	// Check for an autovault.
	if (info->seqcmd[ACMDL_FWD] && info->upperidle && (info->flags & PLAYER_FLAG_COLLISION))
	{
		PlayerActionCheckVault(info, 0);

		if (info->lowerseq == ASEQ_VAULT_LOW || info->lowerseq == ASEQ_PULLUP_HALFWALL)
			return info->lowerseq;
	}

	// Check for a quickturn.
	if (info->seqcmd[ACMDL_QUICKTURN])
		return ASEQ_TURN180;

	// Check for a rotation left.
	if (info->seqcmd[ACMDL_ROTATE_L])
	{
		if (info->lowerseq < ASEQ_PIVOTL_GO || info->lowerseq > ASEQ_PIVOTL_END)
			return ASEQ_PIVOTL_GO;

		return ASEQ_NONE;
	}

	// Check for a rotation right.
	if (info->seqcmd[ACMDL_ROTATE_R])
	{
		if (info->lowerseq < ASEQ_PIVOTR_GO || info->lowerseq > ASEQ_PIVOTR_END)
			return ASEQ_PIVOTR_GO;

		return ASEQ_NONE;
	}

	// Check for a sloped stand.
	if (info->isclient && ((info->lowerseq >= ASEQ_LSTAIR4 && info->lowerseq <= ASEQ_RSTAIR16) || info->lowerseq == ASEQ_STAND))
		return info->lowerseq;

	const int checksloped = CheckSlopedStand(info);
	if (checksloped != ASEQ_NONE)
		return checksloped;

	info->loweridle = true;

	// If was standing on stairs, go to stand.
	if (info->lowerseq >= ASEQ_LSTAIR4 && info->lowerseq <= ASEQ_RSTAIR16)
		return ASEQ_STAND;

	return ASEQ_NONE;
}

// I call this when I end a move and perhaps want to start running or otherwise moving immediately without delay, such as after a pullup.
int BranchLwrStandingRun(playerinfo_t* info)
{
	assert(info);

	if (info->groundentity == NULL && info->waterlevel < 2 && CheckFall(info))
		return ASEQ_FALL;

	if (info->seqcmd[ACMDL_FWD] && info->upperidle)
	{
		PlayerActionCheckVault(info, 0);

		if (info->lowerseq == ASEQ_VAULT_LOW || info->lowerseq == ASEQ_PULLUP_HALFWALL)
			return info->lowerseq;
	}

	if (info->seqcmd[ACMDL_JUMP])
		return ASEQ_JUMPSTD_GO;

	if (info->seqcmd[ACMDL_CREEP_B])
		return ASEQ_CREEPB;

	if (info->seqcmd[ACMDL_CROUCH])
	{
		if (info->seqcmd[ACMDL_FWD])
			return ASEQ_ROLLDIVEF_W;

		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_ROLL_B;

		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_ROLL_L;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_ROLL_R;

		return ASEQ_CROUCH_GO;
	}

	if (info->seqcmd[ACMDL_WALK_F])
		return ASEQ_WALKF;

	if (info->seqcmd[ACMDL_RUN_F])
		return ASEQ_RUNF;

	if (info->seqcmd[ACMDL_CREEP_F])
		return ASEQ_CREEPF;

	if (info->seqcmd[ACMDL_BACK])
		return ASEQ_WALKB;

	if (info->seqcmd[ACMDL_STRAFE_L])
		return ASEQ_STRAFEL;

	if (info->seqcmd[ACMDL_STRAFE_R])
		return ASEQ_STRAFER;

	if (info->seqcmd[ACMDL_ROTATE_L])
		return ((info->lowerseq < ASEQ_PIVOTL_GO || info->lowerseq > ASEQ_PIVOTL_END) ? ASEQ_PIVOTL_GO : ASEQ_NONE);

	if (info->seqcmd[ACMDL_ROTATE_R])
		return ((info->lowerseq < ASEQ_PIVOTR_GO || info->lowerseq > ASEQ_PIVOTR_END) ? ASEQ_PIVOTR_GO : ASEQ_NONE);

	const int checksloped = CheckSlopedStand(info);
	if (checksloped != ASEQ_NONE)
		return checksloped;

	info->loweridle = true;

	if (info->lowerseq >= ASEQ_LSTAIR4 && info->lowerseq <= ASEQ_RSTAIR16) //BUGFIX: '<= ASEQ_LSTAIR16' in original version.
		return ASEQ_STAND; // If was standing on stairs, go to stand.

	return ASEQ_NONE;
}

int BranchLwrWalking(playerinfo_t* info)
{
	const qboolean is_running = (info->buttons & BUTTON_RUN); //mxd
	const qboolean is_creeping = (info->buttons & BUTTON_CREEP); //mxd
	const qboolean in_slime_or_lava = (info->watertype & (CONTENTS_SLIME | CONTENTS_LAVA)); //mxd
	const int curseq = info->lowerseq;

	assert(info);

	// Check for the player falling [LOW PROBABILITY, IMMEDIATE CONCERN].
	if (info->groundentity == NULL && info->waterlevel < 2 && !in_slime_or_lava && CheckFall(info))
		return ASEQ_FALLWALK_GO;

	// Check for an autovault (only occurs if upper half of body is idle!) [LOW PROBABILITY, IMMEDIATE CONCERN].
	if (info->seqcmd[ACMDL_FWD] && info->upperidle && (info->flags & PLAYER_FLAG_COLLISION))
	{
		PlayerActionCheckVault(info, 0);

		if (curseq == ASEQ_VAULT_LOW || curseq == ASEQ_PULLUP_HALFWALL)
			return curseq;

		if (info->seqcmd[ACMDL_ACTION] && PlayerActionCheckJumpGrab(info, 0))
			return ASEQ_JUMPSTD_GO;
	}

	// Check for a jump [LOW PROBABILITY].
	if (info->seqcmd[ACMDL_JUMP] && !in_slime_or_lava)
	{
		if (is_running)
		{
			if (info->seqcmd[ACMDL_FWD])
			{
				if (info->pers.weaponready == WEAPON_READY_SWORDSTAFF && !(info->flags & PLAYER_FLAG_NO_RARM))
					return ASEQ_POLEVAULT1_W;

				return ASEQ_JUMPFWD_WGO;
			}

			if (info->seqcmd[ACMDL_BACK])
				return ASEQ_JUMPBACK_RGO;

			if (info->seqcmd[ACMDL_STRAFE_L])
				return ASEQ_JUMPLEFT_RGO;

			if (info->seqcmd[ACMDL_STRAFE_R])
				return ASEQ_JUMPRIGHT_RGO;
		}
		else if (is_creeping)
		{
			if (info->seqcmd[ACMDL_FWD])
				return ASEQ_JUMPFWD_SGO;

			if (info->seqcmd[ACMDL_BACK])
				return ASEQ_JUMPBACK_SGO;

			if (info->seqcmd[ACMDL_STRAFE_L])
				return ASEQ_JUMPLEFT_SGO;

			if (info->seqcmd[ACMDL_STRAFE_R])
				return ASEQ_JUMPRIGHT_SGO;
		}
		else
		{
			if (info->seqcmd[ACMDL_FWD])
				return ASEQ_JUMPFWD_WGO;

			if (info->seqcmd[ACMDL_BACK])
				return ASEQ_JUMPBACK_WGO;

			if (info->seqcmd[ACMDL_STRAFE_L])
				return ASEQ_JUMPLEFT_WGO;

			if (info->seqcmd[ACMDL_STRAFE_R])
				return ASEQ_JUMPRIGHT_WGO;
		}
	}

	// Check for creep strafing forward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_CREEP_F])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_CSTRAFE_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_CSTRAFE_RIGHT;
	}

	// Check for walk strafing forward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_WALK_F])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_WSTRAFE_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_WSTRAFE_RIGHT;
	}

	// Check for run strafing forward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_RUN_F])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_RSTRAFE_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_RSTRAFE_RIGHT;
	}

	// Check for creep strafing backward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_CREEP_B])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_CSTRAFEB_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_CSTRAFEB_RIGHT;
	}

	// Check for walk strafing backward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_WALK_B])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_WSTRAFEB_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_WSTRAFEB_RIGHT;
	}

	// Check for a crouch [LOW PROBABILITY].
	if (info->seqcmd[ACMDL_CROUCH])
	{
		if (info->seqcmd[ACMDL_FWD])
			return (is_creeping ? ASEQ_CROUCH_WALK_F : ASEQ_ROLLDIVEF_W);

		if (info->seqcmd[ACMDL_BACK])
			return (is_creeping ? ASEQ_CROUCH_WALK_B : ASEQ_ROLL_B);

		if (info->seqcmd[ACMDL_STRAFE_L])
			return (is_creeping ? ASEQ_CROUCH_WALK_L : ASEQ_ROLL_L);

		if (info->seqcmd[ACMDL_STRAFE_R])
			return (is_creeping ? ASEQ_CROUCH_WALK_R : ASEQ_ROLL_R);

		return ASEQ_CROUCH_GO;
	}

	// Look for the action key being pressed [LOW PROBABILITY].
	if (info->seqcmd[ACMDL_ACTION])
	{
		if (info->targetEnt != NULL && PlayerActionCheckRopeGrab(info, 0)) // Climb a rope?
		{
			info->flags |= PLAYER_FLAG_ONROPE;
			P_Sound(info, SND_PRED_ID33, CHAN_VOICE, "player/ropegrab.wav", 0.75f); //mxd

			return ASEQ_CLIMB_ON;
		}

		// Try and use a puzzle piece.
		PlayerActionUsePuzzle(info);

		// Check for an auto-jump vault.
		if (info->upperidle && (info->flags & PLAYER_FLAG_COLLISION) && PlayerActionCheckJumpGrab(info, 0))
			return ASEQ_JUMPSTD_GO;
	}

	// Check for a quickturn [LOW PROBABILITY].
	if (info->seqcmd[ACMDL_QUICKTURN])
		return ASEQ_TURN180;

	// All actions have been accounted for, now just see if we are ending certain sequences.

	// Check for a sudden change of speed [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_CREEP_F])
		return ASEQ_CREEPF;

	if (info->seqcmd[ACMDL_CREEP_B])
		return ASEQ_CREEPB;

	if (info->seqcmd[ACMDL_RUN_F])
		return ASEQ_RUNF;

	if (!info->seqcmd[ACMDL_STRAFE_L] && !info->seqcmd[ACMDL_STRAFE_R])
	{
		// If we're pressing forward, and nothing else is happening, then we're just walking forward.
		if (info->seqcmd[ACMDL_WALK_F] && curseq != ASEQ_WALKF)
			return ASEQ_WALKF;

		// If we're pressing backward, and nothing else is happening, then we're just walking backward.
		if (info->seqcmd[ACMDL_WALK_B] && curseq != ASEQ_WALKB)
			return ASEQ_WALKB;
	}

	return ASEQ_NONE;
}

int BranchLwrRunning(playerinfo_t* info) //TODO: replace with BranchLwrWalking()?
{
	return BranchLwrWalking(info);
}

int BranchLwrRunningStrafe(playerinfo_t* info)
{
	assert(info);

	// Check for the player falling [LOW PROBABILITY, BUT IMMEDIATE CONCERN].
	if (info->groundentity == NULL && info->waterlevel < 2 && !(info->watertype & (CONTENTS_SLIME | CONTENTS_LAVA)) && CheckFall(info))
		return ASEQ_FALLWALK_GO;

	// Check for crouching.
	if (info->seqcmd[ACMDL_CROUCH])
	{
		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_ROLL_B;

		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_ROLL_L;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_ROLL_R;

		info->maxs[2] = 4.0f;

		return ASEQ_ROLLDIVEF_R;
	}

	if (info->seqcmd[ACMDL_RUN_F])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_RSTRAFE_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_RSTRAFE_RIGHT;
	}

	// Check for a change in speeds.
	if (info->seqcmd[ACMDL_WALK_F])
		return ASEQ_WALKF;

	if (info->seqcmd[ACMDL_STRAFE_L])
	{
		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_WSTRAFEB_LEFT;

		if (info->buttons & BUTTON_RUN)
		{
			if (info->lowerseq == ASEQ_DASH_LEFT || info->lowerseq == ASEQ_DASH_LEFT_GO)
				return ASEQ_NONE;

			return ASEQ_DASH_LEFT_GO;
		}

		return ASEQ_STRAFEL;
	}

	if (info->seqcmd[ACMDL_STRAFE_R])
	{
		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_WSTRAFEB_RIGHT;

		if (info->buttons & BUTTON_RUN)
		{
			if (info->lowerseq == ASEQ_DASH_RIGHT || info->lowerseq == ASEQ_DASH_RIGHT_GO)
				return ASEQ_NONE;

			return ASEQ_DASH_RIGHT_GO;
		}

		return ASEQ_STRAFER;
	}

	if (info->seqcmd[ACMDL_QUICKTURN])
		return ASEQ_TURN180;

	if (!info->seqcmd[ACMDL_STRAFE_L] && !info->seqcmd[ACMDL_STRAFE_R])
		return (info->seqcmd[ACMDL_FWD] ? ASEQ_RUNF_GO : ASEQ_STAND); // We're just trying to go forward / We've let go of the important buttons.

	// Look for the action key being pressed [LOW PROBABILITY].
	if (info->seqcmd[ACMDL_ACTION])
	{
		if (info->targetEnt != NULL && PlayerActionCheckRopeGrab(info, 0)) // Climb a rope?
		{
			info->flags |= PLAYER_FLAG_ONROPE;
			P_Sound(info, SND_PRED_ID34, CHAN_VOICE, "player/ropegrab.wav", 0.75f); //mxd

			return ASEQ_CLIMB_ON; // On a rope.
		}

		// Check for a jump grab.
		if (info->upperidle && (info->flags & PLAYER_FLAG_COLLISION) && PlayerActionCheckJumpGrab(info, 0))
			return ASEQ_JUMPSTD_GO;
	}

	return ASEQ_NONE;
}

int BranchLwrShortstep(playerinfo_t* info)
{
	const qboolean is_creeping = (info->buttons & BUTTON_CREEP); //mxd
	const qboolean in_slime_or_lava = (info->watertype & (CONTENTS_SLIME | CONTENTS_LAVA)); //mxd

	assert(info);

	if (info->groundentity == NULL && info->waterlevel < 2 && !in_slime_or_lava && CheckFall(info))
		return ASEQ_FALLWALK_GO;

	if (info->seqcmd[ACMDL_ACTION])
	{
		if (info->targetEnt != NULL && PlayerActionCheckRopeGrab(info, 0)) // Climb a rope?
		{
			info->flags |= PLAYER_FLAG_ONROPE;
			P_Sound(info, SND_PRED_ID36, CHAN_VOICE, "player/ropegrab.wav", 0.75f); //mxd

			return ASEQ_CLIMB_ON;
		}

		if (info->upperidle && (info->flags & PLAYER_FLAG_COLLISION) && PlayerActionCheckJumpGrab(info, 0))
			return ASEQ_JUMPSTD_GO;
	}

	if (info->seqcmd[ACMDL_JUMP] && !in_slime_or_lava)
	{
		if (info->seqcmd[ACMDL_FWD])
			return ASEQ_JUMPFWD_SGO;

		if (info->seqcmd[ACMDL_BACK])
			return ASEQ_JUMPBACK_SGO;

		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_JUMPLEFT_SGO;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_JUMPRIGHT_SGO;

		return ASEQ_JUMPSTD_GO;
	}

	// Check for a crouch.
	if (info->seqcmd[ACMDL_CROUCH])
	{
		if (info->seqcmd[ACMDL_FWD])
			return (is_creeping ? ASEQ_CROUCH_WALK_F : ASEQ_ROLLDIVEF_W);

		if (info->seqcmd[ACMDL_BACK])
			return (is_creeping ? ASEQ_CROUCH_WALK_B : ASEQ_ROLL_B);

		if (info->seqcmd[ACMDL_STRAFE_L])
			return (is_creeping ? ASEQ_CROUCH_WALK_L : ASEQ_ROLL_L);

		if (info->seqcmd[ACMDL_STRAFE_R])
			return (is_creeping ? ASEQ_CROUCH_WALK_R : ASEQ_ROLL_R);

		return ASEQ_CROUCH_GO;
	}

	// Check for walk strafing forward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_CREEP_F])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_CSTRAFE_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_CSTRAFE_RIGHT;
	}

	// Check for walk strafing backward [HIGH PROBABILITY].
	if (info->seqcmd[ACMDL_CREEP_B])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_CSTRAFEB_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_CSTRAFEB_RIGHT;
	}

	if (info->seqcmd[ACMDL_WALK_F])
		return ASEQ_WALKF_GO;

	if (info->seqcmd[ACMDL_RUN_F])
		return ASEQ_RUNF_GO;

	if (info->seqcmd[ACMDL_CREEP_B])
		return ASEQ_CREEPB;

	if (info->seqcmd[ACMDL_BACK])
		return ASEQ_WALKB;

	return ASEQ_NONE;
}

int BranchLwrBackspring(playerinfo_t* info)
{
	assert(info);

	if (info->groundentity == NULL && info->waterlevel < 2 && !(info->watertype & (CONTENTS_SLIME | CONTENTS_LAVA)) && CheckFall(info))
		return ASEQ_FALLWALK_GO;

	if (info->seqcmd[ACMDL_JUMP])
		return ASEQ_JUMPFLIPB;

	if (info->seqcmd[ACMDL_RUN_B])
	{
		if (info->seqcmd[ACMDL_STRAFE_L])
			return ASEQ_WSTRAFEB_LEFT;

		if (info->seqcmd[ACMDL_STRAFE_R])
			return ASEQ_WSTRAFEB_RIGHT;

		if (info->seqcmd[ACMDU_ATTACK])
			return ASEQ_WALKB;

		PlayerAnimSetUpperSeq(info, ASEQ_NONE);
		return ASEQ_JUMPSPRINGB;
	}

	if (info->seqcmd[ACMDL_WALK_B])
		return ASEQ_WALKB;

	if (info->seqcmd[ACMDL_CREEP_B])
		return ASEQ_CREEPB;

	if (info->seqcmd[ACMDL_CROUCH])
		return ASEQ_CROUCH_GO;

	return ASEQ_NONE;
}

int BranchLwrJumping(playerinfo_t* info)
{
	assert(info);

	if (info->seqcmd[ACMDL_ACTION])
	{
		if (info->targetEnt != NULL && PlayerActionCheckRopeGrab(info, 0)) // Climb a rope?
		{
			info->flags |= PLAYER_FLAG_ONROPE;
			return ASEQ_CLIMB_ON;
		}
	}

	if (info->seqcmd[ACMDL_FWD])
		return ASEQ_JUMPFWD_SGO;

	if (info->seqcmd[ACMDL_BACK])
		return ASEQ_JUMPBACK_SGO;

	if (info->seqcmd[ACMDL_STRAFE_L])
		return ASEQ_JUMPLEFT_SGO;

	if (info->seqcmd[ACMDL_STRAFE_R])
		return ASEQ_JUMPRIGHT_SGO;

	return ASEQ_NONE;
}

int BranchLwrKnockDown(const playerinfo_t* info)
{
	return (info->seqcmd[ACMDL_BACK] ? ASEQ_KNOCKDOWN_EVADE : ASEQ_KNOCKDOWN_GETUP);
}

int BranchLwrCrouching(playerinfo_t* info)
{
	const qboolean is_creeping = (info->buttons & BUTTON_CREEP); //mxd

	assert(info);

	info->maxs[2] = 4.0f;

	if (info->groundentity == NULL && info->waterlevel < 2 && CheckFall(info))
		return ASEQ_CROUCH_END;

	// See if we're ending the crouch.
	if (!info->seqcmd[ACMDL_CROUCH] && !info->seqcmd[ACMDL_ROTATE_L] && !info->seqcmd[ACMDL_ROTATE_R])
	{
		if (CheckUncrouch(info))
		{
			info->maxs[2] = 25.0f;
			return ASEQ_CROUCH_END;
		}
	}
	else if (info->seqcmd[ACMDL_JUMP]) // Skip jump check when can't un-crouch.
	{
		return ASEQ_JUMPSTD_GO;
	}

	if (info->seqcmd[ACMDL_FWD])
		return (is_creeping ? ASEQ_CROUCH_WALK_F : ASEQ_ROLLDIVEF_W);

	if (info->seqcmd[ACMDL_BACK])
		return (is_creeping ? ASEQ_CROUCH_WALK_B : ASEQ_ROLL_B);

	if (info->seqcmd[ACMDL_STRAFE_L])
		return (is_creeping ? ASEQ_CROUCH_WALK_L : ASEQ_ROLL_L);

	if (info->seqcmd[ACMDL_STRAFE_R])
		return (is_creeping ? ASEQ_CROUCH_WALK_R : ASEQ_ROLL_R);

	if (info->seqcmd[ACMDL_ROTATE_L])
		return ASEQ_CROUCH_PIVOTL;

	if (info->seqcmd[ACMDL_ROTATE_R])
		return ASEQ_CROUCH_PIVOTR;

	return ASEQ_CROUCH;
}

int BranchLwrSurfaceSwim(playerinfo_t* info)
{
	assert(info);

	if (info->pers.weaponready != WEAPON_READY_HANDS)
	{
		gitem_t* weapon = FindItem("fball");
		if (weapon != NULL)
			Weapon_EquipSpell(info, weapon);
	}

	if (info->seqcmd[ACMDL_ACTION])
	{
		// Try and use a puzzle piece.
		PlayerActionUsePuzzle(info);
		PlayerActionCheckVault(info, 0);

		if (info->lowerseq == ASEQ_VAULT_LOW || info->lowerseq == ASEQ_PULLUP_HALFWALL)
			return info->lowerseq;

		return ASEQ_NONE;
	}

	if (info->seqcmd[ACMDL_FWD])
	{
		PlayerActionCheckVault(info, 0);

		if (info->lowerseq == ASEQ_VAULT_LOW || info->lowerseq == ASEQ_PULLUP_HALFWALL)
			return  info->lowerseq;

		if (info->waterlevel > 2)
			return ASEQ_USWIMF_GO;

		if (info->seqcmd[ACMDL_RUN_F])
		{
			if (info->lowerseq == ASEQ_SSWIM_FAST_GO || info->lowerseq == ASEQ_SSWIM_FAST)
				return ASEQ_SSWIM_FAST;

			return ASEQ_SSWIM_FAST_GO;
		}

		return ((info->lowerseq == ASEQ_SSWIMF_GO || info->lowerseq == ASEQ_SSWIMF) ? ASEQ_SSWIMF : ASEQ_SSWIMF_GO);
	}

	if (info->seqcmd[ACMDL_BACK])
		return ((info->lowerseq == ASEQ_SSWIMB_GO || info->lowerseq == ASEQ_SSWIMB) ? ASEQ_SSWIMB : ASEQ_SSWIMB_GO);

	if (info->seqcmd[ACMDL_STRAFE_L])
		return ((info->lowerseq == ASEQ_SSWIML_GO || info->lowerseq == ASEQ_SSWIML) ? ASEQ_SSWIML : ASEQ_SSWIML_GO);

	if (info->seqcmd[ACMDL_STRAFE_R])
		return ((info->lowerseq == ASEQ_SSWIMR_GO || info->lowerseq == ASEQ_SSWIMR) ? ASEQ_SSWIMR : ASEQ_SSWIMR_GO);

	if (info->waterlevel > 2 && info->lowerseq == ASEQ_SSWIM_IDLE)
		return ASEQ_USWIM_IDLE;

	return ASEQ_NONE;
}

int BranchLwrUnderwaterSwim(playerinfo_t* info)
{
	assert(info);

	if (info->pers.weaponready != WEAPON_READY_HANDS)
	{
		gitem_t* weapon = FindItem("fball");
		if (weapon != NULL)
			Weapon_EquipSpell(info, weapon);
	}

	if (info->seqcmd[ACMDL_ACTION])
	{
		PlayerActionUsePuzzle(info); // Try and use a puzzle piece.
		return ASEQ_NONE;
	}

	if (info->seqcmd[ACMDL_FWD])
	{
		if (info->waterlevel <= 2)
			return ASEQ_SSWIM_RESURFACE;

		return ((info->lowerseq == ASEQ_USWIMF_GO || info->lowerseq == ASEQ_USWIMF) ? ASEQ_USWIMF : ASEQ_USWIMF_GO);
	}

	if (info->seqcmd[ACMDL_BACK])
		return ((info->lowerseq == ASEQ_USWIMB_GO || info->lowerseq == ASEQ_USWIMB) ? ASEQ_USWIMB : ASEQ_USWIMB_GO);

	if (info->seqcmd[ACMDL_STRAFE_L])
		return ((info->lowerseq == ASEQ_USWIML_GO || info->lowerseq == ASEQ_USWIML) ? ASEQ_USWIML : ASEQ_USWIML_GO);

	if (info->seqcmd[ACMDL_STRAFE_R])
		return ((info->lowerseq == ASEQ_USWIMR_GO || info->lowerseq == ASEQ_USWIMR) ? ASEQ_USWIMR : ASEQ_USWIMR_GO);

	if (info->waterlevel <= 2 && info->lowerseq == ASEQ_USWIM_IDLE)
		return ASEQ_SSWIM_IDLE;

	return ASEQ_NONE;
}

int BranchLwrClimbing(playerinfo_t* info)
{
	return (info->isclient ? ASEQ_NONE : info->G_BranchLwrClimbing(info));
}

static int BranchUprReadyHands(playerinfo_t* info)
{
	assert(info);

	// See if we have the arm to do that magic.
	if (!BranchCheckDismemberAction(info, info->pers.weapon->tag))
		return ASEQ_NONE;

	if (info->seqcmd[ACMDU_ATTACK] && !(info->edictflags & FL_CHICKEN))	// Not a chicken.
	{
		info->idletime = info->leveltime;

		// Check Offensive mana. Fireballs have free mana, but if powered up, use the alternate animation sequence.
		if (info->pers.weapon->tag == ITEM_WEAPON_FLYINGFIST || Weapon_CurrentShotsLeft(info) > 0)
			return (info->powerup_timer > info->leveltime) ? info->pers.weapon->altanimseq : info->pers.weapon->playeranimseq;
	}
	else
	{
		info->upperidle = true;
	}

	return ASEQ_NONE;
}

static int BranchUprReadySwordStaff(playerinfo_t* info)
{
	// No arm, no shot.
	if (info->flags & PLAYER_FLAG_NO_RARM)
		return ASEQ_NONE;

	if (info->edictflags & FL_CHICKEN)
	{
		info->upperidle = true;
		return ASEQ_NONE;
	}

	if (info->seqcmd[ACMDU_ATTACK])	// Not a chicken.
	{
		if (strcmp(info->pers.weapon->classname, "Weapon_SwordStaff") == 0) //TODO: check not needed?
		{
			info->idletime = info->leveltime;

			// Make sure we're not about to do a spinning attack.
			if (info->seqcmd[ACMDL_RUN_F] && info->groundentity != NULL)
				return ASEQ_NONE;

			if (info->advancedstaff)
			{
				if (info->seqcmd[ACMDL_ACTION] && info->seqcmd[ACMDL_BACK])
					return ASEQ_WSWORD_BACK;

				if (info->upperseq == ASEQ_WSWORD_STABHOLD)
					return (info->seqcmd[ACMDU_ATTACK] ? ASEQ_WSWORD_STABHOLD : ASEQ_WSWORD_PULLOUT);

				if (info->lowerseq == ASEQ_JUMPFWD && info->seqcmd[ACMDL_FWD])
					return ASEQ_WSWORD_DOWNSTAB;
			}

			return (info->irand(info, 0, 4) == 0 ? ASEQ_WSWORD_STEP : ASEQ_WSWORD_STD1);
		}
	}
	else
	{
		if (!info->seqcmd[ACMDL_RUN_F])
			info->block_timer = info->leveltime + 0.1f; // Auto blocking if not running.

		info->upperidle = true;
	}

	return ASEQ_NONE;
}

static int BranchUprReadyHellStaff(playerinfo_t* info)
{
	// No arm, no shot.
	if (info->flags & PLAYER_FLAG_NO_RARM)
		return ASEQ_NONE;

	if (info->seqcmd[ACMDU_ATTACK] && !(info->edictflags & FL_CHICKEN) && Weapon_CurrentShotsLeft(info) > 0) // Not a chicken.
	{
		info->idletime = info->leveltime;

		// If powered up, use the alternate animation sequence.
		if (strcmp(info->pers.weapon->classname, "item_weapon_hellstaff") == 0) //TODO: check not needed?
			return ((info->powerup_timer > info->leveltime) ? info->pers.weapon->altanimseq : info->pers.weapon->playeranimseq);
	}
	else
	{
		info->upperidle = true;
	}

	return ASEQ_NONE;
}

static int BranchUprReadyBow(playerinfo_t* info)
{
	// No arms, no shot.
	if (info->flags & PLAYER_FLAG_NO_LARM || info->flags & PLAYER_FLAG_NO_RARM)
		return ASEQ_NONE;

	if (info->seqcmd[ACMDU_ATTACK] && !(info->edictflags & FL_CHICKEN) && Weapon_CurrentShotsLeft(info) > 0) // Not a chicken.
	{
		info->idletime = info->leveltime;

		// If powered up, use the alternate animation sequence. //TODO: BranchUprReadyHellStaff and BranchUprReadySwordStaff do weapon->classname check before doing this. Why?
		return ((info->powerup_timer > info->leveltime) ? info->pers.weapon->altanimseq : info->pers.weapon->playeranimseq);
	}

	info->upperidle = true;

	return ASEQ_NONE;
}

// If we are out of bow ammo, then switch us to the next weapon.
int BranchCheckBowAmmo(playerinfo_t* info) //mxd. Named 'BranchCheckAmmo' in original version.
{
	if (info->isclient || Weapon_CurrentShotsLeft(info) > 0) // The client prediction shouldn't test the weapon.
		return ASEQ_NONE;

	info->G_WeapNext(info->self);

	const int seq = (info->pers.weapon->tag == ITEM_WEAPON_REDRAINBOW ? ASEQ_WRRBOW_END : ASEQ_WPHBOW_END); //mxd
	PlayerAnimSetUpperSeq(info, seq);

	return seq;
}

// If we are out of hellstaff ammo, then switch us to the next weapon.
int BranchCheckHellAmmo(playerinfo_t* info)
{
	if (info->isclient || Weapon_CurrentShotsLeft(info) > 0) // The client prediction shouldn't test the weapon.
		return ASEQ_NONE;

	info->G_WeapNext(info->self);
	PlayerAnimSetUpperSeq(info, ASEQ_WHELL_END);

	return ASEQ_WHELL_END;
}


/*-----------------------------------------------
	BranchUprReady
-----------------------------------------------*/

int BranchUprReady(playerinfo_t *playerinfo)
{
	assert(playerinfo);

	if((playerinfo->switchtoweapon!=playerinfo->pers.weaponready||playerinfo->pers.newweapon)&&
		!(playerinfo->edictflags&FL_CHICKEN))
	{
		// Not a chicken, so switch weapons.
		playerinfo->idletime=playerinfo->leveltime;	

		return(PlayerAnimWeaponSwitch(playerinfo));
	}

	switch(playerinfo->pers.weaponready)
	{
		case WEAPON_READY_SWORDSTAFF:
			return BranchUprReadySwordStaff(playerinfo);
			break;
		case WEAPON_READY_HELLSTAFF:
			return BranchUprReadyHellStaff(playerinfo);
			break;
		case WEAPON_READY_BOW:
			return BranchUprReadyBow(playerinfo);
			break;
		case WEAPON_READY_HANDS:
			return BranchUprReadyHands(playerinfo);
			break;
		default:		// In case Weapon_ready_none
			playerinfo->pers.weaponready = WEAPON_READY_HANDS;
			return BranchUprReadyHands(playerinfo);
			break;
	}

	return(ASEQ_NONE);
}

// if we are out of offensive mana, then switch us to the next weapon
int BranchCheckMana(playerinfo_t *playerinfo)
{
	if (Weapon_CurrentShotsLeft(playerinfo) || playerinfo->isclient)		// The client prediction shouldn't test the weapon.
		return(BranchUprReady(playerinfo));

	playerinfo->G_WeapNext(playerinfo->self);

   	return(ASEQ_NONE);
}


/*-----------------------------------------------
	BranchIdle
-----------------------------------------------*/

int BranchIdle(playerinfo_t *playerinfo)
{
	assert(playerinfo);
	
	if(!playerinfo->sv_cinematicfreeze)
	{
		//Run special cases if we're in the ready position
		if (playerinfo->lowerseq == ASEQ_IDLE_READY_GO ||
			playerinfo->lowerseq == ASEQ_IDLE_READY ||
			playerinfo->lowerseq == ASEQ_IDLE_LOOKR ||
			playerinfo->lowerseq == ASEQ_IDLE_LOOKL)
		{
			switch(playerinfo->irand(playerinfo, 0, 6))
			{
			case 0:
				return ASEQ_IDLE_LOOKR;
				break;
			case 1:
				return ASEQ_IDLE_LOOKL;
				break;
			case 2:
				return ASEQ_IDLE_READY_END;
				break;
			default:
				return ASEQ_NONE;
				break;
			}
		}
		else if ((playerinfo->pers.weaponready == WEAPON_READY_BOW) || (playerinfo->isclient))
		{	
			// Because the bow doesn't look right in some idles.
			switch(playerinfo->irand(playerinfo, 0, 10))
			{
				case 0:
					return ASEQ_IDLE_WIPE_BROW;
					break;
				case 1:
					if (playerinfo->irand(playerinfo, 0, 1) == 1)
						return ASEQ_IDLE_SCRATCH_ASS;

					break;
				case 2:
					return ASEQ_IDLE_LOOKBACK;
					break;
				default:
					return ASEQ_IDLE_READY;
					break;
			}
		}
		else if ( (playerinfo->pers.weaponready == WEAPON_READY_SWORDSTAFF) || (playerinfo->pers.weaponready == WEAPON_READY_HELLSTAFF))
		{
			// Because the staff doesn't look right in some idles.
			switch(playerinfo->irand(playerinfo, 0, 10))
			{
				case 0:
					return ASEQ_IDLE_FLY1;
					break;
				case 1:
					return ASEQ_IDLE_FLY2;
					break;
				case 2:
					return ASEQ_IDLE_WIPE_BROW;
					break;
				default:
					return ASEQ_IDLE_READY;
					break;
			}
		}
		else
		{
			switch(playerinfo->irand(playerinfo, 0, 10))
			{
				case 0:
					return ASEQ_IDLE_FLY1;
					break;
				case 1:
					return ASEQ_IDLE_FLY2;
					break;
				case 2:
					if (playerinfo->irand(playerinfo, 0, 1) == 1)
						return ASEQ_IDLE_SCRATCH_ASS;

					break;
				case 3:
					return ASEQ_IDLE_LOOKBACK;
					break;
				case 4:
					return ASEQ_IDLE_WIPE_BROW;
					break;
				default:
					return ASEQ_IDLE_READY;
					break;
			}
		}
	}

	return ASEQ_NONE;
}

