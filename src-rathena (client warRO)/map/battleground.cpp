// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "battleground.hpp"

#include "../common/cbasetypes.hpp"
#include "../common/malloc.hpp"
#include "../common/nullpo.hpp"
#include "../common/showmsg.hpp"
#include "../common/socket.hpp"
#include "../common/strlib.hpp"
#include "../common/sql.hpp"
#include "../common/timer.hpp"
#include "../common/utils.hpp"

#include "battle.hpp"
#include "clif.hpp"
#include "guild.hpp"
#include "homunculus.hpp"
#include "log.hpp"
#include "mercenary.hpp"
#include "mob.hpp"
#include "npc.hpp"
#include "party.hpp"
#include "pc.hpp"
#include "pc_groups.hpp"
#include "pet.hpp"
#include "storage.hpp"

#include <stdlib.h>
#include <math.h>

static DBMap* bg_team_db; // int bg_id -> struct battleground_data*
static unsigned int bg_team_counter = 0; // Next bg_id

struct battleground_data* bg_team_search(int bg_id)
{ // Search a BG Team using bg_id
	if( !bg_id )
		return NULL;

	return (struct battleground_data *)idb_get(bg_team_db, bg_id);
}

struct map_session_data* bg_getavailablesd(struct battleground_data *bg)
{
	int i;

	nullpo_retr(NULL, bg);

	ARR_FIND(0, MAX_BG_MEMBERS, i, bg->members[i].sd != NULL);

	return ( i < MAX_BG_MEMBERS ) ? bg->members[i].sd : NULL;
}

int bg_team_delete(int bg_id)
{ // Deletes BG Team from db
	int i;
	struct battleground_data *bg = bg_team_search(bg_id);

	if( bg == NULL )
		return 0;

	for( i = 0; i < MAX_BG_MEMBERS; i++ ) {
		struct map_session_data *sd;

		if( (sd = bg->members[i].sd) == NULL )
			continue;

		bg_send_dot_remove(sd);
		sd->bg_id = 0;
	}

	idb_remove(bg_team_db, bg_id);

	return 1;
}

int bg_team_warp(int bg_id, unsigned short mapindex, short x, short y)
{ // Warps a Team
	int i;
	struct battleground_data *bg = bg_team_search(bg_id);

	if( bg == NULL )
		return 0;

	for( i = 0; i < MAX_BG_MEMBERS; i++ ) {
		if( bg->members[i].sd != NULL ) {
			bg_respawn_timer_delete(bg->members[i].sd);
			pc_setpos(bg->members[i].sd, mapindex, x, y, CLR_TELEPORT);
		}
	}
	return 1;
}

int bg_send_dot_remove(struct map_session_data *sd)
{
	if( sd && sd->bg_id )
		clif_bg_xy_remove(sd);
	return 0;
}

int bg_team_join(int bg_id, struct map_session_data *sd, int flag)
{ // Player joins team
	int i;
	struct battleground_data *bg = bg_team_search(bg_id);

	if( bg == NULL || sd == NULL || sd->bg_id )
		return 0;

	ARR_FIND(0, MAX_BG_MEMBERS, i, bg->members[i].sd == NULL);
	if( i == MAX_BG_MEMBERS )
		return 0; // No free slots

	sd->bg_id = bg_id;
	sd->bg_position = -1;
	sd->idletime = last_tick;
	bg->members[i].sd = sd;
	bg->members[i].x = sd->bl.x;
	bg->members[i].y = sd->bl.y;
	bg->count++;

	guild_send_dot_remove(sd);

	for( i = 0; i < MAX_BG_MEMBERS; i++ ) {
		struct map_session_data *pl_sd;

		if( (pl_sd = bg->members[i].sd) == NULL )
			continue;

		if( pl_sd != sd )
			clif_hpmeter_single(sd->fd, pl_sd->bl.id, pl_sd->battle_status.hp, pl_sd->battle_status.max_hp);

		if( battle_config.bgw_enable )
			clif_guild_emblem(pl_sd,bg->g);
	}

	if (battle_config.bgw_enable) {
		sd->bg_respawn_timer_count = 10;
		sd->bg_respawn_timer = add_timer(gettick()+1, bg_respawn_timer, sd->bl.id, 0);
	
		// Force Refresh Patent
		bg_refresh_patent(bg_id);
	
		if (flag)
			bg_join_notify(bg,sd);
	
		clif_guild_notice(sd);
	}

	clif_bg_hp(sd);
	clif_bg_xy(sd);
	return 1;
}

int bg_team_leave(struct map_session_data *sd, enum bg_team_leave_type flag)
{ // Single Player leaves team
	int i, bg_id;
	struct battleground_data *bg;
	struct guild *g;

	if( sd == NULL || !sd->bg_id )
		return 0;

	bg_send_dot_remove(sd);
	bg_id = sd->bg_id;
	sd->bg_id = 0;

	if( (bg = bg_team_search(bg_id)) == NULL )
		return 0;

	ARR_FIND(0, MAX_BG_MEMBERS, i, bg->members[i].sd == sd);
	if( i < MAX_BG_MEMBERS ) // Removes member from BG
		memset(&bg->members[i], 0, sizeof(bg->members[0]));

	bg->count--;

	if (--bg->count != 0)
		bg_leave_notify(bg, sd, flag);

	// Return Normal Guild
	status_change_end(&sd->bl,SC_GUILDAURA,INVALID_TIMER);
	status_change_end(&sd->bl,SC_BATTLEORDERS,INVALID_TIMER);
	status_change_end(&sd->bl,SC_REGENERATION,INVALID_TIMER);
		
	if( sd->status.guild_id && (g = guild_search(sd->status.guild_id)) != NULL )
	{
		clif_guild_belonginfo(sd);
		clif_guild_basicinfo(sd);
		clif_guild_allianceinfo(sd);
		clif_guild_memberlist(sd);
		clif_guild_skillinfo(sd);
		clif_guild_emblem(sd, g);
	}

	clif_name_area(&sd->bl);
	clif_guild_emblem_area(&sd->bl);

	// Special Flag Drop
	if( sd->bg_flags.nd != NULL )
		bg_flag_drop(sd);

	if( bg->logout_event[0] && flag )
		npc_event(sd, bg->logout_event, 0);

	// Without Event and Refresh Position.
	if (battle_config.bgw_enable)
	{
		sd->bg_position = 0;
		sd->bg_kick.count = 0;
		memset(&sd->bg_score, 0, sizeof(sd->bg_score));
		memset(&sd->bg_kick.mes, '\0', sizeof(sd->bg_kick.mes));
		memset(&sd->bg_kick.char_id, 0,  sizeof(sd->bg_kick.char_id));
		bg_block_skill_end(sd);
		
		if( battle_config.bgw_name_position && bg->count > 0 )
			bg_refresh_patent(bg_id);
		else
			bg_clean_skill(bg);

		if (flag && bg->count <= 0 && bg->without_event[0])
		{
			if (!npc_event_do(bg->without_event))
				ShowError("bg_team_leave: Battleground Without Event '%s' not found!\n", bg->without_event);
		}
	}
	return bg->count;
}

int bg_member_respawn(struct map_session_data *sd)
{ // Respawn after killed
	struct battleground_data *bg;

	if( sd == NULL || !pc_isdead(sd) || !sd->bg_id || (bg = bg_team_search(sd->bg_id)) == NULL )
		return 0;

	if( bg->mapindex == 0 )
		return 0; // Respawn not handled by Core

	pc_setpos(sd, bg->mapindex, bg->x, bg->y, CLR_OUTSIGHT);
	status_revive(&sd->bl, 1, 100);

	if( battle_config.bgw_enable && !map_getmapflag(sd->bl.m,MF_NOBGRESPAWN) && bg->respawn_x && bg->respawn_y )
	{
		sd->bg_respawn_timer_count = 10;
		sd->bg_respawn_timer = add_timer(gettick()+1, bg_respawn_timer, sd->bl.id, 0);
	}

	return 1; // Warped
}

int bg_create(int bg_id, unsigned short mapindex, short rx, short ry, short rsx, short rsy, int army, const char *ev, const char *dev, const char *wev)
{
	struct battleground_data *bg;
	bg_team_counter++;

	CREATE(bg, struct battleground_data, 1);
	bg->bg_id = bg_id;
	bg->count = 0;
	bg->mapindex = mapindex;
	bg->x = rx;
	bg->y = ry;
	bg->respawn_x = rsx;
	bg->respawn_y = rsy;
	bg->army = army;
	bg->g = &bg_guild[army];
	safestrncpy(bg->logout_event, ev, sizeof(bg->logout_event));
	safestrncpy(bg->die_event, dev, sizeof(bg->die_event));
	safestrncpy(bg->without_event, wev, sizeof(bg->without_event));

	memset(&bg->members, 0, sizeof(bg->members));
	idb_put(bg_team_db, bg_id, bg);

	return bg->bg_id;
}

int bg_team_get_id(struct block_list *bl)
{
	nullpo_ret(bl);
	switch( bl->type ) {
		case BL_PC:
			return ((TBL_PC*)bl)->bg_id;
		case BL_PET:
			if( ((TBL_PET*)bl)->master )
				return ((TBL_PET*)bl)->master->bg_id;
			break;
		case BL_MOB: {
			struct map_session_data *msd;
			struct mob_data *md = (TBL_MOB*)bl;

			if( md->special_state.ai && (msd = map_id2sd(md->master_id)) != NULL )
				return msd->bg_id;

			return md->bg_id;
		}
		case BL_HOM:
			if( ((TBL_HOM*)bl)->master )
				return ((TBL_HOM*)bl)->master->bg_id;
			break;
		case BL_MER:
			if( ((TBL_MER*)bl)->master )
				return ((TBL_MER*)bl)->master->bg_id;
			break;
		case BL_NPC:
			if( ((TBL_NPC*)bl)->subtype == NPCTYPE_SCRIPT )
				return ((TBL_NPC*)bl)->battleground.bg_id;
			break;
		case BL_SKILL:
			return ((TBL_SKILL*)bl)->group->bg_id;
	}

	return 0;
}

int bg_send_message(struct map_session_data *sd, const char *mes, int len)
{
	struct battleground_data *bg;

	nullpo_ret(sd);

	if( sd->bg_id == 0 || (bg = bg_team_search(sd->bg_id)) == NULL )
		return 0;
	
	clif_bg_message(bg, sd->bl.id, sd->status.name, mes, len);

	return 0;
}

/**
 * @see DBApply
 */
int bg_send_xy_timer_sub(DBKey key, DBData *data, va_list ap)
{
	struct battleground_data *bg = (struct battleground_data *)db_data2ptr(data);
	struct map_session_data *sd;
	int i;

	nullpo_ret(bg);

	for( i = 0; i < MAX_BG_MEMBERS; i++ ) {
		if( (sd = bg->members[i].sd) == NULL )
			continue;

		if( sd->bl.x != bg->members[i].x || sd->bl.y != bg->members[i].y ) { // xy update
			bg->members[i].x = sd->bl.x;
			bg->members[i].y = sd->bl.y;
			clif_bg_xy(sd);
		}

		// [CreaitveSD] Battleground Warfare
		if( battle_config.bgw_enable ) {
			int idle_autokick = battle_config.bgw_idle_autokick;

			if( idle_autokick && DIFF_TICK(last_tick, sd->idletime) >= idle_autokick ) {
				bg_team_leave(sd, BGTL_KICK_IDLE);
				clif_messagecolor(&sd->bl, BG_COLOR, msg_txt(sd,battle_config.bgw_msg_txt+67), true, SELF);
			}
			else
				bg_guild_requestinfo(sd); // Fix Temporary
		}
	}

	return 0;
}

TIMER_FUNC(bg_send_xy_timer){
	bg_team_db->foreach(bg_team_db, bg_send_xy_timer_sub, tick);

	return 0;
}

// [CreativeSD]: Battleground Warfare
#include "bgw_func.inc"

void do_init_battleground(void)
{
	bg_team_db = idb_alloc(DB_OPT_RELEASE_DATA);
	bg_reward_db = idb_alloc(DB_OPT_RELEASE_DATA);
	add_timer_func_list(bg_send_xy_timer, "bg_send_xy_timer");
	add_timer_func_list(bg_report_afk, "bg_report_afk");
	add_timer_func_list(bg_respawn_timer, "bg_respawn_timer");
	add_timer_func_list(bg_digit_timer, "bg_digit_timer");
	add_timer_func_list(bg_flag_alarm_npc, "bg_flag_alarm_npc");
	add_timer_func_list(bg_flag_alarm_player, "bg_flag_alarm_player");
	add_timer_interval(gettick() + battle_config.bg_update_interval, bg_send_xy_timer, 0, 0, battle_config.bg_update_interval);

	// Create Fake Guilds
	bg_create_guild();

	// Load Rewards from SQL
	bg_load_rewards_from_sql();

	ShowMessage(CL_WHITE"[BattleGround]: " CL_RESET " BattleGround Warfare (version: %s) successfully initialized.\n", bg_version);
	ShowMessage(CL_WHITE"[BattleGround]: " CL_RESET " by (c) CreativeSD, suport in www.creativesd.com.br\n");
}

void do_final_battleground(void)
{
	bg_team_db->destroy(bg_team_db, bg_team_db_final);
	bg_reward_db->destroy(bg_reward_db, NULL);
}
