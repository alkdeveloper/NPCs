// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos
#ifndef _CHAR_RESTOCK_HPP_
#define _CHAR_RESTOCK_HPP_

#include "../common/cbasetypes.hpp"

bool char_restock_load(uint32 char_id, struct item* item);
bool char_restock_save(uint32 char_id, struct mmo_charstatus* status);

#endif /* _CHAR_RESTOCK_HPP_ */
