// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

BUILDIN_FUNC(getstuffitem)
{
	int get_count, i, reserved_id, type;
	unsigned short nameid, amount;
	struct item it;
	TBL_PC *sd;
	struct script_data *data;
	unsigned char flag = 0;
	struct item_data *id;

	if (!script_rid2sd(sd))
	{	// No player attached.
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	amount = script_getnum(st,3);
	type = script_getnum(st,4);
	switch(type)
	{
		case 1:
			reserved_id = battle_config.stuff_gvg_reserved_id;
			break;
		case 2:
			reserved_id = battle_config.stuff_pvp_reserved_id;
			break;
		default:
			reserved_id = battle_config.stuff_bg_reserved_id;
			break;
	}

	if( reserved_id <= 0 ) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	data = script_getdata(st, 2);
	get_val(st, data);
	if (data_isstring(data)) {
		const char *name = conv_str(st, data);
		id = itemdb_searchname(name);
		if (id == NULL)
		{	//Failed
			script_pushint(st, 0);
			return SCRIPT_CMD_SUCCESS;
		}
		nameid = id->nameid;
	} else if( data_isint(data) ) {// <item id>
		nameid = conv_num(st,data);
		if( !(id = itemdb_exists(nameid)) ){
			ShowError("buildin_getstuffitem: Nonexistant item %d requested.\n", nameid);
			return SCRIPT_CMD_FAILURE; //No item created.
		}
	} else {
		ShowError("buildin_getstuffitem: invalid data type for argument #1 (%d).", data->type);
		return SCRIPT_CMD_FAILURE;
	}

	if (!itemdb_exists(nameid))
	{	// Item does not exist.
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	memset(&it, 0, sizeof(it));
	it.nameid = nameid;
	it.identify = 1;
	it.bound = BOUND_NONE;
	it.card[0] = CARD0_CREATE;
	it.card[2] = GetWord(reserved_id, 0);
	it.card[3] = GetWord(reserved_id, 1);
	//Check if it's stackable.
	if (!itemdb_isstackable2(id))
		get_count = 1;
	else
		get_count = amount;

	for (i = 0; i < amount; i += get_count)
	{
		if (pc_additem(sd, &it, get_count, LOG_TYPE_SCRIPT))
			clif_additem(sd, 0, 0, flag);
	}

	script_pushint(st, flag);
	return SCRIPT_CMD_SUCCESS;
}
