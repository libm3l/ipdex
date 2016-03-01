
/*
 *     Copyright (C) 2012  Adam Jirasek
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *     
 *     contact: libm3l@gmail.com
 * 
 */




/*
 *     Header file Data_Thread_Functions.h
 *
 *     Date: 2013-02-23
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */




#ifndef  __DATA_THREAD_FUNCTIONS_H__
#define  __DATA_THREAD_FUNCTIONS_H__


#ifndef CPP_C
#ifdef __cplusplus 
#define CPP_C "C"
#else
#define CPP_C
#endif
#endif

extern CPP_C  lmsize_t Data_Thread_Case_0(data_thread_args_t *, SR_thread_str_t *, lmsize_t , lmsize_t *,
	lmint_t *, lmint_t *, lmsize_t *, lmsize_t);
extern CPP_C  lmint_t Data_Thread_Case_200(data_thread_args_t *, SR_thread_str_t *, lmsize_t , sem_t *);

#endif
