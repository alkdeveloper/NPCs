// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

ACMD_FUNC(restockadd)
{
	char item_name[100], output[128];
	int number, zeny = 0;
	struct item_data* id;

	nullpo_retr(-1, sd);
	memset(item_name, '\0', sizeof(item_name));

	if( !battle_config.restock_enable ) {
		clif_displaymessage(fd, msg_txt(sd,battle_config.restock_msg_txt));
		return 0;
	}

	if (!message || !*message || (
		sscanf(message, "\"%99[^\"]\" %11d", item_name, &number) < 2 &&
		sscanf(message, "%99s %11d", item_name, &number) < 2
	)) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+2)));
		return -1;
	}

	if( number <= 0 || number > battle_config.restock_max_stack ) {
		sprintf(output, msg_txt(sd,battle_config.restock_msg_txt+3), battle_config.restock_max_stack);
		clif_displaymessage(fd, output);
		return -1;
	}

	if( (id = itemdb_searchname(item_name)) == NULL && (id = itemdb_exists(atoi(item_name))) == NULL )
	{
		clif_displaymessage(fd, msg_txt(sd,19)); // Invalid item ID or name.
		return -1;
	}

	if( battle_config.restock_charge_oncmd ) {
		if( restock_check_vip(sd) )
			zeny = battle_config.restock_vip_zeny_cost;
		else  if( battle_config.restock_regular_zeny_cost )
			zeny = battle_config.restock_regular_zeny_cost;
	}

	if( zeny && sd->status.zeny < zeny ) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+13)));
		return -1;
	}

	if( restock_add_item(sd, id->nameid, number) ) {
		if( zeny )
			pc_payzeny(sd, zeny, LOG_TYPE_COMMAND, NULL);
		sprintf(output, msg_txt(sd,(battle_config.restock_msg_txt+8)), id->jname);
		clif_displaymessage(fd, output);
	}
	else {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+1)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+6)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+7)));
	}
	return 0;
}

ACMD_FUNC(restockrem)
{
	char item_name[100], output[128];
	struct item_data* id;

	nullpo_retr(-1, sd);
	memset(item_name, '\0', sizeof(item_name));
	
	if( !battle_config.restock_enable ) {
		clif_displaymessage(fd, msg_txt(sd,battle_config.restock_msg_txt));
		return 0;
	}

	if (!message || !*message || (
		sscanf(message, "\"%99[^\"]\"", item_name) < 1 &&
		sscanf(message, "%99s", item_name) < 1
	)) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+4)));
		return -1;
	}

	if( (id = itemdb_searchname(item_name)) == NULL && (id = itemdb_exists(atoi(item_name))) == NULL )
	{
		clif_displaymessage(fd, msg_txt(sd,19)); // Invalid item ID or name.
		return -1;
	}

	if( restock_del_item(sd, id->nameid) ) {
		sprintf(output, msg_txt(sd,(battle_config.restock_msg_txt+9)), id->jname);
		clif_displaymessage(fd, output);
	}
	else {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+4)));
	}
	return 0;
}

ACMD_FUNC(restocklist)
{
	int i, n = 0, count, max = 0;
	char output[128];
	struct item_data* id;

	nullpo_retr(-1, sd);
	
	if( !battle_config.restock_enable ) {
		clif_displaymessage(fd, msg_txt(sd,battle_config.restock_msg_txt));
		return 0;
	}

	// Remove Invalid Items and Count
	count = restock_clear(sd);

	// Get Max Items
	max = restock_check_vip(sd) ? battle_config.restock_vip_max_items : battle_config.restock_regular_max_items;

	if( !count )
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+11)));
	else {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.restock_msg_txt+10)));
		for( i=0; i < max; i++ ) {
			if( sd->status.restock[i].nameid == 0 || sd->status.restock[i].amount == 0 )
				continue;
			if( (id = itemdb_exists(sd->status.restock[i].nameid)) == NULL )
				continue;
			sprintf(output, " - (ID: %d) %dx %s", sd->status.restock[i].nameid, sd->status.restock[i].amount, id->jname);
			clif_displaymessage(fd, output);
			n++;
		}

		sprintf(output, msg_txt(sd,(battle_config.restock_msg_txt+12)), n);
		clif_displaymessage(fd, output);
	}
	return 0;
}

ACMD_FUNC(restock)
{
	struct npc_data *nd = npc_name2id("Restock#main");

	nullpo_retr(-1, sd);

	if( !battle_config.restock_enable ) {
		clif_displaymessage(fd, msg_txt(sd,battle_config.restock_msg_txt));
		return 0;
	}

	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.restock_msg_txt+14)));
		return -1;
	}

	run_script(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);
	return 0;
}
