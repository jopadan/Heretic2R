//
// cl_smk.c -- libsmacker interface (https://libsmacker.sourceforge.net)
//
// Copyright 1998 Raven Software
//

#include "client.h"
#include "screen.h"
#include "snd_dll.h"
#include "sound.h"
#include "libsmacker/smacker.h"

static cvar_t* cin_rate;

static int cinematic_frame;
static int cinematic_total_frames;

static smk smk_obj;
static const byte* smk_frame;
static const byte* smk_palette;
static int smk_width;
static int smk_height;

static void SMK_Stop(void)
{
	NOT_IMPLEMENTED
}

// Returns number of frames in .smk
static int SMK_Open(const char* name)
{
	char backdrop[MAX_QPATH];
	char overlay[MAX_QPATH];
	int frame_count;

	smk_obj = smk_open_file(name, SMK_MODE_DISK);
	if (smk_obj == NULL)
		return 0;

	smk_enable_all(smk_obj, SMK_VIDEO_TRACK | SMK_AUDIO_TRACK_0);
	smk_info_all(smk_obj, NULL, &frame_count, NULL);
	smk_info_video(smk_obj, &smk_width, &smk_height, NULL);

	smk_first(smk_obj);
	smk_palette = smk_get_palette(smk_obj);

	if ((smk_width & 7) != 0 || (smk_height & 7) != 0)
	{
		Com_Printf("...Smacker file must but a multiple of 8 high and wide\n");
		SMK_Stop();

		return 0;
	}

	re.DrawInitCinematic(smk_width, smk_height, overlay, backdrop);

	return frame_count;
}

static void SMK_DoFrame(void)
{
	smk_frame = smk_get_video(smk_obj);
	smk_next(smk_obj);
}

void SMK_Shutdown(void)
{
	if (smk_obj != NULL)
	{
		smk_close(smk_obj);
		smk_obj = NULL;
		smk_frame = NULL;
		smk_palette = NULL;
	}
}

static void SCR_DoCinematicFrame(void)
{
	SMK_DoFrame();
	cinematic_frame++;
}

void SCR_PlayCinematic(const char* name)
{
	char smk_filepath[MAX_OSPATH];

	SCR_BeginLoadingPlaque();

	cin_rate = Cvar_Get("cin_rate", "15.0", 0); // H2

	//CDAudio_Stop(); //mxd. Already done in SCR_BeginLoadingPlaque()!
	if (sound_library != NULL)
		S_Shutdown();

	//mxd. Skip 'SmackW32.dll' loading logic.

	sprintf_s(smk_filepath, sizeof(smk_filepath), "video/%s", name); //mxd. sprintf -> sprintf_s
	const char* path = FS_GetPath(smk_filepath);
	if (path == NULL)
	{
		Com_Printf("...Unable to find file\n");
		SCR_FinishCinematic();

		return;
	}

	sprintf_s(smk_filepath, sizeof(smk_filepath), "%s/video/%s", path, name); //mxd. sprintf -> sprintf_s
	Com_Printf("Opening cinematic file : %s.....\n", smk_filepath);

	cinematic_frame = 0;
	cinematic_total_frames = SMK_Open(smk_filepath);
	if (cinematic_total_frames == 0)
	{
		Com_Printf("...Unable to open file\n");
		SCR_FinishCinematic();

		return;
	}

	cl.cinematictime = (int)((float)(cls.realtime - 2000) / cin_rate->value);

	SCR_DoCinematicFrame();
	SCR_DrawCinematic();

	Cvar_SetValue("paused", 0);
	cls.state = ca_connected;

	Key_ClearStates();
	cls.key_dest = key_game;
}

void SCR_DrawCinematic(void)
{
	if (cl.cinematictime > 0)
		re.DrawCinematic(smk_width, smk_height, smk_frame, (const paletteRGB_t*)smk_palette, 1.0f);
}

void SCR_RunCinematic(void)
{
	if (cl.cinematictime < 1)
		return;

	// Pause if menu or console is up.
	if (cls.key_dest != key_game)
	{
		cl.cinematictime = cls.realtime - cinematic_frame * 1000 / (int)cin_rate->value; // Q2: / 14
		return;
	}

	const int frame = (int)((float)(cls.realtime - cl.cinematictime) * cin_rate->value / 1000.0f);

	if (frame <= cinematic_frame) //mxd. Original code waits using SmackWait function.
		return;

	if (frame > cinematic_frame + 1)
	{
		Com_Printf("Dropped frame: %i > %i\n", frame, cinematic_frame + 1);
		cl.cinematictime = cls.realtime - cinematic_frame * 1000 / (int)cin_rate->value; // Q2: / 14
	}

	SCR_DoCinematicFrame();

	if (cinematic_frame >= cinematic_total_frames - 1)
		SCR_FinishCinematic();
	else
		SCR_EndLoadingPlaque();
}

void SCR_StopCinematic(void)
{
	cl.cinematictime = 0; // Done
	SMK_Shutdown(); // H2
}

// Called when either the cinematic completes, or it is aborted.
void SCR_FinishCinematic(void)
{
	// Tell the server to advance to the next map / cinematic.
	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	SZ_Print(&cls.netchan.message, va("nextserver %i\n", cl.servercount));

	SCR_StopCinematic();
	if (sound_library != NULL) //TODO: not needed?
		S_Init();
}