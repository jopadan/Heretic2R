//
// menu_options.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "menu.h"

extern cvar_t* m_banner_options;
extern cvar_t* m_banner_savecfg;

void M_Menu_Options_f(void);

// Functions shared between client and menu_options logic.
void CL_SaveConfig_f(void);