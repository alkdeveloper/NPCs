// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

#include "restock.hpp"

#include "../common/cbasetypes.hpp"
#include "../common/mmo.hpp"
#include "../common/nullpo.hpp"
#include "../common/utilities.hpp"
#include "../common/utils.hpp"

#include "battle.hpp"
#include "map.hpp"
#include "pc.hpp"
#include "storage.hpp"

#include <stdlib.h>

bool restock_check_vip(struct map_session_data* sd)
{
	if (battle_config.restock_vip_type == 1 && pc_get_group_id(sd) >= battle_config.restock_group_level)
		return true;
	else if (battle_config.restock_vip_type == 2 && pc_get_group_level(sd) >= battle_config.restock_group_level)
		return true;
	else if( pc_isvip(sd) )
		return true;
	else
		return false;
}

int restock_get_slot(struct map_session_data *sd, int nameid, int max)
{
	int i = 0;

	nullpo_retr(0,sd);

	ARR_FIND(0, max, i, sd->status.restock[i].nameid == nameid);
	if( i >= max )
		return max;

	return i;
}

int restock_get_item(struct map_session_data *sd, int nameid, int reserved_id)
{
	int i, amount = 0, max;

	nullpo_retr(0,sd);

	if( nameid < 0 )
		return 0;

	if( sd->state.storage_flag )
		return 0;

	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;
	if( (i = restock_get_slot(sd,nameid,max)) >= max )
		return 0;

	amount = sd->status.restock[i].amount;

	if( reserved_id )
		ARR_FIND(0, MAX_STORAGE, i, sd->storage.u.items_storage[i].nameid == nameid && sd->storage.u.items_storage[i].card[0]==CARD0_CREATE && MakeDWord(sd->storage.u.items_storage[i].card[2], sd->storage.u.items_storage[i].card[3])==reserved_id);
	else
		ARR_FIND(0, MAX_STORAGE, i, sd->storage.u.items_storage[i].nameid == nameid);

	if( i >= MAX_STORAGE )
		return 0;

	if( sd->storage.u.items_storage[i].amount < amount )
		amount = sd->storage.u.items_storage[i].amount;

	if( amount )
		storage_storageget(sd, &sd->storage, i, amount);

	return amount;
}

int restock_add_item(struct map_session_data *sd, int nameid, int amount)
{
	int i, max;

	nullpo_retr(-1,sd);

	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;
	i = restock_get_slot(sd,nameid,max);
	if( i >= max )
		i = restock_get_slot(sd,0,max);

	// Check New Slot
	if( i < MAX_RESTOCK ) {
		sd->status.restock[i].nameid = nameid;
		sd->status.restock[i].amount = amount;
		return 1;
	}
	return 0;
}

int restock_del_item(struct map_session_data *sd, int nameid)
{
	int i, max;

	nullpo_retr(-1, sd);

	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;
	i = restock_get_slot(sd,nameid,max);

	if( i < max ) {
		sd->status.restock[i].nameid = 0;
		sd->status.restock[i].amount = 0;
		return 1;
	}
	return 0;
}

int restock_clear(struct map_session_data* sd)
{
	int i, c = 0, max;

	nullpo_retr(-1, sd);

	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;

	for( i = 0; i < MAX_RESTOCK; i++ ) {
		struct item_data* id;
		if( sd->status.restock[i].nameid == 0 )
			continue;
		else if( sd->status.restock[i].amount == 0 || (id = itemdb_exists(sd->status.restock[i].nameid)) == NULL ) {
			sd->status.restock[i].nameid = 0;
			sd->status.restock[i].amount = 0;
		}
		else c++;
	}
	return c;
}

bool restock_check_zone(int16 m)
{
	struct map_data *mapdata = map_getmapdata(m);

	if( mapdata == NULL )
		return false;
	if( mapdata->flag[MF_RESTOCK_ON] )
		return true;
	if( mapdata->flag[MF_RESTOCK_OFF] )
		return false;
	if( !mapdata->flag[MF_PVP] && !mapdata->flag[MF_GVG] && !mapdata->flag[MF_BATTLEGROUND] && !mapdata->flag[MF_GVG_CASTLE] )
		return true;
	if( (battle_config.restock_maps&0x1) && mapdata->flag[MF_PVP] )
		return true;
	if( (battle_config.restock_maps&0x10) && mapdata->flag[MF_GVG] )
		return true;
	if( (battle_config.restock_maps&0x100) && mapdata->flag[MF_BATTLEGROUND] )
		return true;
	if( (battle_config.restock_maps&0x200) && mapdata->flag[MF_GVG_CASTLE] && !agit_flag && !agit2_flag && !agit3_flag )
		return true;
	if( (battle_config.restock_maps&0x400) && mapdata->flag[MF_GVG_CASTLE] && (agit_flag || agit2_flag || agit3_flag) )
		return true;

	return false;
}
