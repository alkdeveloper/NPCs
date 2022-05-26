// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

#ifndef RESTOCK_HPP
#define RESTOCK_HPP

#include "../common/cbasetypes.hpp"

bool restock_check_vip(struct map_session_data* sd);
int restock_get_slot(struct map_session_data* sd, int nameid, int max);
int restock_get_item(struct map_session_data* sd, int nameid, int reserved_id);
int restock_add_item(struct map_session_data* sd, int nameid, int amount);
int restock_del_item(struct map_session_data* sd, int nameid);
int restock_clear(struct map_session_data* sd);
bool restock_check_zone(int16 m);

#endif /* RESTOCK_HPP */
