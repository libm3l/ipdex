
#ifndef  __FCE_CLIENT_FUNCTIONS_PRT_H__
#define  __FCE_CLIENT_FUNCTIONS_PRT_H__
#include "Server_Header.h"

lmint_t client_sender(void *, const lmchar_t *, lmint_t, client_fce_struct_t *, opts_t *, opts_t *);
client_recevier_struct_t *client_recevier(const lmchar_t *, lmint_t , client_fce_struct_t *, opts_t *, opts_t *);

#endif
