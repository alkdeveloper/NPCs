// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

ACMD_FUNC(reloadbg)
{
	struct npc_data *nd = npc_name2id("BattleGround#queue_main");

	if( !battle_config.bgw_enable )
		return true;

	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 23)));
		return -1;
	}

	npc_event(sd, "BattleGround#queue_main::OnCmdReload", 0);
	clif_messagecolor(&sd->bl, BG_COLOR, msg_txt(sd, (battle_config.bgw_msg_txt + 24)), true, SELF);
	return 0;
}

ACMD_FUNC(bgregister)
{
	struct npc_data *nd = npc_name2id("BattleGround#cmd_register");

	if( !battle_config.bgw_enable )
		return true;

	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 25)));
		return -1;
	}

	run_script(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);
	return 0;
}

ACMD_FUNC(bgjoin)
{
	struct npc_data *nd = npc_name2id("BattleGround#cmd_join");

	if( !battle_config.bgw_enable )
		return true;

	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 26)));
		return -1;
	}

	run_script(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);
	return 0;
}

ACMD_FUNC(bgleave)
{
	int16 m = sd->bl.m;

	if( !battle_config.bgw_enable )
		return true;

	if (sd->bg_id)
	{
		if (map_getmapflag(m, MF_BATTLEGROUND))
		{
			bg_team_leave(sd, BGTL_QUIT);
			if (!sd->bg_id)
			{
				if (battle_config.bgw_afk_warp_save_point)
					pc_setpos(sd, sd->status.save_point.map, sd->status.save_point.x, sd->status.save_point.y, CLR_TELEPORT);
				else {
					unsigned short map_index = mapindex_name2id("bat_room");
					if (map_index)
						pc_setpos(sd, map_index, 154, 150, CLR_TELEPORT);
				}
				return 0;
			}
			else {
				clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 27)));
				return 0;
			}
		}
		else {
			// Não ativar labeis de eventos por estar fora da arena.
			bg_team_leave(sd, BGTL_LEFT);
			if (sd->bg_id)
			{
				clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 28)));
				return 1;
			}
			else {
				clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 27)));
				return -1;
			}
		}
	}
	else {
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 29)));
		return -1;
	}
}

ACMD_FUNC(bgunregister)
{
	struct npc_data *nd = npc_name2id("BattleGround#cmd_unregister");

	if( !battle_config.bgw_enable )
		return true;

	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd, (battle_config.bgw_msg_txt + 25)));
		return -1;
	}

	run_script(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);
	return 0;
}

ACMD_FUNC(bgreportafk)
{
	struct map_session_data *pl_sd = NULL;
	char output[128];

	if( !battle_config.bgw_enable )
		return true;
	
	memset(atcmd_player_name, '\0', sizeof(atcmd_player_name));
	if (!*message || !*message || sscanf(message, "%23[^\n]", atcmd_player_name) < 1)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+30)));
		return -1;
	}

	if ((pl_sd = map_nick2sd(atcmd_player_name,true)) == NULL && (pl_sd = map_charid2sd(atoi(atcmd_player_name))) == NULL)
	{
		sprintf(atcmd_output, msg_txt(sd,(battle_config.bgw_msg_txt+31)), atcmd_player_name);
		clif_displaymessage(fd, atcmd_output);
		return -1;
	}

	if (!pl_sd->bg_id)
	{
		sprintf(atcmd_output, msg_txt(sd,(battle_config.bgw_msg_txt+32)), message);
		clif_displaymessage(fd, atcmd_output);
		return -1;
	}

	if (battle_config.bgw_afk_team_report && sd->bg_id != pl_sd->bg_id)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+33)));
		return -1;
	}

	if (pl_sd->npc_id)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+34)));
		return -1;
	}

	if (!DIFF_TICK(last_tick, pl_sd->idletime))
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+35)));
		return -1;
	}

	if (pl_sd->bg_afk_timer && pl_sd->bg_afk_timer != INVALID_TIMER)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+36)));
		return -1;
	}

	clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+37)));
	sprintf(output, msg_txt(sd,(battle_config.bgw_msg_txt+38)), battle_config.bgw_afk_timer);
	clif_messagecolor(&pl_sd->bl, BG_COLOR, output, true, SELF);

	pl_sd->bg_afk_timer = add_timer(gettick() + (battle_config.bgw_afk_timer * 1000), bg_report_afk, pl_sd->bl.id, 0);
	return 0;
}

ACMD_FUNC(bgwaiting)
{
	struct npc_data *nd;
	struct script_state *st;
	int arg1 = 0, arg2 = 0, count = 0, nargs = 0;

	if( !battle_config.bgw_enable )
		return true;

	if (strcmpi("bgwaiting",command))
	{
		nd = npc_name2id("BattleGround#cmd_waiting");
		if (nd == NULL)
		{
			clif_displaymessage(sd->fd, msg_txt(sd,(battle_config.bgw_msg_txt+39)));
			return -1;
		}
	}
	else
	{
		nd = npc_name2id("BattleGround#cmd_playing");
		if (nd == NULL)
		{
			clif_displaymessage(sd->fd, msg_txt(sd,(battle_config.bgw_msg_txt+40)));
			return -1;
		}
	}

	if (*message)
		count = sscanf(message, "%2d %1d", &arg1, &arg2);

	st = script_alloc_state(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);

	if (count > 0)
		setd_sub(st, NULL, ".@atcmd_parameters", nargs++, (void*)__64BPRTSIZE(arg1), NULL);

	if (count > 1)
		setd_sub(st, NULL, ".@atcmd_parameters", nargs++, (void*)__64BPRTSIZE(arg2), NULL);

	setd_sub(st, NULL, ".@atcmd_numparameters", 0, (void*)__64BPRTSIZE(nargs), NULL);
	run_script_main(st);
	return 0;
}

ACMD_FUNC(bgranking)
{
	struct npc_data *nd;
	struct script_state *st;
	int arg1 = 0, arg2 = 0, arg3 = 0, count = 0, nargs = 0;

	if( !battle_config.bgw_enable )
		return true;

	nd = npc_name2id("BattleGround#cmd_ranking");
	if (nd == NULL)
	{
		clif_displaymessage(sd->fd, msg_txt(sd,(battle_config.bgw_msg_txt+41)));
		return -1;
	}

	if (*message)
		count = sscanf(message, "%2d %1d %2d", &arg1, &arg2, &arg3);

	st = script_alloc_state(nd->u.scr.script, 0, sd->bl.id, nd->bl.id);

	if (count > 0)
		setd_sub(st, NULL, ".@atcmd_parameters", nargs++, (void*)__64BPRTSIZE(arg1), NULL);

	if (count > 1)
		setd_sub(st, NULL, ".@atcmd_parameters", nargs++, (void*)__64BPRTSIZE(arg2), NULL);

	if (count > 2)
		setd_sub(st, NULL, ".@atcmd_parameters", nargs++, (void*)__64BPRTSIZE(arg3), NULL);

	setd_sub(st, NULL, ".@atcmd_numparameters", 0, (void*)__64BPRTSIZE(nargs), NULL);
	run_script_main(st);
	return 0;
}

ACMD_FUNC(bgorder)
{
	struct battleground_data *bg = NULL;
	nullpo_retr(-1,sd);

	if( !battle_config.bgw_enable )
		return true;

	if( !message || !*message )
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+42)));
		return -1;
	}

	if( map_getmapflag(sd->bl.m,MF_BATTLEGROUND) && sd->bg_id )
	{
		if( (bg = bg_team_search(sd->bg_id)) == NULL && bg->master_id != sd->status.char_id )
		{
			clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+43)));
			return -1;
		}
		clif_broadcast2(&sd->bl, message, (int)strlen(message)+1, BG_COLOR, 0x190, 12, 0, 0, BG);
	}
	else {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+44)));
		return -1;
	}

	return 0;
}

ACMD_FUNC(bgchangeleader) {
	struct battleground_data *bg = NULL;
	struct map_session_data *pl_sd = NULL;
	struct map_session_data *tsd = NULL;
	char output[128];
	int i, showlist = 0;

	nullpo_retr(-1, sd);

	if( !battle_config.bgw_enable )
		return true;

	memset(atcmd_player_name, '\0', sizeof(atcmd_player_name));

	if (!sd->bg_id)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+45)));
		return -1;
	}

	bg = bg_team_search(sd->bg_id);
	if (bg == NULL || bg->master_id != sd->status.char_id )
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+46)));
		return -1;
	}
	else if (bg->count <= 1)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+47)));
		return -1;
	}
	else if (!message || !*message || sscanf(message, "%23[^\n]", atcmd_player_name) < 1)
		showlist = 1;
	else if ((pl_sd = map_nick2sd(atcmd_player_name,true)) == NULL || pl_sd->bg_id != sd->bg_id) {
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+48)));
		showlist = 1;
	}
	else {
		if (bg_change_master(sd->bg_id, pl_sd->status.char_id, sd))
		{
			sprintf(output, msg_txt(sd,(battle_config.bgw_msg_txt+49)), pl_sd->status.name);
			clif_displaymessage(fd, output);
			return 0;
		}
		else {
			sprintf(output, msg_txt(sd,(battle_config.bgw_msg_txt+50)), pl_sd->status.name);
			clif_displaymessage(fd, output);
			showlist = 1;
		}
	}

	if (showlist)
	{
		clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+51)));
		for (i = 0; i < bg->count; i++)
		{
			if (bg->members[i].sd == sd)
				continue;

			tsd = bg->members[i].sd;
			if (tsd == NULL)
				continue;

			sprintf(output, msg_txt(sd,(battle_config.bgw_msg_txt+52)), tsd->status.name, tsd->status.base_level, tsd->status.job_level, job_name(tsd->status.class_));
			clif_displaymessage(fd, output);
		}
	}

	return -1;
}

ACMD_FUNC(bgreloadrewards)
{
	if( !battle_config.bgw_enable )
		return true;

	do_bg_reward_reload();
	clif_displaymessage(fd, msg_txt(sd,(battle_config.bgw_msg_txt+53)));
	return 0;
}
