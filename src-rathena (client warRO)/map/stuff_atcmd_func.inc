// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

ACMD_FUNC(stuffitem)
{
	char item_name[100];
	int number = 0, type = 0, reserved_id = 0;
	char flag = 0;
	struct item item_tmp;
	struct item_data *item_data[10];
	int get_count, i, j=0;
	char *itemlist;

	nullpo_retr(-1, sd);
	memset(item_name, '\0', sizeof(item_name));

	if (!message || !*message || (
		sscanf(message, "\"%99[^\"]\" %d %11d", item_name, &type, &number) < 2 &&
		sscanf(message, "%99s %d %11d", item_name, &type, &number) < 2
	)) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+3))); // Please enter an item name or ID (usage: @stuffitem <item name/ID> <amount> <quantity>).
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+4)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+5)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+6)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+7)));
		return -1;
	}

	type--;
	if( type < 0 || type > 2 ) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+8)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+3))); // Please enter an item name or ID (usage: @stuffitem <item name/ID> <amount> <quantity>).
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+4)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+5)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+6)));
		clif_displaymessage(fd, msg_txt(sd,(battle_config.stuff_msg_txt+7)));
		return -1;
	}

	switch(type) {
		case 1:
			reserved_id = battle_config.stuff_pvp_reserved_id;
			break;
		case 2:
			reserved_id = battle_config.stuff_gvg_reserved_id;
			break;
		default:
			reserved_id = battle_config.stuff_bg_reserved_id;
	}

	if( reserved_id <= 0 ) {
		char output[128];
		sprintf(output, msg_txt(sd,battle_config.stuff_msg_txt+9), msg_txt(sd,battle_config.stuff_msg_txt+type));
		clif_displaymessage(fd, output);
		return 0;
	}

	itemlist = strtok(item_name, ":");
	while (itemlist != NULL && j<10) {
		if ((item_data[j] = itemdb_searchname(itemlist)) == NULL &&
		    (item_data[j] = itemdb_exists(atoi(itemlist))) == NULL){
			clif_displaymessage(fd, msg_txt(sd,19)); // Invalid item ID or name.
			return -1;
		}
		itemlist = strtok(NULL, ":"); //next itemline
		j++;
	}

	if (number <= 0)
		number = 1;
	get_count = number;

	for(j--; j>=0; j--){ //produce items in list
		unsigned short item_id = item_data[j]->nameid;

		//Check if it's stackable.
		if (!itemdb_isstackable2(item_data[j]))
			get_count = 1;

		for (i = 0; i < number; i += get_count) {
			// if not pet egg
			if (!pet_create_egg(sd, item_id)) {
				memset(&item_tmp, 0, sizeof(item_tmp));
				item_tmp.nameid = item_id;
				item_tmp.identify = 1;
				item_tmp.card[0] = CARD0_CREATE;
				item_tmp.card[2] = GetWord(reserved_id, 0);
				item_tmp.card[3] = GetWord(reserved_id, 1);
				item_tmp.bound = BOUND_NONE;
				if ((flag = pc_additem(sd, &item_tmp, get_count, LOG_TYPE_COMMAND)))
					clif_additem(sd, 0, 0, flag);
			}
		}
	}

	if (flag == 0)
		clif_displaymessage(fd, msg_txt(sd,18)); // Item created.
	return 0;
}
