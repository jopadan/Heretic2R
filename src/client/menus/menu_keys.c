//
// menu_keys.c -- logic shared between actionkeys, movekeys, doubletapkeys and shortkeys menus.
//
// Copyright 1998 Raven Software
//

#include "client.h"
#include "menu_keys.h"

#include "menu_citymap.h"
#include "menu_help.h"
#include "menu_objectives.h"
#include "menu_worldmap.h"


typedef struct
{
	char* command;
	cvar_t** label;
} BindData_s;

#define NUM_BINDS	49

static BindData_s bindnames[NUM_BINDS] =
{
	// Action keys.
	{ "+attack",			&m_item_attack },
	{ "+defend",			&m_item_defend },
	{ "+action",			&m_item_action },
	{ "+lookup",			&m_item_lookup },
	{ "+lookdown",		&m_item_lookdown },
	{ "centerview",		&m_item_centerview },
	{ "+mlook",			&m_item_mouselook },
	{ "+klook",			&m_item_keyboardlook },
	{ "+lookaround",		&m_item_lookaround },
	{ "weapnext",		&m_item_nextweapon },
	{ "weapprev",		&m_item_prevweapon },
	{ "defnext",			&m_item_nextdef },
	{ "defprev",			&m_item_prevdef },

	// Move keys.
	{ "+forward",		&m_item_walkforward },
	{ "+back",			&m_item_backpedal },
	{ "+left",			&m_item_turnleft },
	{ "+right",			&m_item_turnright },
	{ "+creep",			&m_item_creep },
	{ "+speed",			&m_item_run },
	{ "+moveleft",		&m_item_stepleft },
	{ "+moveright",		&m_item_stepright },
	{ "+strafe",			&m_item_sidestep },
	{ "+moveup",			&m_item_up },
	{ "+movedown",		&m_item_down },
	{ "+quickturn",		&m_item_quickturn },

	// Shortcut keys.
	{ "menu_help",		&m_item_helpscreen },
	{ "use *powerup",	&m_item_powerup },
	{ "use *ring",		&m_item_bluering },
	{ "use *meteor",		&m_item_meteor },
	{ "use *morph",		&m_item_morph },
	{ "use *tele",		&m_item_teleport },
	{ "use *lshield",	&m_item_shield },
	{ "use *tornado",	&m_item_tornado },
	{ "+inventory",		&m_item_inventory },
	{ "menu_objectives",	&m_banner_objectives },
	{ "menu_world_map",	&m_banner_worldmap },
	{ "menu_city_map",	&m_banner_citymap },
	{ "messagemode",		&m_item_messagemode },
	{ "score",			&m_item_frags },

	// Double-tap keys.
	{ "+flip_left",		&m_item_flipleft },
	{ "+flip_right",		&m_item_flipright },
	{ "+flip_forward",	&m_item_flipforward },
	{ "+flip_back",		&m_item_flipback },
	{ "+roll_left",		&m_item_rollleft },
	{ "+roll_right",		&m_item_rollright },
	{ "+roll_forward",	&m_item_rollforward },
	{ "+roll_back",		&m_item_rollback },
	{ "+quickturn",		&m_item_quickturn },
	{ "+spinattack",		&m_item_spinattack }
};

int keys_count;
int keys_category_offset;
qboolean use_doublebind;
qboolean bind_grab;

menuframework_s s_keys_menu;
static menuinputkey_s s_keys_items[14];

static void UnbindCommand(char* command)
{
	NOT_IMPLEMENTED
}

static void KeyBindingFunc(void* self)
{
	NOT_IMPLEMENTED
}

void Keys_MenuInit(void)
{
	static char key_labels[NUM_BINDS][MAX_QPATH];

	s_keys_menu.nitems = 0;

	int oy = 0;
	for (int i = 0; i < keys_count; i++, oy += 20)
	{
		const cvar_t* label = *bindnames[keys_category_offset + i].label;
		Com_sprintf(key_labels[i], sizeof(key_labels[i]), "\x02%s", label->string);

		menuinputkey_s* item = &s_keys_items[i];
		item->generic.type = MTYPE_INPUT_KEY;
		item->generic.flags = QMF_SINGLELINE;
		item->generic.x = 0;
		item->generic.y = oy;
		item->generic.localdata[0] = i;
		item->generic.name = key_labels[i];
		item->generic.width = re.BF_Strlen(key_labels[i]);

		Menu_AddItem(&s_keys_menu, item);
	}

	Menu_Center(&s_keys_menu);
}

const char* Keys_MenuKey(const int key)
{
	char cmd[1024];

	menuinputkey_s* item = (menuinputkey_s*)Menu_ItemAtCursor(&s_keys_menu);
	const int bind_index = item->generic.localdata[0] + keys_category_offset;

	if (bind_grab)
	{
		if (key != K_ESCAPE && key != '`')
		{
			const char* format = (use_doublebind ? "bind_double \"%s\" \"%s\"\n" : "bind \"%s\" \"%s\"\n"); // H2
			Com_sprintf(cmd, sizeof(cmd), format, Key_KeynumToString(key), bindnames[bind_index].command);
			Cbuf_InsertText(cmd);
		}

		bind_grab = false;
		return NULL;
	}

	switch (key)
	{
		case K_ENTER:
		case K_KP_ENTER:
			KeyBindingFunc(item);
			return NULL;

		case K_BACKSPACE:
		case K_DEL:
		case K_KP_DEL:
			UnbindCommand(bindnames[bind_index].command);
			return NULL;

		default:
			return Default_MenuKey(&s_keys_menu, key);
	}
}

void M_FindKeysForCommand(const int command_index, int* twokeys)
{
	const char* command = bindnames[keys_category_offset + command_index].command;
	const int len = (int)strlen(command);
	int count = 0;

	for (int i = 0; i < 256; i++)
	{
		const char* b = (use_doublebind ? keybindings_double[i] : keybindings[i]);

		if (b != NULL && strncmp(b, command, len) == 0)
		{
			twokeys[count] = i;
			count++;

			if (count == 2)
				break;
		}
	}
}