// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

	// Restock System
	{ "restock_enable",						&battle_config.restock_enable,                      1,     0,            1,         },
	{ "restock_msg_txt",					&battle_config.restock_msg_txt,                  1842,     0,      INT_MAX,         },
	{ "restock_vip_type",					&battle_config.restock_vip_type,                    0,     0,            3,         },
	{ "restock_group_level",				&battle_config.restock_group_level,                 1,     1,           99,         },
	{ "restock_regular_max_items",			&battle_config.restock_regular_max_items,           1,     1,  MAX_RESTOCK,         },
	{ "restock_vip_max_items",				&battle_config.restock_vip_max_items,				1,     1,  MAX_RESTOCK,         },
	{ "restock_max_stack",					&battle_config.restock_max_stack,                   1,     1,  MAX_AMOUNT,          },
	{ "restock_charge_oncmd",				&battle_config.restock_charge_oncmd,			 0x00,  0x00,         0x03,         },
	{ "restock_regular_zeny_cost",          &battle_config.restock_regular_zeny_cost,         1000,    0,     MAX_ZENY,         },
	{ "restock_vip_zeny_cost",              &battle_config.restock_vip_zeny_cost,             1000,    0,     MAX_ZENY,         },
	{ "restock_maps",                       &battle_config.restock_maps,                      0x00, 0x00,        0x711,         },
