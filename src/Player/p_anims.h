//
// p_anims.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "Player.h"

extern PLAYER_API void PlayerBasicAnimReset(playerinfo_t *info);
extern PLAYER_API void PlayerAnimReset(playerinfo_t *info);
extern PLAYER_API void PlayerAnimSetLowerSeq(playerinfo_t *info, int seq);
extern PLAYER_API void PlayerAnimSetUpperSeq(playerinfo_t *info, int seq);
extern PLAYER_API void PlayerAnimUpperIdle(playerinfo_t *info);
extern PLAYER_API void PlayerAnimLowerIdle(playerinfo_t *info);
extern PLAYER_API void PlayerAnimUpperUpdate(playerinfo_t *info);
extern PLAYER_API void PlayerAnimLowerUpdate(playerinfo_t *info);
extern PLAYER_API void PlayerAnimSetVault(playerinfo_t *info, int seq);
extern PLAYER_API void PlayerPlayPain(const playerinfo_t *info, int type);

extern int PlayerAnimWeaponSwitch(playerinfo_t *info);

enum seq_cmd_e 
{
	ACMD_NONE,
	ACMDU_ATTACK,
	ACMDU_SWITCHWPN,
	ACMDU_DEFSPELL,
	ACMDL_ACTION,
	ACMDL_JUMP,
	ACMDL_CROUCH,
	ACMDL_CREEP_F,
	ACMDL_WALK_F,
	ACMDL_RUN_F,
	ACMDL_FWD,
	ACMDL_CREEP_B,
	ACMDL_WALK_B,
	ACMDL_RUN_B,
	ACMDL_BACK,
	ACMDL_STRAFE_L,
	ACMDL_STRAFE_R,
	ACMDL_ROTATE_L,
	ACMDL_ROTATE_R,
	ACMDL_QUICKTURN,

	// NOTE: This is assumed to be 20 by the seqcmd array in playerinfo_t in p_types.h.
	// DO NOT CHANGE THIS without mirroring this change in playerinfo_t.
	ACMD_MAX
};

enum seq_anim_e //TODO: rename WSWORD to WSTAFF
{
	ASEQ_NONE,
	ASEQ_WSWORD_STD1,
	ASEQ_WSWORD_STD2,
	ASEQ_WSWORD_STEP2,
	ASEQ_WSWORD_STEP,
	ASEQ_WSWORD_BACK,
	ASEQ_WSWORD_DOWNSTAB,
	ASEQ_WSWORD_STABHOLD,
	ASEQ_WSWORD_PULLOUT,
	ASEQ_WSWORD_BLOCK_L,
	ASEQ_WSWORD_BLOCK2_L,
	ASEQ_WSWORD_BLOCKATK_L,
	ASEQ_WSWORD_BLOCK_R,
	ASEQ_WSWORD_BLOCK2_R,
	ASEQ_WSWORD_BLOCKATK_R,
	ASEQ_WSWORD_BLOCKED_L,
	ASEQ_WSWORD_BLOCKED_R,
	ASEQ_WFIREBALL,
	ASEQ_WARRAY,
	ASEQ_WSPHERE_GO,
	ASEQ_WSPHERE_HOLD,
	ASEQ_WSPHERE_FIRE1,
	ASEQ_WSPHERE_FIRE2,
	ASEQ_WSPHERE_FIRE3,
	ASEQ_WSPHERE_FIRE4,
	ASEQ_WFIREWALL,
	ASEQ_WRIPPER,
	ASEQ_WBIGBALL,
	ASEQ_WBLAST,
	ASEQ_WRRBOW_GO,
	ASEQ_WRRBOW_DRAW,
	ASEQ_WRRBOW_HOLD,
	ASEQ_WRRBOW_FIRE,
	ASEQ_WRRBOW_READY,
	ASEQ_WRRBOW_END,
	ASEQ_WPHBOW_GO,
	ASEQ_WPHBOW_DRAW,
	ASEQ_WPHBOW_HOLD,
	ASEQ_WPHBOW_FIRE,
	ASEQ_WPHBOW_READY,
	ASEQ_WPHBOW_END,
	ASEQ_WHELL_GO,
	ASEQ_WHELL_FIRE1,
	ASEQ_WHELL_FIRE2,
	ASEQ_WHELL_END,
	ASEQ_SPELL_DEF,
	ASEQ_HAND2HAND,
	ASEQ_HAND2SWD,
	ASEQ_HAND2HELL,
	ASEQ_HAND2BOW,
	ASEQ_SWD2HAND,
	ASEQ_SWD2HELL,
	ASEQ_SWD2BOW,
	ASEQ_HELL2HAND,
	ASEQ_HELL2SWD,
	ASEQ_HELL2BOW,
	ASEQ_BOW2HAND,
	ASEQ_BOW2SWD,
	ASEQ_BOW2HELL,
	ASEQ_BOW2BOW,
	ASEQ_PUSHBUTTON_GO,
	ASEQ_PUSHLEVERLEFT,
	ASEQ_PUSHLEVERRIGHT,
	ASEQ_LOWER_BASE,
	ASEQ_STAND,
	ASEQ_PIVOTL_GO,
	ASEQ_PIVOTL,
	ASEQ_PIVOTL_END,
	ASEQ_PIVOTR_GO,
	ASEQ_PIVOTR,
	ASEQ_PIVOTR_END,
	ASEQ_TURN180,
	ASEQ_RUNF_GO,
	ASEQ_RUNF,
	ASEQ_RUNF_END,
	ASEQ_WALKF_GO,
	ASEQ_WALKF,
	ASEQ_WALKF_END,
	ASEQ_CREEPF,
	ASEQ_CREEPF_END,
	ASEQ_WALKB,
	ASEQ_CREEPB,
	ASEQ_CREEPB_END,
	ASEQ_CROUCH_GO,
	ASEQ_CROUCH,
	ASEQ_CROUCH_END,
	ASEQ_CROUCH_PIVOTL,
	ASEQ_CROUCH_PIVOTR,
	ASEQ_STRAFEL,
	ASEQ_STRAFEL_END,
	ASEQ_STRAFER,
	ASEQ_STRAFER_END,
	ASEQ_JUMPSTD_GO,
	ASEQ_JUMPFWD_SGO,
	ASEQ_JUMPFWD_WGO,
	ASEQ_JUMPFWD_RGO,
	ASEQ_JUMPFWD,
	ASEQ_JUMPUP,
	ASEQ_JUMPUP_LOOP,
	ASEQ_JUMPFLIPL,
	ASEQ_JUMPFLIPR,
	ASEQ_JUMPSPRINGBGO,
	ASEQ_JUMPSPRINGB,
	ASEQ_JUMPFLIPB,
	ASEQ_ROLLDIVEF_W,
	ASEQ_ROLLDIVEF_R,
	ASEQ_ROLL_FROM_FFLIP,
	ASEQ_POLEVAULT1_W,
	ASEQ_POLEVAULT1_R,
	ASEQ_POLEVAULT2,
	ASEQ_LANDLIGHT,
	ASEQ_LANDHEAVY,
	ASEQ_FALLWALK_GO,
	ASEQ_FALLRUN_GO,
	ASEQ_FALL,
	ASEQ_FALLARMSUP,
	ASEQ_VAULT_LOW,
	ASEQ_VAULT_HIGH,
	ASEQ_PULLUP_WALL,
	ASEQ_SSWIM_IDLE,
	ASEQ_SSWIMF_GO,
	ASEQ_SSWIMF,
	ASEQ_SSWIMF_END,
	ASEQ_SSWIMB_GO,
	ASEQ_SSWIMB,
	ASEQ_SSWIMB_END,
	ASEQ_SSWIML_GO,	
	ASEQ_SSWIMR_GO,
	ASEQ_SSWIML,
	ASEQ_SSWIMR,
	ASEQ_SSWIML_END,
	ASEQ_SSWIMR_END,
	ASEQ_WSWORD_SPIN,
	ASEQ_WSWORD_SPIN2,
	ASEQ_WSWORD_SPINBLOCKED,
	ASEQ_WSWORD_SPINBLOCKED2,
	ASEQ_WSWORD_LOWERDOWNSTAB,
	ASEQ_WSWORD_LOWERPULLOUT,
	ASEQ_PULLUP_HALFWALL,
	ASEQ_TUMBLEON1,
	ASEQ_TUMBLEON2,
	ASEQ_LSTAIR4,
	ASEQ_LSTAIR8,
	ASEQ_LSTAIR12,
	ASEQ_LSTAIR16,
	ASEQ_RSTAIR4,
	ASEQ_RSTAIR8,
	ASEQ_RSTAIR12,
	ASEQ_RSTAIR16,
	ASEQ_IDLE_READY_GO,
	ASEQ_IDLE_READY,
	ASEQ_IDLE_READY_END,
	ASEQ_IDLE_LOOKL,
	ASEQ_IDLE_LOOKR,
	ASEQ_PAIN_A,
	ASEQ_PAIN_B,
	ASEQ_IDLE_FLY1,
	ASEQ_IDLE_FLY2,
	ASEQ_FALLL,
	ASEQ_FALLR,
	ASEQ_FALLL_END,
	ASEQ_FALLR_END,
	ASEQ_DEATH_A,
	ASEQ_USWIMF_GO,
	ASEQ_USWIMF,
	ASEQ_USWIMF_END,
	ASEQ_DIVE,
	ASEQ_USWIMB_GO,
	ASEQ_USWIMB,
	ASEQ_USWIMB_END,
	ASEQ_USWIML_GO,	
	ASEQ_USWIMR_GO,
	ASEQ_USWIML,
	ASEQ_USWIMR,
	ASEQ_USWIML_END,
	ASEQ_USWIMR_END,
	ASEQ_SLIDE_FORWARD,
	ASEQ_SLIDE_BACKWARD,
	ASEQ_SSWIM_RESURFACE,
	ASEQ_ROLL_L,
	ASEQ_ROLL_R,
	ASEQ_USWIM_IDLE,
	ASEQ_ROLL_B,
	ASEQ_CLIMB_ON,
	ASEQ_CLIMB_UP_START_R,
	ASEQ_CLIMB_UP_START_L,
	ASEQ_CLIMB_UP_R,
	ASEQ_CLIMB_UP_L,
	ASEQ_CLIMB_DOWN_START_R,
	ASEQ_CLIMB_DOWN_START_L,
	ASEQ_CLIMB_DOWN_R,
	ASEQ_CLIMB_DOWN_L,
	ASEQ_CLIMB_OFF,
	ASEQ_CLIMB_HOLD_R,
	ASEQ_CLIMB_HOLD_L,
	ASEQ_CLIMB_SETTLE_R,
	ASEQ_CLIMB_SETTLE_L,
	ASEQ_KNOCKDOWN,
	ASEQ_KNOCKDOWN_GETUP,
	ASEQ_KNOCKDOWN_EVADE,
	ASEQ_SHRINE,
	ASEQ_TAKEPUZZLEPIECE,
	ASEQ_TAKEPUZZLEUNDERWATER,
	ASEQ_DROWN,
	ASEQ_FORWARD_FLIP_L_GO,
	ASEQ_FORWARD_FLIP_R_GO,
	ASEQ_FORWARD_FLIP_L,
	ASEQ_FORWARD_FLIP_R,
	ASEQ_CSTRAFE_LEFT,
	ASEQ_CSTRAFE_RIGHT,
	ASEQ_WSTRAFE_LEFT,
	ASEQ_WSTRAFE_RIGHT,
	ASEQ_RSTRAFE_LEFT,
	ASEQ_RSTRAFE_RIGHT,
	ASEQ_JUMPBACK_SGO,
	ASEQ_JUMPBACK_WGO,
	ASEQ_JUMPBACK_RGO,
	ASEQ_JUMPBACK,
	ASEQ_JUMPFLIPBACK,
	ASEQ_JUMPLEFT_SGO,
	ASEQ_JUMPLEFT_WGO,
	ASEQ_JUMPLEFT_RGO,
	ASEQ_JUMPLEFT,
	ASEQ_JUMPFLIPLEFT,
	ASEQ_JUMPRIGHT_SGO,
	ASEQ_JUMPRIGHT_WGO,
	ASEQ_JUMPRIGHT_RGO,
	ASEQ_JUMPRIGHT,
	ASEQ_JUMPFLIPRIGHT,
	ASEQ_DROWN_IDLE,
	ASEQ_DASH_LEFT_GO,
	ASEQ_DASH_LEFT,
	ASEQ_DASH_RIGHT_GO,
	ASEQ_DASH_RIGHT,
	ASEQ_CSTRAFEB_LEFT,
	ASEQ_CSTRAFEB_RIGHT,
	ASEQ_WSTRAFEB_LEFT,
	ASEQ_WSTRAFEB_RIGHT,
	ASEQ_OVERHANG,
	ASEQ_DEATH_B,
	ASEQ_DEATH_FLYFWD,
	ASEQ_DEATH_FLYBACK,	
	ASEQ_DEATH_CHOKE,
	ASEQ_IDLE_LOOKBACK,
	ASEQ_IDLE_SCRATCH_ASS,
	ASEQ_IDLE_WIPE_BROW,
	ASEQ_CROUCH_WALK_F,
	ASEQ_CROUCH_WALK_B,
	ASEQ_CROUCH_WALK_L,
	ASEQ_CROUCH_WALK_R,
	ASEQ_SSWIM_FAST_GO,
	ASEQ_SSWIM_FAST,
	ASEQ_MAX
};

typedef struct seq_data2_s
{
	char nosplit;
	int	viewheight;
	int collideseq;
	int waterseq;
} seq_data2_t;

extern seq_data2_t PlayerSeqData2[ASEQ_MAX];