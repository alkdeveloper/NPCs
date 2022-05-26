// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

BUILDIN_FUNC(addrestock)
{
	TBL_PC *sd;
	struct item_data *id = NULL;
	int i = 0, amount = 0;
	unsigned short nameid;

	if (!script_nick2sd(4,sd)) {
		script_pushint(st,0);
		return SCRIPT_CMD_SUCCESS;
	}

	nameid = script_getnum(st, 2);
	if (!(id = itemdb_exists(nameid))) {
		ShowError("buildin_addrestock: Invalid item '%d'.\n", id);
		return SCRIPT_CMD_FAILURE; //No item created.
	}

	amount = script_getnum(st,3);

	if( amount <= 0 ) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	script_pushint(st, restock_add_item(sd, (int)nameid, amount));
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(delrestock)
{
	TBL_PC *sd;
	struct item_data* id = NULL;
	int i = 0;
	unsigned short nameid;

	if (!script_nick2sd(3,sd))
	{
		script_pushint(st,0);
		return true;
	}

	nameid = script_getnum(st, 2);
	if (!(id = itemdb_exists(nameid))) {
		ShowError("buildin_delrestock: Invalid item '%d'.\n", id);
		return SCRIPT_CMD_FAILURE; //No item created.
	}

	script_pushint(st, restock_del_item(sd, nameid));
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(getrestocklist)
{
	TBL_PC *sd;
	int i = 0, j = 0, max;

	if (!script_rid2sd(sd))
	{
		script_pushint(st,0);
		return SCRIPT_CMD_SUCCESS;
	}

	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;

	// Clear Items
	restock_clear(sd);

	for( i=0; i < max; i++ )
	{
		if( sd->status.restock[i].nameid && sd->status.restock[i].amount ) {
			pc_setreg(sd,reference_uid(add_str("@restock_nameid"), j),sd->status.restock[i].nameid);
			pc_setreg(sd,reference_uid(add_str("@restock_amount"), j),sd->status.restock[i].amount);
			j++;
		}
	}
	pc_setreg(sd,add_str("@restock_count"),j);
	script_pushint(st,j);
	return SCRIPT_CMD_SUCCESS;
}
