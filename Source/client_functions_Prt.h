
#ifndef  __FCE_CLIENT_FUNCTIONS_PRT_H__
#define  __FCE_CLIENT_FUNCTIONS_PRT_H__

typedef struct client_fce_struct{
	lmchar_t *data_name;
	lmchar_t SR_MODE;
	lmint_t mode;
}client_fce_struct_t;

typedef struct client_receiver_struct{
	node_t *data;
	lmint_t sockfd;
}client_recevier_struct_t;

lmint_t client_sender(void *, const lmchar_t *, lmint_t, client_fce_struct_t *, opts_t *, opts_t *);
client_recevier_struct_t *client_recevier(const lmchar_t *, lmint_t , client_fce_struct_t *, opts_t *, opts_t *);

#endif
