// © Creative Services and Development
// Site Oficial: www.creativesd.com.br
// Termos de Contrato e Autoria em: http://creativesd.com.br/?p=termos

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "map.hpp" // EVENT_NAME_LENGTH

// Está definição deve estar de acordo com o 'SCRIPT_MAX_ARRAYSIZE', caso você aumente esta definição,
// variáveis vetorizadas de scripts não irão aceitar a indexação e haverá perca de dados em processos de scripts.
//
// Is setting should be in accordance with the 'SCRIPT_MAX_ARRAYSIZE' if you increase this setting,
// variable vectorized scripts will not accept indexing and will miss data in scripting processes.
#define MAX_QUEUE_PLAYERS	127

// Queue Name Length
#define	QUEUE_NAME_LENGTH	30

#ifndef QUEUE_COLOR
	#define	QUEUE_COLOR	0x89cff0
#endif

enum queue_join_notify {
	QUEUE_J_NO,
	QUEUE_J_OK
};

enum queue_leave_notify {
	QUEUE_L_NONE,
	QUEUE_L_TOWN,
	QUEUE_L_LEFT,
	QUEUE_L_KICK,
	QUEUE_L_QUIT,
	QUEUE_L_REQ,
	QUEUE_L_WAIT,
	QUEUE_L_CLEAR,
	QUEUE_L_EXPIRED,
	QUEUE_L_MAX
};

struct queue_players {
	int position;
	struct map_session_data *sd;
	struct queue_players *next;
};

struct queue_data {
	unsigned int queue_id;
	unsigned char count;
	int min_level, max_level;
	struct queue_players *first, *last;
	char name[QUEUE_NAME_LENGTH];
	char join_event[EVENT_NAME_LENGTH];
};

/* [CreativeSD]: Queue System */
struct queue_data *queue_search(int queue_id);
int queue_create(int queue_id, const char *name, int min_level, int max_level, const char *event);
int queue_delete(int queue_id);
int queue_join(struct map_session_data *sd, int queue_id, enum queue_join_notify flag);
int queue_leave(struct map_session_data *sd, enum queue_leave_notify flag);
int queue_clean(struct queue_data *queue, int delay, enum queue_leave_notify flag);
int queue_atcommand_list(struct map_session_data *sd);
int queue_check_dual(struct map_session_data* sd);
void queue_join_notify(int queue_id, struct map_session_data *sd);
void queue_leave_notify(int queue_id, struct map_session_data *sd, enum queue_leave_notify flag);
const char* queue_delay(struct map_session_data *sd, int delay);

TIMER_FUNC(queue_check_idle);

#define queue_check_town(m) (map_getmapflag(m,MF_TOWN))

void do_init_queue(void);
void do_final_queue(void);

#endif /* _QUEUE_HPP_ */
