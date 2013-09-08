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
 *     Function SignalC.c
 *
 *     Date: 2013-02-23
 * 
 * 
 *     Description:
 * 
 *
 *     Input parameters:
 * 
 *
 *     Return value:
 * 
 * 
 *
 *     Modifications:
 *     Date		Version		Patch number		CLA 
 *
 *
 *     Description
 * 
 */



#include "libm3l.h"
// #include "Local_Header.h"
#include "SignalC.h"

void catch_int(int sig_num)
{
    sigset_t mask_set;     /* used to set a signal masking set. */
    sigset_t old_set;        /* used to store the old mask set.   */
/*
 * re-set the signal handler again to catch_int, for next time 
 */
    signal(SIGINT, catch_int);
/*
 * mask any further signals while you are inside the handler. 
 */
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

//     DelSEM(id); 
//     DelSMH_Int(shm_buff, shmid);
//     DelSMH_Int(shm_n, shmid1);
//     if(m3l_Umount(&TmpACKN) != 1)
// 	Perror("m3l_Umount");
    exit(0);
    
    /* restore the old signal mask */
    sigprocmask(SIG_SETMASK, &old_set, NULL);
}


void catch_usr(int sig_num)
{
    sigset_t mask_set;     /* used to set a signal masking set. */
    sigset_t old_set;        /* used to store the old mask set.   */
/*
 * re-set the signal handler again to catch_int, for next time 
 */
	printf(" End of iterational process \n");

    signal(SIGUSR1, catch_usr);
/*
 * mask any further signals while you are inside the handler. 
 */
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

//     DelSEM(id); 
//     DelSMH_Int(shm_buff, shmid);
//     DelSMH_Int(shm_n, shmid1);
//     
//     if(m3l_Umount(&TmpACKN) != 1)
// 	Perror("m3l_Umount");
    exit(0);
    
    /* restore the old signal mask */
    sigprocmask(SIG_SETMASK, &old_set, NULL);
}