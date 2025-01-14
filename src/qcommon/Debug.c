//
// Debug.c -- Development aids
//
// Copyright 2024 mxd
//

#include "Debug.h"
#include "client.h"
#include "vid.h"

#if _DEBUG
	#include <windows.h>
	#include <stdio.h>
#endif

// Print to Visual Studio console.
GAME_DECLSPEC void DBG_IDEPrint(const char* fmt, ...)
{
#if _DEBUG
	va_list argptr;
	char msg[1024];

	va_start(argptr, fmt);
	vsprintf_s(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	strcat_s(msg, sizeof(msg), "\n");
	OutputDebugString(msg);
#endif
}

// Print vector.
GAME_DECLSPEC char* pv(const vec3_t v)
{
	static char buf[8][128];
	static int buf_index;

	buf_index = (buf_index + 1) % 7;

	sprintf_s(buf[buf_index], sizeof(buf[buf_index]), "[%f %f %f]", v[0], v[1], v[2]);

	return buf[buf_index];
}

// Print short vector.
GAME_DECLSPEC char* psv(const short* v)
{
	static char buf[8][128];
	static int buf_index;

	buf_index = (buf_index + 1) % 7;

	sprintf_s(buf[buf_index], sizeof(buf[buf_index]), "[%i %i %i]", v[0], v[1], v[2]);

	return buf[buf_index];
}

#define NUM_DEBUG_MESSAGES	16

typedef struct
{
	char title[32];
	char message[256];
} DebugHudMessage_t;

static DebugHudMessage_t dbg_messages[NUM_DEBUG_MESSAGES];

GAME_DECLSPEC void DBG_HudPrint(const int slot, const char* label, const char* fmt, ...)
{
#if _DEBUG

	va_list argptr;

	if (slot < 0 || slot >= NUM_DEBUG_MESSAGES)
	{
		DBG_IDEPrint("DBG_HudPrint: invalid HUD slot %i!", slot);
		return;
	}

	DebugHudMessage_t* msg = &dbg_messages[slot];

	// Title
	strcpy_s(msg->title, sizeof(msg->title), label);

	// Message
	va_start(argptr, fmt);
	vsprintf_s(msg->message, sizeof(msg->message), fmt, argptr);
	va_end(argptr);

#endif
}

void DBG_DrawMessages(void)
{
#if _DEBUG

	int ox = 0;
	int oy = viddef.height / 2 - 4;

	for (int i = 0; i < NUM_DEBUG_MESSAGES; i++)
	{
		const DebugHudMessage_t* msg = &dbg_messages[i];
		ox = max(ox, (int)strlen(msg->title));
	}

	// Convert to char offset, add pad...
	ox = (ox + 1) * 8;

	for (int i = 0; i < NUM_DEBUG_MESSAGES; i++)
	{
		const DebugHudMessage_t* msg = &dbg_messages[i];

		const int t_len = (int)strlen(msg->title);
		const int m_len = (int)strlen(msg->message);

		if (t_len == 0 || m_len == 0)
			continue;

		// Title
		DrawString(ox - t_len * 8, oy, msg->title, TextPalette[P_CYAN], -1);

		// Message
		DrawString(ox + 8, oy, msg->message, TextPalette[P_CYAN], -1);

		oy += 10;
	}

#endif
}