// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef BATTLEGROUND_HPP
#define BATTLEGROUND_HPP

#include "../common/cbasetypes.hpp"
#include "../common/mmo.hpp" // struct party

#define MAX_BG_MEMBERS 30

// Enable Shared Codes
#define BGW_ENABLE

enum bg_team_leave_type {
	BGTL_LEFT = 0x0,
	BGTL_QUIT = 0x1,
	BGTL_AFK  = 0x2,
	BGTL_KICK = 0x3,
	BGTL_KICK_IDLE = 0x4,
};

struct battleground_member_data {
	unsigned short x, y;
	struct map_session_data *sd;
	unsigned afk : 1;
};

struct battleground_data {
	unsigned int bg_id;
	unsigned char count;
	struct battleground_member_data members[MAX_BG_MEMBERS];
	// BG Cementery
	unsigned short mapindex, x, y;
	// Logout Event
	char logout_event[EVENT_NAME_LENGTH];
	char die_event[EVENT_NAME_LENGTH];

	// Battleground Warfare
	// Fake Guild
	struct guild *g;
	int army, master_id;
	t_tick skill_block_timer[MAX_GUILDSKILL];
	// Timer Count
	int timerdigit_count, timerdigit;
	// Respawn Timer
	unsigned short respawn_x, respawn_y;
	// Without Event
	char without_event[EVENT_NAME_LENGTH];
};

void do_init_battleground(void);
void do_final_battleground(void);

struct battleground_data* bg_team_search(int bg_id);
int bg_send_dot_remove(struct map_session_data *sd);
int bg_team_get_id(struct block_list *bl);
struct map_session_data* bg_getavailablesd(struct battleground_data *bg);

int bg_create(int bg_id, unsigned short mapindex, short rx, short ry, short rsx, short rsy, int army, const char *ev, const char *dev, const char *wev);
int bg_team_join(int bg_id, struct map_session_data *sd, int flag);
int bg_team_delete(int bg_id);
int bg_team_leave(struct map_session_data *sd, enum bg_team_leave_type flag);
int bg_team_warp(int bg_id, unsigned short mapindex, short x, short y);
int bg_member_respawn(struct map_session_data *sd);
int bg_send_message(struct map_session_data *sd, const char *mes, int len);

// Battleground Warfare
#include "bgw_def.inc"

#endif /* BATTLEGROUND_HPP */
