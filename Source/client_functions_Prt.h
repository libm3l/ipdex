
#ifndef  __FCE_CLIENT_FUNCTIONS_PRT_H__
#define  __FCE_CLIENT_FUNCTIONS_PRT_H__

typedef struct client_fce_struct{
	lmchar_t *data_name;
	lmchar_t SR_MODE;
	lmint_t mode;
	lmchar_t Caller;
}client_fce_struct_t;

// lmint_t client_sender(void *, const lmchar_t *, lmint_t, lmchar_t *, lmchar_t , lmint_t, lmchar_t, opts_t *, opts_t *);
// void *client_recevier(const lmchar_t *, lmint_t , lmchar_t *, lmchar_t , lmint_t, lmchar_t, opts_t *, opts_t *);
lmint_t client_sender(void *, const lmchar_t *, lmint_t, client_fce_struct_t *, opts_t *, opts_t *);
void *client_recevier(const lmchar_t *, lmint_t , client_fce_struct_t *, opts_t *, opts_t *);
#endif
