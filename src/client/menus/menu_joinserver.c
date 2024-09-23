//
// menu_joinserver.h
//
// Copyright 1998 Raven Software
//

#include <windows.h>
#include "client.h"
#include "menu_joinserver.h"
#include "menu_addressbook.h"
#include "cl_strings.h"
#include "sound.h"

cvar_t* m_banner_join;
cvar_t* m_item_refresh;

#define MAX_LOCAL_SERVERS	8
#define NO_SERVER_STRING	"<*****>"

static menuframework_s s_joinserver_menu;

static menuaction_s s_joinserver_address_book_action;
static menuaction_s s_joinserver_search_action;
static menuaction_s s_joinserver_server_actions[MAX_LOCAL_SERVERS];

static int m_num_servers;

// User-readable information.
static char local_server_names[MAX_LOCAL_SERVERS][80];

// Network address.
static netadr_t local_server_netadr[MAX_LOCAL_SERVERS];

// Q2 counterpart
static void AddressBookFunc(void* data)
{
	M_Menu_AddressBook_f();
}

static void SearchLocalGames(void)
{
	m_num_servers = 0;

	for (int i = 0; i < MAX_LOCAL_SERVERS; i++)
		strcpy_s(local_server_names[i], sizeof(local_server_names[i]), NO_SERVER_STRING); //mxd. strcpy -> strcpy_s

	M_DrawTextBox(8, 72, 36, 3);
	M_Print(32, 80, GM_CH_SERVERS, TextPalette[P_WHITE]);
	S_StopAllSounds_Sounding();

	// The text box won't show up unless we do a buffer swap.
	re.EndFrame();
	Sleep(500); // H2

	// Send out info packets.
	CL_PingServers_f();
}

// Q2 counterpart
static void SearchLocalGamesFunc(void* data)
{
	SearchLocalGames();
}

static void JoinServerFunc(void* self)
{
	char buffer[128];

	const int index = (menuaction_s*)self - s_joinserver_server_actions;
	if (index >= m_num_servers || Q_stricmp(local_server_names[index], NO_SERVER_STRING) == 0)
		return;

	S_StopAllSounds_Sounding(); // H2
	Com_sprintf(buffer, sizeof(buffer), "connect %s\n", NET_AdrToString(&local_server_netadr[index]));
	Cbuf_AddText(buffer);
	M_ForceMenuOff();
	M_UpdateOrigMode(); // H2
}

void JoinServer_MenuInit(void)
{
	static char name_address[MAX_QPATH];
	static char name_refresh[MAX_QPATH];

	s_joinserver_menu.nitems = 0;

	Com_sprintf(name_address, sizeof(name_address), "\x02%s", m_banner_address->string);
	s_joinserver_address_book_action.generic.type = MTYPE_ACTION;
	s_joinserver_address_book_action.generic.name = name_address;
	s_joinserver_address_book_action.generic.width = re.BF_Strlen(name_address);
	s_joinserver_address_book_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_joinserver_address_book_action.generic.x = 0;
	s_joinserver_address_book_action.generic.y = 0;
	s_joinserver_address_book_action.generic.callback = AddressBookFunc;

	Com_sprintf(name_refresh, sizeof(name_refresh), "\x02%s", m_item_refresh->string);
	s_joinserver_search_action.generic.type = MTYPE_ACTION;
	s_joinserver_search_action.generic.name = name_refresh;
	s_joinserver_search_action.generic.width = re.BF_Strlen(name_refresh);
	s_joinserver_search_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_joinserver_search_action.generic.x = 0;
	s_joinserver_search_action.generic.y = 20;
	s_joinserver_search_action.generic.callback = SearchLocalGamesFunc;

	for (int i = 0; i < MAX_LOCAL_SERVERS; i++)
	{
		menuaction_s* item = &s_joinserver_server_actions[i];
		item->generic.type = MTYPE_ACTION;
		strcpy_s(local_server_names[i], sizeof(local_server_names[i]), NO_SERVER_STRING);
		item->generic.name = local_server_names[i];
		item->generic.width = re.BF_Strlen(local_server_names[i]);
		item->generic.flags = QMF_LEFT_JUSTIFY | QMF_MULTILINE | QMF_SELECT_SOUND;
		item->generic.x = 0;
		item->generic.y = 60 + i * 20;
		item->generic.callback = JoinServerFunc;
	}

	Menu_AddItem(&s_joinserver_menu, &s_joinserver_address_book_action.generic);
	Menu_AddItem(&s_joinserver_menu, &s_joinserver_search_action.generic);

	for (int i = 0; i < MAX_LOCAL_SERVERS; i++)
		Menu_AddItem(&s_joinserver_menu, &s_joinserver_server_actions[i]);

	Menu_Center(&s_joinserver_menu);
	SearchLocalGames();
}

static void JoinServer_MenuDraw(void)
{
	char name[MAX_QPATH];

	// Draw menu BG.
	re.BookDrawPic(0, 0, "book/back/b_conback8.bk", cls.m_menuscale);

	if (cls.m_menualpha == 0.0f)
		return;

	// Draw menu title.
	Com_sprintf(name, sizeof(name), "\x03%s", m_banner_join->string);
	const int x = M_GetMenuLabelX(re.BF_Strlen(name));
	const int y = M_GetMenuOffsetY(&s_joinserver_menu);
	re.DrawBigFont(x, y, name, cls.m_menualpha);

	// Update server name items width.
	for (int i = 0; i < MAX_LOCAL_SERVERS; i++)
		s_joinserver_server_actions[i].generic.width = re.BF_Strlen(local_server_names[i]);

	// Draw menu items.
	s_joinserver_menu.x = M_GetMenuLabelX(s_joinserver_menu.width);
	Menu_Draw(&s_joinserver_menu);
}

// Q2 counterpart
static const char* JoinServer_MenuKey(const int key)
{
	return Default_MenuKey(&s_joinserver_menu, key);
}

// Q2 counterpart
void M_Menu_JoinServer_f(void)
{
	JoinServer_MenuInit();
	M_PushMenu(JoinServer_MenuDraw, JoinServer_MenuKey);
}