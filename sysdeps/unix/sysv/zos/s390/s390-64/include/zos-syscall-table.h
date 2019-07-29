/* (C) 2018 Rocket Software */
#ifndef _ASM_ZOS_BPX_OFFSETS_H
#define _ASM_ZOS_BPX_OFFSETS_H 1
#include <asm/unistd_64.h>

/* Fake syscall numbers for syscalls that don't exist on s390x linux,
   but do on z/OS.  */
#define __NR_msgctl 500
#define __NR_msgget 501
#define __NR_msgrcv 502
#define __NR_msgsnd 503
#define __NR_semctl 504
#define __NR_semget 505
#define __NR_semop 506
#define __NR_shmat 507
#define __NR_shmctl 508
#define __NR_shmdt 509
#define __NR_shmget 510
#define __NR_accept 511


/* BPX assembler callable services */

/* All services
 * High order bits are all 0.
 */

#define  __BPX_off_resource                    8     /*  BPX1RMG  */
#define  __BPX_off_isatty                      12    /*  BPX1ITY  */
#define  __BPX_off_ttyname                     16    /*  BPX1TYN  */
#define  __BPX_off_tcflush                     20    /*  BPX1TFH  */
#define  __BPX_off_tcdrain                     24    /*  BPX1TDR  */
#define  __BPX_off_tcflow                      28    /*  BPX1TFW  */
#define  __BPX_off_tcgetattr                   32    /*  BPX1TGA  */
#define  __BPX_off_tcgetpgrp                   36    /*  BPX1TGP  */
#define  __BPX_off_tcsetattr                   40    /*  BPX1TSA  */
#define  __BPX_off_tcsendbreak                 44    /*  BPX1TSB  */
#define  __BPX_off_tcsetpgrp                   48    /*  BPX1TSP  */
#define  __BPX_off_access                      52    /*  BPX1ACC  */
#define  __BPX_off_chdir                       56    /*  BPX1CHD  */
#define  __BPX_off_chmod                       60    /*  BPX1CHM  */
#define  __BPX_off_chown                       64    /*  BPX1CHO  */
#define  __BPX_off_closedir                    68    /*  BPX1CLD  */
#define  __BPX_off_close                       72    /*  BPX1CLO  */
#define  __BPX_off_w_getmntent                 76    /*  BPX1GMN  */
#define  __BPX_off_w_statvfs                    80    /*  BPX1STF  */
#define  __BPX_off_chaudit                     84    /*  BPX1CHA  */
#define  __BPX_off_fchmod                      88    /*  BPX1FCM  */
#define  __BPX_off_fchown                      92    /*  BPX1FCO  */
#define  __BPX_off_fcntl                       96    /*  BPX1FCT  */
#define  __BPX_off_fpathconf                   100   /*  BPX1FPC  */
#define  __BPX_off_fstat                       104   /*  BPX1FST  */
#define  __BPX_off_fsync                       108   /*  BPX1FSY  */
#define  __BPX_off_ftruncate                   112   /*  BPX1FTR  */
#define  __BPX_off_getcwd                      116   /*  BPX1GCW  */
#define  __BPX_off_w_ioctl                     120   /*  BPX1IOC  */
#define  __BPX_off_link                        124   /*  BPX1LNK  */
#define  __BPX_off_lseek                       128   /*  BPX1LSK  */
#define  __BPX_off_lstat                       132   /*  BPX1LST  */
#define  __BPX_off_mkdir                       136   /*  BPX1MKD  */
#define  __BPX_off_fchaudit                    140   /*  BPX1FCA  */
#define  __BPX_off_mknod                       144   /*  BPX1MKN  */
#define  __BPX_off_mount                       148   /*  BPX1MNT  */
#define  __BPX_off_opendir                     152   /*  BPX1OPD  */
#define  __BPX_off_open                        156   /*  BPX1OPN  */
#define  __BPX_off_pathconf                    160   /*  BPX1PCF  */
#define  __BPX_off_pipe                        164   /*  BPX1PIP  */
#define  __BPX_off_readdir                     168   /*  BPX1RDD  */
#define  __BPX_off_readlink                    172   /*  BPX1RDL  */
#define  __BPX_off_read                        176   /*  BPX1RED  */
#define  __BPX_off_rename                      180   /*  BPX1REN  */
#define  __BPX_off_rewinddir                   184   /*  BPX1RWD  */
#define  __BPX_off_rmdir                       188   /*  BPX1RMD  */
#define  __BPX_off_stat                        192   /*  BPX1STA  */
#define  __BPX_off_symlink                     196   /*  BPX1SYM  */
#define  __BPX_off_extlink_np                  200   /*  BPX1EXT  */
#define  __BPX_off_umask                       204   /*  BPX1UMK  */
#define  __BPX_off_umount                      208   /*  BPX1UMT  */
#define  __BPX_off_unlink                      212   /*  BPX1UNL  */
#define  __BPX_off_utime                       216   /*  BPX1UTI  */
#define  __BPX_off_write                       220   /*  BPX1WRT  */
#define  __BPX_off_alarm                       224   /*  BPX1ALR  */
#define  __BPX_off_exec                        228   /*  BPX1EXC  */
#define  __BPX_off__exit                       232   /*  BPX1EXI  */
#define  __BPX_off_execmvs                     236   /*  BPX1EXM  */
#define  __BPX_off_fork                        240   /*  BPX1FRK  */
#define  __BPX_off_getegid                     244   /*  BPX1GEG  */
#define  __BPX_off_geteuid                     248   /*  BPX1GEU  */
#define  __BPX_off_getgrgid                    252   /*  BPX1GGI  */
#define  __BPX_off_getgrnam                    256   /*  BPX1GGN  */
#define  __BPX_off_getgroups                   260   /*  BPX1GGR  */
#define  __BPX_off_getgid                      264   /*  BPX1GID  */
#define  __BPX_off_getlogin                    268   /*  BPX1GLG  */
#define  __BPX_off_getpgrp                     272   /*  BPX1GPG  */
#define  __BPX_off_getpid                      276   /*  BPX1GPI  */
#define  __BPX_off_getpwnam                    280   /*  BPX1GPN  */
#define  __BPX_off_getppid                     284   /*  BPX1GPP  */
#define  __BPX_off_getpwuid                    288   /*  BPX1GPU  */
#define  __BPX_off_getugrps                    292   /*  BPX1GUG  */
#define  __BPX_off_getuid                      296   /*  BPX1GUI  */
#define  __BPX_off_setdubdefault               300   /*  BPX1SDD  */
#define  __BPX_off_kill                        308   /*  BPX1KIL  */
#define  __BPX_off_mvssigsetup                 312   /*  BPX1MSS  */
#define  __BPX_off_pause                       316   /*  BPX1PAS  */
#define  __BPX_off_ptrace                      320   /*  BPX1PTR  */
#define  __BPX_off_sigaction                   324   /*  BPX1SIA  */
#define  __BPX_off_setgid                      328   /*  BPX1SGI  */
#define  __BPX_off_mvsunsigsetup               336   /*  BPX1MSD  */
#define  __BPX_off_sigpending                  340   /*  BPX1SIP  */
#define  __BPX_off_sleep                       344   /*  BPX1SLP  */
#define  __BPX_off_setpgid                     348   /*  BPX1SPG  */
#define  __BPX_off_sigprocmask                 352   /*  BPX1SPM  */
#define  __BPX_off_setsid                      356   /*  BPX1SSI  */
#define  __BPX_off_sigsuspend                  360   /*  BPX1SSU  */
#define  __BPX_off_setuid                      364   /*  BPX1SUI  */
#define  __BPX_off_sysconf                     368   /*  BPX1SYC  */
#define  __BPX_off_times                       372   /*  BPX1TIM  */
#define  __BPX_off_uname                       376   /*  BPX1UNA  */
#define  __BPX_off_wait                        380   /*  BPX1WAT  */
#define  __BPX_off_quiesce                     388   /*  BPX1QSE  */
#define  __BPX_off_unquiesce                   392   /*  BPX1UQS  */
#define  __BPX_off_MvsIptAffinity              396   /*  BPX1IPT  */
#define  __BPX_off_mvsprocclp                  408   /*  BPX1MPC  */
#define  __BPX_off_pthread_quiesc              412   /*  BPX1PTQ  */
#define  __BPX_off_sigputback                  416   /*  BPX1SPB  */
#define  __BPX_off_seteuid                     420   /*  BPX1SEU  */
#define  __BPX_off_setegid                     424   /*  BPX1SEG  */
#define  __BPX_off_w_getpsent                  428   /*  BPX1GPS  */
#define  __BPX_off_pthread_create              432   /*  BPX1PTC  */
#define  __BPX_off_pthread_xandg               436   /*  BPX1PTX  */
#define  __BPX_off_pthread_join                440   /*  BPX1PTJ  */
#define  __BPX_off_pthread_detach              444   /*  BPX1PTD  */
#define  __BPX_off_pthread_cancel              448   /*  BPX1PTB  */
#define  __BPX_off_pthread_self                452   /*  BPX1PTS  */
#define  __BPX_off_pthread_setintr             460   /*  BPX1PSI  */
#define  __BPX_off_pthread_kill                464   /*  BPX1PTK  */
#define  __BPX_off_sigwait                     468   /*  BPX1SWT  */
#define  __BPX_off_pthread_setintrtype         472   /*  BPX1PST  */
#define  __BPX_off_pthread_testintr            476   /*  BPX1PTI  */
#define  __BPX_off_cond_cancel                 480   /*  BPX1CCA  */
#define  __BPX_off_cond_post                   484   /*  BPX1CPO  */
#define  __BPX_off_cond_setup                  488   /*  BPX1CSE  */
#define  __BPX_off_cond_timed_wait             492   /*  BPX1CTW  */
#define  __BPX_off_cond_wait                   496   /*  BPX1CWA  */
#define  __BPX_off_chattr                      500   /*  BPX1CHR  */
#define  __BPX_off_fchattr                     504   /*  BPX1FCR  */
#define  __BPX_off_accept                      508   /*  BPX1ACP  */
#define  __BPX_off_bind                        512   /*  BPX1BND  */
#define  __BPX_off_connect                     516   /*  BPX1CON  */
#define  __BPX_off_gethostid                   520   /*  BPX1HST  */
#define  __BPX_off_getpeername                 524   /*  BPX1GNM  */
#define  __BPX_off_getsockopt                  528   /*  BPX1OPT  */
#define  __BPX_off_listen                      532   /*  BPX1LSN  */
#define  __BPX_off_readv                       536   /*  BPX1RDV  */
#define  __BPX_off_recv                        540   /*  BPX1RCV  */
#define  __BPX_off_recvfrom                    544   /*  BPX1RFM  */
#define  __BPX_off_recvmsg                     548   /*  BPX1RMS  */
#define  __BPX_off_select                      552   /*  BPX1SEL  */
#define  __BPX_off_send                        556   /*  BPX1SND  */
#define  __BPX_off_sendmsg                     560   /*  BPX1SMS  */
#define  __BPX_off_sendto                      564   /*  BPX1STO  */
#define  __BPX_off_setpeer                     568   /*  BPX1SPR  */
#define  __BPX_off_shutdown                    572   /*  BPX1SHT  */
#define  __BPX_off_socket_pair                 576   /*  BPX1SOC  */
#define  __BPX_off_writev                      580   /*  BPX1WRV  */
#define  __BPX_off_v_reg                       584   /*  BPX1VRG  */
#define  __BPX_off_v_rpn                       588   /*  BPX1VRP  */
#define  __BPX_off_bind2addrsel                592   /*  BPX1BAS  */
#define  __BPX_off_v_get                       596   /*  BPX1VGT  */
#define  __BPX_off_v_rel                       600   /*  BPX1VRL  */
#define  __BPX_off_v_lookup                    604   /*  BPX1VLK  */
#define  __BPX_off_v_rdwr                      608   /*  BPX1VRW  */
#define  __BPX_off_v_readdir                   612   /*  BPX1VRD  */
#define  __BPX_off_v_readlink                  616   /*  BPX1VRA  */
#define  __BPX_off_v_create                    620   /*  BPX1VCR  */
#define  __BPX_off_v_mkdir                     624   /*  BPX1VMD  */
#define  __BPX_off_v_symlink                   628   /*  BPX1VSY  */
#define  __BPX_off_v_getattr                   632   /*  BPX1VGA  */
#define  __BPX_off_v_settatr                   636   /*  BPX1VSA  */
#define  __BPX_off_v_link                      640   /*  BPX1VLN  */
#define  __BPX_off_v_rmdir                     644   /*  BPX1VRE  */
#define  __BPX_off_v_remove                    648   /*  BPX1VRM  */
#define  __BPX_off_v_rename                    652   /*  BPX1VRN  */
#define  __BPX_off_v_fstatfs                   656   /*  BPX1VSF  */
#define  __BPX_off_v_lockctl                   660   /*  BPX1VLO  */
#define  __BPX_off_attach_exec                 664   /*  BPX1ATX  */
#define  __BPX_off_attach_execmvs              668   /*  BPX1ATM  */
#define  __BPX_off_MVSpauseInit                680   /*  BPX1MPI  */
#define  __BPX_off_set_thread_limits           684   /*  BPX1STL  */
#define  __BPX_off_MVSpause                    688   /*  BPX1MP   */
#define  __BPX_off_msgctl                      692   /*  BPX1QCT  */
#define  __BPX_off_msgget                      696   /*  BPX1QGT  */
#define  __BPX_off_msgrcv                      700   /*  BPX1QRC  */
#define  __BPX_off_msgsnd                      704   /*  BPX1QSN  */
#define  __BPX_off_semctl                      708   /*  BPX1SCT  */
#define  __BPX_off_semget                      712   /*  BPX1SGT  */
#define  __BPX_off_semop                       716   /*  BPX1SOP  */
#define  __BPX_off_shmat                       720   /*  BPX1MAT  */
#define  __BPX_off_shmctl                      724   /*  BPX1MCT  */
#define  __BPX_off_shmdt                       728   /*  BPX1MDT  */
#define  __BPX_off_shmget                      732   /*  BPX1MGT  */
#define  __BPX_off_w_getipc                    736   /*  BPX1GET  */
#define  __BPX_off_setpriority                 740   /*  BPX1SPY  */
#define  __BPX_off_getpriority                 744   /*  BPX1GPY  */
#define  __BPX_off_nice                        748   /*  BPX1NIC  */
#define  __BPX_off_getitimer                   752   /*  BPX1GTR  */
#define  __BPX_off_setitimer                   756   /*  BPX1STR  */
#define  __BPX_off_spawn                       760   /*  BPX1SPN  */
#define  __BPX_off_chpriority                  764   /*  BPX1CHP  */
#define  __BPX_off_pfsctl                      768   /*  BPX1PCT  */
#define  __BPX_off_getgrent                    772   /*  BPX1GGE  */
#define  __BPX_off_getpwent                    776   /*  BPX1GPE  */
#define  __BPX_off_setgrent                    780   /*  BPX1SGE  */
#define  __BPX_off_setpwent                    784   /*  BPX1SPE  */
#define  __BPX_off_password                    788   /*  BPX1PWD  */
#define  __BPX_off_setgroups                   792   /*  BPX1SGR  */
#define  __BPX_off_mmap                        796   /*  BPX1MMP  */
#define  __BPX_off_mprotect                    800   /*  BPX1MPR  */
#define  __BPX_off_msync                       804   /*  BPX1MSY  */
#define  __BPX_off_munmap                      808   /*  BPX1MUN  */
#define  __BPX_off_setrlimit                   816   /*  BPX1SRL  */
#define  __BPX_off_getrlimit                   820   /*  BPX1GRL  */
#define  __BPX_off_getrusage                   824   /*  BPX1GRU  */
#define  __BPX_off_truncate                    828   /*  BPX1TRU  */
#define  __BPX_off_lchown                      832   /*  BPX1LCO  */
#define  __BPX_off_waitid_wait3                840   /*  BPX1WTE  */
#define  __BPX_off_statvfs                     844   /*  BPX1STV  */
#define  __BPX_off_fstatvfs                    848   /*  BPX1FTV  */
#define  __BPX_off_fchdir                      852   /*  BPX1FCD  */
#define  __BPX_off_readdir2                    856   /*  BPX1RD2  */
#define  __BPX_off_getpgid                     860   /*  BPX1GEP  */
#define  __BPX_off_getsid                      864   /*  BPX1GES  */
#define  __BPX_off_sync                        868   /*  BPX1SYN  */
#define  __BPX_off_chroot                      872   /*  BPX1CRT  */
#define  __BPX_off_v_export                    876   /*  BPX1VEX  */
#define  __BPX_off_loadhfs                     880   /*  BPX1LOD  */
#define  __BPX_off_realpath                    884   /*  BPX1RPH  */
#define  __BPX_off_deletehfs                   888   /*  BPX1DEL  */
#define  __BPX_off_setreuid                    892   /*  BPX1SRU  */
#define  __BPX_off_setregid                    896   /*  BPX1SRG  */
#define  __BPX_off_tcgetcp                     900   /*  BPX1TGC  */
#define  __BPX_off_tcsetcp                     904   /*  BPX1TSC  */
#define  __BPX_off_tcsettables                 908   /*  BPX1TST  */
#define  __BPX_off_tcgetsid                    912   /*  BPX1TGS  */
#define  __BPX_off_grantpt                     916   /*  BPX1GPT  */
#define  __BPX_off_unlockpt                    920   /*  BPX1UPT  */
#define  __BPX_off_ttyname2                    924   /*  BPX2TYN  */
#define  __BPX_off_isatty2                     928   /*  BPX2ITY  */
#define  __BPX_off_poll                        932   /*  BPX1POL  */
#define  __BPX_off_getwd                       936   /*  BPX1GWD  */
#define  __BPX_off_read_extlink                940   /*  BPX1RDX  */
#define  __BPX_off_v_access                    944   /*  BPX1VAC  */
#define  __BPX_off_querydub                    948   /*  BPX1QDB  */
#define  __BPX_off_oe_env_np                   960   /*  BPX1ENV  */
#define  __BPX_off_pthread_security_np         964   /*  BPX1TLS  */
#define  __BPX_off_auth_check_rsrc_np          972   /*  BPX1ACK  */
#define  __BPX_off_recvmsg2                    976   /*  BPX2RMS  */
#define  __BPX_off_sendmsg2                    980   /*  BPX2SMS  */
#define  __BPX_off_w_pioctl                    984   /*  BPX1PIO  */
#define  __BPX_off_asyncio                     988   /*  BPX1AIO  */
#define  __BPX_off_server_init                 1004  /*  BPX1SIN  */
#define  __BPX_off_server_pwu                  1008  /*  BPX1SPW  */
#define  __BPX_off_console_np                  1012  /*  BPX1CCS  */
#define  __BPX_off_pthread_tag_np              1016  /*  BPX1PTT  */
#define  __BPX_off_getclientid                 1024  /*  BPX1GCL  */
#define  __BPX_off_givesocket                  1028  /*  BPX1GIV  */
#define  __BPX_off_takesocket                  1032  /*  BPX1TAK  */
#define  __BPX_off___smf_record                1036  /*  BPX1SMF  */
#define  __BPX_off_v_pathconf                  1040  /*  BPX1VPC  */
#define  __BPX_off___security                  1044  /*  BPX1SEC  */
#define  __BPX_off___wlm                       1048  /*  BPX1WLM  */
#define  __BPX_off_openstat                    1052  /*  BPX2OPN  */
#define  __BPX_off___getthent                  1056  /*  BPX1GTH  */
#define  __BPX_off_accept_and_recv             1060  /*  BPX1ANR  */
#define  __BPX_off_send_file                   1064  /*  BPX1SF   */
#define  __BPX_off___shm_lock                  1068  /*  BPX1SLK  */
#define  __BPX_off___pid_affinity              1072  /*  BPX1PAF  */
#define  __BPX_off_set_timer_event             1076  /*  BPX1STE  */
#define  __BPX_off_srx_np                      1080  /*  BPX1SRX  */
#define  __BPX_off___Sigactionset              1084  /*  BPX1SA2  */
#define  __BPX_off_aio_suspend                 1088  /*  BPX1ASP  */
#define  __BPX_off_sigtimedwait                1096  /*  BPX1STW  */
#define  __BPX_off___osenv                     1100  /*  BPX1OSE  */
#define  __BPX_off_sigqueue                    1104  /*  BPX1SGQ  */
#define  __BPX_off_Pread                       1108  /*  BPX1RW   */
#define  __BPX_off___smc                       1112  /*  BPX1SMC  */
#define  __BPX_off_sw_signaldelv               1124  /*  BPX1DSD  */
#define  __BPX_off___mount                     1128  /*  BPX2MNT  */
#define  __BPX_off___cpl                       1132  /*  BPX1CPL  */
#define  __BPX_off___map_init                  1136  /*  BPX1MMI  */
#define  __BPX_off___map_service               1140  /*  BPX1MMS  */
#define  __BPX_off_server_thread_query         1144  /*  BPX1STQ  */
#define  __BPX_off_MvsThreadAffinity           1148  /*  BPX1TAF  */
#define  __BPX_off_pthread_quiesce_and_get_np  1152  /*  BPX1PQG  */
#define  __BPX_off_gethostbyname               1156  /*  BPX1GHN  */
#define  __BPX_off_gethostbyaddr               1160  /*  BPX1GHA  */
#define  __BPX_off_GetAddrInfo                 1164  /*  BPX1GAI  */
#define  __BPX_off_FreeAddrInfo                1168  /*  BPX1FAI  */
#define  __BPX_off_GetNameInfo                 1172  /*  BPX1GNI  */
#define  __BPX_off___poe                       1176  /*  BPX1POE  */
#define  __BPX_off_lchattr                     1180  /*  BPX1LCR  */
#define  __BPX_off_v_open                      1184  /*  BPX1VOP  */
#define  __BPX_off_v_close                     1188  /*  BPX1VCL  */

/* Mapping all Linux s390 syscall names to their nearest z/OS
   equivalents. Many of them might not be used by glibc.
   Anything defined to 0 means that the corresponding linux syscall
   has no sufficiently similar BPX service, and may or may not need
   to be implemented from scratch, if possible. */

#define __BPX_exit    __BPX_off__exit
#define __BPX_fork    __BPX_off_fork
#define __BPX_read    __BPX_off_read
#define __BPX_write   __BPX_off_write
#define __BPX_open    __BPX_off_open
#define __BPX_close   __BPX_off_close
#define __BPX_restart_syscall 0 /* no glibc wrapper expected */
#define __BPX_creat   0 /* no direct equivalent */
#define __BPX_link    __BPX_off_link
#define __BPX_unlink  __BPX_off_unlink
#define __BPX_execve  __BPX_off_exec /* TODO: different semantics/behavior */
#define __BPX_chdir   __BPX_off_chdir
#define __BPX_mknod   __BPX_off_mknod
#define __BPX_chmod   __BPX_off_chmod
#define __BPX_lseek   __BPX_off_lseek
#define __BPX_getpid  __BPX_off_getpid
#define __BPX_mount   __BPX_off_mount /* or __mount? */
#define __BPX_umount  0 /* can be implemented with umount2 */
#define __BPX_ptrace  __BPX_off_ptrace
#define __BPX_alarm   __BPX_off_alarm
#define __BPX_pause   __BPX_off_pause
#define __BPX_utime   __BPX_off_utime
#define __BPX_access  __BPX_off_access
#define __BPX_nice    __BPX_off_nice
#define __BPX_sync    __BPX_off_sync
#define __BPX_kill    __BPX_off_kill
#define __BPX_rename  __BPX_off_rename
#define __BPX_mkdir   __BPX_off_mkdir
#define __BPX_rmdir   __BPX_off_rmdir
#define __BPX_dup     0 /* most functionality is in fcntl */
#define __BPX_pipe    __BPX_off_pipe
#define __BPX_times   __BPX_off_times
#define __BPX_brk     0 /* TODO: this is problematic */
#define __BPX_signal  0 /* TODO: might be able to emulate with sigaction */
#define __BPX_acct    0 /* process accounting is not supported */
#define __BPX_umount2 __BPX_off_umount /* see umount above */
#define __BPX_ioctl   __BPX_off_w_ioctl /* what does the w mean? */
#define __BPX_fcntl   __BPX_off_fcntl
#define __BPX_setpgid __BPX_off_setpgid
#define __BPX_umask   __BPX_off_umask
#define __BPX_chroot  __BPX_off_chroot
#define __BPX_ustat   __BPX_off_w_getmntent /* not an exact mapping */
#define __BPX_dup2    0 /* implement with fcntl */
#define __BPX_getppid __BPX_off_getppid
#define __BPX_getpgrp __BPX_off_getpgrp
#define __BPX_setsid  __BPX_off_setsid
#define __BPX_sigaction   __BPX_off_sigaction /* and __Sigactionset */
#define __BPX_sigsuspend  __BPX_off_sigsuspend
#define __BPX_sigpending  __BPX_off_sigpending
#define __BPX_sethostname 0 /* TODO: */
#define __BPX_setrlimit   __BPX_off_setrlimit
#define __BPX_getrusage   __BPX_off_getrusage
#define __BPX_gettimeofday 0 /* TODO: this is problematic */
#define __BPX_settimeofday 0 /* see gettimeofday */
#define __BPX_symlink     __BPX_off_symlink
#define __BPX_readlink    __BPX_off_readlink
#define __BPX_uselib      0 /* no glibc wrapper expected */
#define __BPX_swapon      0 /* TODO: how does z/OS manage swap? */
#define __BPX_reboot      0 /* we should leave this as a stub for now */
#define __BPX_readdir   __BPX_off_readdir /* no glibc wrapper expected, see readdir2 */
#define __BPX_mmap      __BPX_off_mmap
#define __BPX_munmap    __BPX_off_munmap
#define __BPX_truncate  __BPX_off_truncate
#define __BPX_ftruncate __BPX_off_ftruncate
#define __BPX_fchmod    __BPX_off_fchmod
#define __BPX_getpriority __BPX_off_getpriority
#define __BPX_setpriority __BPX_off_setpriority
#define __BPX_statfs      0 /* can implement with statvfs */
#define __BPX_fstatfs     0 /* can implement with fstatvfs */
#define __BPX_socketcall  0 /* can implement with the other socket services */
#define __BPX_syslog      0 /* linux-style kernel logging is not supported */
#define __BPX_setitimer   __BPX_off_setitimer
#define __BPX_getitimer   __BPX_off_getitimer
#define __BPX_stat  __BPX_off_stat
#define __BPX_lstat __BPX_off_lstat
#define __BPX_fstat __BPX_off_fstat
#define __BPX_lookup_dcookie 0 /* I can't find a corresponding service */
#define __BPX_vhangup 0 /* try to implement with other termios */
#define __BPX_idle    0 /* doesn't exist in linux anymore */
#define __BPX_wait4   0 /* might be able to implement with waitid */
#define __BPX_swapoff 0 /* see swapon */
#define __BPX_sysinfo 0 /* no straightforward equivalent, maybe a non-BPX service? */
#define __BPX_ipc     0 /* could be implemented with other sysvipcs */
#define __BPX_fsync   __BPX_off_fsync
#define __BPX_sigreturn 0 /* TODO: without this, implementing signal handlers is hard */
#define __BPX_clone    0 /* cannot be implemented as is in linux without kernel support */
#define __BPX_setdomainname 0 /* Maybe other services could implement this */
#define __BPX_uname    __BPX_off_uname
#define __BPX_adjtimex 0 /* might not be implementable on z/OS */
#define __BPX_mprotect __BPX_off_mprotect
#define __BPX_sigprocmask __BPX_off_sigprocmask
#define __BPX_create_module   0 /* no glibc wrapper expected */
#define __BPX_init_module     0 /* no glibc wrapper expected */
#define __BPX_delete_module   0 /* no glibc wrapper expected */
#define __BPX_get_kernel_syms 0 /* no glibc wrapper expected */
#define __BPX_quotactl 0 /* maybe implementable with non-BPX services */
#define __BPX_getpgid  __BPX_off_getpgid
#define __BPX_fchdir   __BPX_off_fchdir
#define __BPX_bdflush  0 /* deprecated on linux */
#define __BPX_sysfs    0 /* implementable with other calls */
#define __BPX_personality 0 /* no straightforward equivalent */
#define __BPX_afs_syscall 0 /* stub pseudo-call */
#define __BPX_getdents 0 /* no glibc wrapper expected */
#define __BPX_select   __BPX_off_select
#define __BPX_flock    0 /* TODO: implement this, somehow */
#define __BPX_msync    __BPX_off_msync
#define __BPX_readv    __BPX_off_readv
#define __BPX_writev   __BPX_off_writev
#define __BPX_getsid   __BPX_off_getsid
#define __BPX_fdatasync  0 /* can be implemented with fsync */
#define __BPX__sysctl    0 /* no glibc wrapper expected */
#define __BPX_mlock      0 /* is arbitrary memory locking possible? */
#define __BPX_munlock    0 /* see mlock */
#define __BPX_mlockall   0 /* see mlock */
#define __BPX_munlockall 0 /* see mlock */
#define __BPX_sched_setparam         0 /* bpx scheduler interface? */
#define __BPX_sched_getparam         0 /* bpx scheduler interface? */
#define __BPX_sched_setscheduler     0 /* bpx scheduler interface? */
#define __BPX_sched_getscheduler     0 /* bpx scheduler interface? */
#define __BPX_sched_yield            0 /* bpx scheduler interface? */
#define __BPX_sched_get_priority_max 0 /* bpx scheduler interface? */
#define __BPX_sched_get_priority_min 0 /* bpx scheduler interface? */
#define __BPX_sched_rr_get_interval  0 /* bpx scheduler interface? */
#define __BPX_nanosleep  0 /* implement with cond_timed_wait */
#define __BPX_mremap     0 /* difficult to implement */
#define __BPX_query_module 0 /* no glibc wrapper expected */
#define __BPX_poll __BPX_off_poll
#define __BPX_nfsservctl 0 /* removed in linux */
#define __BPX_prctl 0 /* could be implemented with other syscalls */
#define __BPX_rt_sigreturn    0 /* no rt signal support */
#define __BPX_rt_sigaction    0 /* no rt signal support */
#define __BPX_rt_sigprocmask  0 /* no rt signal support */
#define __BPX_rt_sigpending   0 /* no rt signal support */
#define __BPX_rt_sigtimedwait 0 /* no rt signal support */
#define __BPX_rt_sigqueueinfo 0 /* no rt signal support */
#define __BPX_rt_sigsuspend   0 /* no rt signal support */
#define __BPX_pread64   __BPX_off_Pread /* pread/pwrite share a call */
#define __BPX_pwrite64  __BPX_off_Pread /* pread/pwrite share a call */
#define __BPX_getcwd    __BPX_off_getcwd
#define __BPX_capget    0 /* implement with cap_get_proc if needed */
#define __BPX_capset    0 /* implement with cap_set_proc if needed */
#define __BPX_sigaltstack 0 /* how could this be implemented? */
#define __BPX_sendfile  0 /* emulate with read()+write() */
#define __BPX_getpmsg   0 /* stub pseudo-call */
#define __BPX_putpmsg   0 /* stub pseudo-call */
#define __BPX_vfork     0 /* implement with fork */
#define __BPX_getrlimit __BPX_off_getrlimit
#define __BPX_lchown    __BPX_off_lchown
#define __BPX_getuid    __BPX_off_getuid
#define __BPX_getgid    __BPX_off_getgid
#define __BPX_geteuid   __BPX_off_geteuid
#define __BPX_getegid   __BPX_off_getegid
#define __BPX_setreuid  __BPX_off_setreuid
#define __BPX_setregid  __BPX_off_setregid
#define __BPX_getgroups __BPX_off_getgroups
#define __BPX_setgroups __BPX_off_setgroups
#define __BPX_fchown    __BPX_off_fchown
#define __BPX_setresuid 0 /* any calls that implement the saved part? */
#define __BPX_getresuid 0 /* any calls that implement the saved part? */
#define __BPX_setresgid 0 /* any calls that implement the saved part? */
#define __BPX_getresgid 0 /* any calls that implement the saved part? */
#define __BPX_chown     __BPX_off_chown
#define __BPX_setuid    __BPX_off_setuid
#define __BPX_setgid    __BPX_off_setgid
#define __BPX_setfsuid  0 /* any calls that implement the saved part? */
#define __BPX_setfsgid  0 /* any calls that implement the saved part? */
#define __BPX_pivot_root 0 /* no glibc wrapper required */
#define __BPX_mincore   0 /* how to check for resident pages? */
#define __BPX_madvise   0 /* no support for kernel hinting */
#define __BPX_getdents64 0 /* no glibc wrapper required */
#define __BPX_readahead 0 /* read file into cache */
#define __BPX_setxattr     0 /* does z/OS have xattr support? */
#define __BPX_lsetxattr    0 /* does z/OS have xattr support? */
#define __BPX_fsetxattr    0 /* does z/OS have xattr support? */
#define __BPX_getxattr     0 /* does z/OS have xattr support? */
#define __BPX_lgetxattr    0 /* does z/OS have xattr support? */
#define __BPX_fgetxattr    0 /* does z/OS have xattr support? */
#define __BPX_listxattr    0 /* does z/OS have xattr support? */
#define __BPX_llistxattr   0 /* does z/OS have xattr support? */
#define __BPX_flistxattr   0 /* does z/OS have xattr support? */
#define __BPX_removexattr  0 /* does z/OS have xattr support? */
#define __BPX_lremovexattr 0 /* does z/OS have xattr support? */
#define __BPX_fremovexattr 0 /* does z/OS have xattr support? */
#define __BPX_gettid 0 /* no glibc wrapper expected */
#define __BPX_tkill  0 /* no glibc wrapper expected */
#define __BPX_futex  0 /* no glibc wrapper expected */
#define __BPX_sched_setaffinity 0 /* bpx scheduler interface? */
#define __BPX_sched_getaffinity 0 /* bpx scheduler interface? */
#define __BPX_tgkill 0 /* no glibc wrapper expected */
#define __BPX_io_setup     0 /* no glibc wrapper expected */
#define __BPX_io_destroy   0 /* no glibc wrapper expected */
#define __BPX_io_getevents 0 /* no glibc wrapper expected */
#define __BPX_io_submit    0 /* no glibc wrapper expected */
#define __BPX_io_cancel    0 /* no glibc wrapper expected */
#define __BPX_exit_group   0 /* z/OS thread support is strange */
#define __BPX_epoll_create 0 /* no epoll support */
#define __BPX_epoll_ctl    0 /* no epoll support */
#define __BPX_epoll_wait   0 /* no epoll support */
#define __BPX_set_tid_address 0 /* z/OS thread support is strange */
#define __BPX_fadvise64 0 /* no support for kernel hinting */
#define __BPX_timer_create     0 /* evented IO? */
#define __BPX_timer_settime    0 /* evented IO? */
#define __BPX_timer_gettime    0 /* evented IO? */
#define __BPX_timer_getoverrun 0 /* evented IO? */
#define __BPX_timer_delete     0 /* evented IO? */
#define __BPX_clock_settime    0 /* z/OS clock support? */
#define __BPX_clock_gettime    0 /* z/OS clock support? */
#define __BPX_clock_getres     0 /* z/OS clock support? */
#define __BPX_clock_nanosleep  0 /* z/OS clock support? */
#define __BPX_statfs64  0 /* implement with statvfs */
#define __BPX_fstatfs64 0 /* implement with fstatvfs */
#define __BPX_remap_file_pages 0 /* deprecated in linux */
#define __BPX_mbind         0 /* no NUMA policy support */
#define __BPX_get_mempolicy 0 /* no NUMA policy support */
#define __BPX_set_mempolicy 0 /* no NUMA policy support */
#define __BPX_mq_open         0 /* z/OS MQs don't support this interface natively */
#define __BPX_mq_unlink       0 /* z/OS MQs don't support this interface natively */
#define __BPX_mq_timedsend    0 /* z/OS MQs don't support this interface natively */
#define __BPX_mq_timedreceive 0 /* z/OS MQs don't support this interface natively */
#define __BPX_mq_notify       0 /* z/OS MQs don't support this interface natively */
#define __BPX_mq_getsetattr   0 /* z/OS MQs don't support this interface natively */
#define __BPX_kexec_load  0 /* z/OS only has the one kernel */
#define __BPX_add_key     0 /* no keyring */
#define __BPX_request_key 0 /* no keyring */
#define __BPX_keyctl      0 /* no keyring */
#define __BPX_waitid __BPX_off_waitid_wait3
#define __BPX_ioprio_set 0 /* IO scheduler interface? */
#define __BPX_ioprio_get 0 /* IO scheduler interface? */
#define __BPX_inotify_init      0 /* no inotify-style interface */
#define __BPX_inotify_add_watch 0 /* no inotify-style interface */
#define __BPX_inotify_rm_watch  0 /* no inotify-style interface */
#define __BPX_migrate_pages     0 /* no compatible interface */
#define __BPX_openat     0 /* can be implemented with open */
#define __BPX_mkdirat    0 /* can be implemented with mkdir */
#define __BPX_mknodat    0 /* can be implemented with mknod */
#define __BPX_fchownat   0 /* can be implemented with fchown */
#define __BPX_futimesat  0 /* can be implemented with futimes */
#define __BPX_newfstatat 0 /* can be implemented with newfstat */
#define __BPX_unlinkat   0 /* can be implemented with unlink */
#define __BPX_renameat   0 /* can be implemented with rename */
#define __BPX_linkat     0 /* can be implemented with link */
#define __BPX_symlinkat  0 /* can be implemented with symlink */
#define __BPX_readlinkat 0 /* can be implemented with readlink */
#define __BPX_fchmodat   0 /* can be implemented with fchmod */
#define __BPX_faccessat  0 /* can be implemented with faccess */
#define __BPX_pselect6   0 /* an equivalent interface MUST exist somewhere */
#define __BPX_ppoll      0 /* an equivalent interface MUST exist somewhere */
#define __BPX_unshare    0 /* maybe implement using shmem calls */
#define __BPX_set_robust_list 0 /* no clearly equivalent calls */
#define __BPX_get_robust_list 0 /* no clearly equivalent calls */
#define __BPX_splice     0 /* can be emulated using read+write */
#define __BPX_sync_file_range 0 /* is there an easy way to emulate */
#define __BPX_tee        0 /* can be emulated */
#define __BPX_vmsplice   0 /* difficult to emulate */
#define __BPX_move_pages 0 /* no equivalent interface exists */
#define __BPX_getcpu     0 /* no equivalent interface exists */
#define __BPX_epoll_pwait 0 /* no epoll support */
#define __BPX_utimes     0 /* maybe implementable via utime */
#define __BPX_fallocate  0 /* fallback impl for posix_fallocate */
#define __BPX_utimensat  0 /* maybe implement with fcntl */
#define __BPX_signalfd   0 /* no equivalent interface exists */
#define __BPX_timerfd    0 /* no equivalent interface exists */
#define __BPX_eventfd    0 /* no equivalent interface exists */
#define __BPX_timerfd_create  0 /* no equivalent interface exists */
#define __BPX_timerfd_settime 0 /* no equivalent interface exists */
#define __BPX_timerfd_gettime 0 /* no equivalent interface exists */
#define __BPX_signalfd4  0 /* no equivalent interface exists */
#define __BPX_eventfd2   0 /* no equivalent interface exists */
#define __BPX_inotify_init1 0 /* no inotify support */
#define __BPX_pipe2      0 /* implement with pipe+fcntl */
#define __BPX_dup3       0 /* implement with fcntl */
#define __BPX_epoll_create1 0 /* no epoll support */
#define __BPX_preadv     0 /* implement as shim over readv */
#define __BPX_pwritev    0 /* implement as shim over writev */
#define __BPX_rt_tgsigqueueinfo 0 /* no rt signal support */
#define __BPX_perf_event_open   0 /* no perf event support */
#define __BPX_fanotify_init 0 /* no fanotify support */
#define __BPX_fanotify_mark 0 /* no fanotify support */
#define __BPX_prlimit64 0 /* implement with set_thread_limits if necessary */
#define __BPX_name_to_handle_at 0 /* emulate with open() */
#define __BPX_open_by_handle_at 0 /* emulate with open() */
#define __BPX_clock_adjtime 0 /* z/OS clock support? */
#define __BPX_syncfs 0 /* z/OS mount interface? */
#define __BPX_setns  0 /* no equivalent interface */
#define __BPX_process_vm_readv   0 /* emulate with readv */
#define __BPX_process_vm_writev  0 /* emulate with writev */
#define __BPX_s390_runtime_instr 0 /* equivalent interface? */
#define __BPX_kcmp 0 /* no equivalent interface */
#define __BPX_finit_module  0 /* no equivalent interface */
#define __BPX_sched_setattr 0 /* scheduler interface? */
#define __BPX_sched_getattr 0 /* scheduler interface? */
#define __BPX_renameat2     0 /* no equivalent interface */
#define __BPX_seccomp       0 /* no equivalent interface */
#define __BPX_getrandom     0 /* z/OS RNG interface? */
#define __BPX_memfd_create  0 /* shim it if necessary */
#define __BPX_bpf 0 /* z/OS packet filtering? */
#define __BPX_s390_pci_mmio_write 0 /* z/OS mmio interface? */
#define __BPX_s390_pci_mmio_read  0 /* z/OS mmio interface? */
#define __BPX_execveat    0 /* shim it if necessary */
#define __BPX_userfaultfd 0 /* z/OS page fault handling? */
#define __BPX_membarrier  0 /* how does this work? */
#define __BPX_recvmmsg    0 /* emulate with recvmsg if necessary */
#define __BPX_sendmmsg    0 /* emulate with sendmsg if necessary */
#define __BPX_socket      0 /* emulate with other socket opts */
#define __BPX_socketpair  0 /* emulate with other socket opts */
#define __BPX_bind    __BPX_off_bind /* bind2addrsel? */
#define __BPX_connect __BPX_off_connect
#define __BPX_listen  __BPX_off_listen
#define __BPX_accept4 0 /* no equivalent interface */
#define __BPX_getsockopt __BPX_off_getsockopt /* get/setsockopt share a call */
#define __BPX_setsockopt __BPX_off_getsockopt /* get/setsockopt share a call */
#define __BPX_getsockname 0 /* z/OS socket addr */
#define __BPX_getpeername 0 /* z/OS socket addr */
#define __BPX_sendto   __BPX_off_sendto
#define __BPX_sendmsg  __BPX_off_sendmsg /* sendmsg2? */
#define __BPX_recvfrom __BPX_off_recvfrom
#define __BPX_recvmsg  __BPX_off_recvmsg /* recvmsg2? */
#define __BPX_shutdown 0 /* maybe don't implement this for now */
#define __BPX_mlock2   0 /* no equivalent memlock interface */
#define __BPX_copy_file_range 0 /* emulate with read+write */
#define __BPX_preadv2  0 /* no equivalent interface */
#define __BPX_pwritev2 0 /* no equivalent interface */
#define __BPX_s390_guarded_storage 0 /* implement from scratch */
#define __BPX_statx      0 /* partially emulate with stat */
#define __BPX_s390_sthyi 0 /* hold off on implementing for now */

/* Syscalls that don't exist on s390x linux.  */
#define __BPX_msgctl __BPX_off_msgctl
#define __BPX_msgget __BPX_off_msgget
#define __BPX_msgrcv __BPX_off_msgrcv
#define __BPX_msgsnd __BPX_off_msgsnd
#define __BPX_semctl __BPX_off_semctl
#define __BPX_semget __BPX_off_semget
#define __BPX_semop __BPX_off_semop
#define __BPX_shmat __BPX_off_shmat
#define __BPX_shmctl __BPX_off_shmctl
#define __BPX_shmdt __BPX_off_shmdt
#define __BPX_shmget __BPX_off_shmget
#define __BPX_accept __BPX_off_accept

/* some things use these aliases */
#define __BPX_fcntl64 __BPX_fcntl
#define __BPX_fadvise64_64 __BPX_fadvise64
#define __BPX_truncate64 __BPX_truncate
#define __BPX_ftruncate64 __BPX_ftruncate
#define __BPX_pwritev64 __BPX_pwritev
#define __BPX_pwritev64v2 __BPX_pwritev2
#define __BPX_preadv64 __BPX_preadv
#define __BPX_preadv64v2 __BPX_preadv2


/* TODO: move this stuff out of here */

/* whether or not the call is enabled or disabled right now. This stuff should
   not be present in any final version, it's just for debugging in the early
   stages of the port. To enable a syscall, define the corresponding macro
   below to 'if_true', and the corresponding macro above to the proper
   proto.  */

#define __shim_enabled_exit(if_true, if_false) if_false
#define __shim_enabled_fork(if_true, if_false) if_true
#define __shim_enabled_read(if_true, if_false) if_true
#define __shim_enabled_write(if_true, if_false) if_true
#define __shim_enabled_open(if_true, if_false) if_true
#define __shim_enabled_close(if_true, if_false) if_true
#define __shim_enabled_restart_syscall(if_true, if_false) if_false
#define __shim_enabled_creat(if_true, if_false) if_false
#define __shim_enabled_link(if_true, if_false) if_false
#define __shim_enabled_unlink(if_true, if_false) if_true
#define __shim_enabled_execve(if_true, if_false) if_true
#define __shim_enabled_chdir(if_true, if_false) if_true
#define __shim_enabled_mknod(if_true, if_false) if_false
#define __shim_enabled_chmod(if_true, if_false) if_true
#define __shim_enabled_lseek(if_true, if_false) if_true
#define __shim_enabled_getpid(if_true, if_false) if_true
#define __shim_enabled_mount(if_true, if_false) if_false
#define __shim_enabled_umount(if_true, if_false) if_false
#define __shim_enabled_ptrace(if_true, if_false) if_false
#define __shim_enabled_alarm(if_true, if_false) if_true
#define __shim_enabled_pause(if_true, if_false) if_false
#define __shim_enabled_utime(if_true, if_false) if_false
#define __shim_enabled_access(if_true, if_false) if_true
#define __shim_enabled_nice(if_true, if_false) if_false
#define __shim_enabled_sync(if_true, if_false) if_false
#define __shim_enabled_kill(if_true, if_false) if_false
#define __shim_enabled_rename(if_true, if_false) if_false
#define __shim_enabled_mkdir(if_true, if_false) if_true
#define __shim_enabled_rmdir(if_true, if_false) if_true
#define __shim_enabled_dup(if_true, if_false) if_false
#define __shim_enabled_pipe(if_true, if_false) if_false
#define __shim_enabled_times(if_true, if_false) if_false
#define __shim_enabled_brk(if_true, if_false) if_false
#define __shim_enabled_signal(if_true, if_false) if_false
#define __shim_enabled_acct(if_true, if_false) if_false
#define __shim_enabled_umount2(if_true, if_false) if_false
#define __shim_enabled_ioctl(if_true, if_false) if_false
#define __shim_enabled_fcntl(if_true, if_false) if_true
#define __shim_enabled_setpgid(if_true, if_false) if_false
#define __shim_enabled_umask(if_true, if_false) if_true
#define __shim_enabled_chroot(if_true, if_false) if_false
#define __shim_enabled_ustat(if_true, if_false) if_false
#define __shim_enabled_dup2(if_true, if_false) if_true
#define __shim_enabled_getppid(if_true, if_false) if_true
#define __shim_enabled_getpgrp(if_true, if_false) if_false
#define __shim_enabled_setsid(if_true, if_false) if_false
#define __shim_enabled_sigaction(if_true, if_false) if_true
#define __shim_enabled_sigsuspend(if_true, if_false) if_false
#define __shim_enabled_sigpending(if_true, if_false) if_false
#define __shim_enabled_sethostname(if_true, if_false) if_false
#define __shim_enabled_setrlimit(if_true, if_false) if_false
#define __shim_enabled_getrusage(if_true, if_false) if_false
#define __shim_enabled_gettimeofday(if_true, if_false) if_false
#define __shim_enabled_settimeofday(if_true, if_false) if_false
#define __shim_enabled_symlink(if_true, if_false) if_true
#define __shim_enabled_readlink(if_true, if_false) if_true
#define __shim_enabled_uselib(if_true, if_false) if_false
#define __shim_enabled_swapon(if_true, if_false) if_false
#define __shim_enabled_reboot(if_true, if_false) if_false
#define __shim_enabled_readdir(if_true, if_false) if_false
#define __shim_enabled_mmap(if_true, if_false) if_true
#define __shim_enabled_munmap(if_true, if_false) if_true
#define __shim_enabled_truncate(if_true, if_false) if_true
#define __shim_enabled_ftruncate(if_true, if_false) if_true
#define __shim_enabled_fchmod(if_true, if_false) if_true
#define __shim_enabled_getpriority(if_true, if_false) if_false
#define __shim_enabled_setpriority(if_true, if_false) if_false
#define __shim_enabled_statfs(if_true, if_false) if_false
#define __shim_enabled_fstatfs(if_true, if_false) if_false
#define __shim_enabled_socketcall(if_true, if_false) if_false
#define __shim_enabled_syslog(if_true, if_false) if_false
#define __shim_enabled_setitimer(if_true, if_false) if_false
#define __shim_enabled_getitimer(if_true, if_false) if_false
#define __shim_enabled_stat(if_true, if_false) if_true
#define __shim_enabled_lstat(if_true, if_false) if_true
#define __shim_enabled_fstat(if_true, if_false) if_true
#define __shim_enabled_lookup_dcookie(if_true, if_false) if_false
#define __shim_enabled_vhangup(if_true, if_false) if_false
#define __shim_enabled_idle(if_true, if_false) if_false
#define __shim_enabled_waitpid(if_true, if_false) if_true
#define __shim_enabled_wait(if_true, if_false) if_true
#define __shim_enabled_wait4(if_true, if_false) if_false
#define __shim_enabled_swapoff(if_true, if_false) if_false
#define __shim_enabled_sysinfo(if_true, if_false) if_false
#define __shim_enabled_ipc(if_true, if_false) if_false
#define __shim_enabled_fsync(if_true, if_false) if_false
#define __shim_enabled_sigreturn(if_true, if_false) if_false
#define __shim_enabled_clone(if_true, if_false) if_false
#define __shim_enabled_setdomainname(if_true, if_false) if_false
#define __shim_enabled_uname(if_true, if_false) if_false
#define __shim_enabled_adjtimex(if_true, if_false) if_false
#define __shim_enabled_mprotect(if_true, if_false) if_true
#define __shim_enabled_sigprocmask(if_true, if_false) if_true
#define __shim_enabled_create_module(if_true, if_false) if_false
#define __shim_enabled_init_module(if_true, if_false) if_false
#define __shim_enabled_delete_module(if_true, if_false) if_false
#define __shim_enabled_get_kernel_syms(if_true, if_false) if_false
#define __shim_enabled_quotactl(if_true, if_false) if_false
#define __shim_enabled_getpgid(if_true, if_false) if_true
#define __shim_enabled_fchdir(if_true, if_false) if_true
#define __shim_enabled_bdflush(if_true, if_false) if_false
#define __shim_enabled_sysfs(if_true, if_false) if_false
#define __shim_enabled_personality(if_true, if_false) if_false
#define __shim_enabled_afs_syscall(if_true, if_false) if_false
#define __shim_enabled_getdents(if_true, if_false) if_false
#define __shim_enabled_select(if_true, if_false) if_false
#define __shim_enabled_flock(if_true, if_false) if_false
#define __shim_enabled_msync(if_true, if_false) if_false
#define __shim_enabled_readv(if_true, if_false) if_false
#define __shim_enabled_writev(if_true, if_false) if_true
#define __shim_enabled_getsid(if_true, if_false) if_false
#define __shim_enabled_fdatasync(if_true, if_false) if_false
#define __shim_enabled__sysctl(if_true, if_false) if_false
#define __shim_enabled_mlock(if_true, if_false) if_false
#define __shim_enabled_munlock(if_true, if_false) if_false
#define __shim_enabled_mlockall(if_true, if_false) if_false
#define __shim_enabled_munlockall(if_true, if_false) if_false
#define __shim_enabled_sched_setparam(if_true, if_false) if_false
#define __shim_enabled_sched_getparam(if_true, if_false) if_false
#define __shim_enabled_sched_setscheduler(if_true, if_false) if_false
#define __shim_enabled_sched_getscheduler(if_true, if_false) if_false
#define __shim_enabled_sched_yield(if_true, if_false) if_false
#define __shim_enabled_sched_get_priority_max(if_true, if_false) if_false
#define __shim_enabled_sched_get_priority_min(if_true, if_false) if_false
#define __shim_enabled_sched_rr_get_interval(if_true, if_false) if_false
#define __shim_enabled_nanosleep(if_true, if_false) if_false
#define __shim_enabled_mremap(if_true, if_false) if_false
#define __shim_enabled_query_module(if_true, if_false) if_false
#define __shim_enabled_poll(if_true, if_false) if_false
#define __shim_enabled_nfsservctl(if_true, if_false) if_false
#define __shim_enabled_prctl(if_true, if_false) if_false
#define __shim_enabled_rt_sigreturn(if_true, if_false) if_false
#define __shim_enabled_rt_sigaction(if_true, if_false) if_false
#define __shim_enabled_rt_sigprocmask(if_true, if_false) if_false
#define __shim_enabled_rt_sigpending(if_true, if_false) if_false
#define __shim_enabled_rt_sigtimedwait(if_true, if_false) if_false
#define __shim_enabled_rt_sigqueueinfo(if_true, if_false) if_false
#define __shim_enabled_rt_sigsuspend(if_true, if_false) if_false
#define __shim_enabled_pread64(if_true, if_false) if_false
#define __shim_enabled_pwrite64(if_true, if_false) if_false
#define __shim_enabled_getcwd(if_true, if_false) if_true
#define __shim_enabled_capget(if_true, if_false) if_false
#define __shim_enabled_capset(if_true, if_false) if_false
#define __shim_enabled_sigaltstack(if_true, if_false) if_false
#define __shim_enabled_sendfile(if_true, if_false) if_false
#define __shim_enabled_getpmsg(if_true, if_false) if_false
#define __shim_enabled_putpmsg(if_true, if_false) if_false
#define __shim_enabled_vfork(if_true, if_false) if_false
#define __shim_enabled_getrlimit(if_true, if_false) if_false
#define __shim_enabled_lchown(if_true, if_false) if_true
#define __shim_enabled_getuid(if_true, if_false) if_true
#define __shim_enabled_getgid(if_true, if_false) if_true
#define __shim_enabled_geteuid(if_true, if_false) if_true
#define __shim_enabled_getegid(if_true, if_false) if_true
#define __shim_enabled_setreuid(if_true, if_false) if_true
#define __shim_enabled_setregid(if_true, if_false) if_true
#define __shim_enabled_getgroups(if_true, if_false) if_false
#define __shim_enabled_setgroups(if_true, if_false) if_false
#define __shim_enabled_fchown(if_true, if_false) if_true
#define __shim_enabled_setresuid(if_true, if_false) if_false
#define __shim_enabled_getresuid(if_true, if_false) if_false
#define __shim_enabled_setresgid(if_true, if_false) if_false
#define __shim_enabled_getresgid(if_true, if_false) if_false
#define __shim_enabled_chown(if_true, if_false) if_true
#define __shim_enabled_setuid(if_true, if_false) if_true
#define __shim_enabled_setgid(if_true, if_false) if_true
#define __shim_enabled_setfsuid(if_true, if_false) if_false
#define __shim_enabled_setfsgid(if_true, if_false) if_false
#define __shim_enabled_pivot_root(if_true, if_false) if_false
#define __shim_enabled_mincore(if_true, if_false) if_false
#define __shim_enabled_madvise(if_true, if_false) if_false
#define __shim_enabled_getdents64(if_true, if_false) if_false
#define __shim_enabled_readahead(if_true, if_false) if_false
#define __shim_enabled_setxattr(if_true, if_false) if_false
#define __shim_enabled_lsetxattr(if_true, if_false) if_false
#define __shim_enabled_fsetxattr(if_true, if_false) if_false
#define __shim_enabled_getxattr(if_true, if_false) if_false
#define __shim_enabled_lgetxattr(if_true, if_false) if_false
#define __shim_enabled_fgetxattr(if_true, if_false) if_false
#define __shim_enabled_listxattr(if_true, if_false) if_false
#define __shim_enabled_llistxattr(if_true, if_false) if_false
#define __shim_enabled_flistxattr(if_true, if_false) if_false
#define __shim_enabled_removexattr(if_true, if_false) if_false
#define __shim_enabled_lremovexattr(if_true, if_false) if_false
#define __shim_enabled_fremovexattr(if_true, if_false) if_false
#define __shim_enabled_gettid(if_true, if_false) if_false
#define __shim_enabled_tkill(if_true, if_false) if_false
#define __shim_enabled_futex(if_true, if_false) if_false
#define __shim_enabled_sched_setaffinity(if_true, if_false) if_false
#define __shim_enabled_sched_getaffinity(if_true, if_false) if_false
#define __shim_enabled_tgkill(if_true, if_false) if_false
#define __shim_enabled_io_setup(if_true, if_false) if_false
#define __shim_enabled_io_destroy(if_true, if_false) if_false
#define __shim_enabled_io_getevents(if_true, if_false) if_false
#define __shim_enabled_io_submit(if_true, if_false) if_false
#define __shim_enabled_io_cancel(if_true, if_false) if_false
#define __shim_enabled_exit_group(if_true, if_false) if_true
#define __shim_enabled_epoll_create(if_true, if_false) if_false
#define __shim_enabled_epoll_ctl(if_true, if_false) if_false
#define __shim_enabled_epoll_wait(if_true, if_false) if_false
#define __shim_enabled_set_tid_address(if_true, if_false) if_false
#define __shim_enabled_fadvise64(if_true, if_false) if_false
#define __shim_enabled_timer_create(if_true, if_false) if_false
#define __shim_enabled_timer_settime(if_true, if_false) if_false
#define __shim_enabled_timer_gettime(if_true, if_false) if_false
#define __shim_enabled_timer_getoverrun(if_true, if_false) if_false
#define __shim_enabled_timer_delete(if_true, if_false) if_false
#define __shim_enabled_clock_settime(if_true, if_false) if_false
#define __shim_enabled_clock_gettime(if_true, if_false) if_false
#define __shim_enabled_clock_getres(if_true, if_false) if_false
#define __shim_enabled_clock_nanosleep(if_true, if_false) if_false
#define __shim_enabled_statfs64(if_true, if_false) if_false
#define __shim_enabled_fstatfs64(if_true, if_false) if_false
#define __shim_enabled_remap_file_pages(if_true, if_false) if_false
#define __shim_enabled_mbind(if_true, if_false) if_false
#define __shim_enabled_get_mempolicy(if_true, if_false) if_false
#define __shim_enabled_set_mempolicy(if_true, if_false) if_false
#define __shim_enabled_mq_open(if_true, if_false) if_false
#define __shim_enabled_mq_unlink(if_true, if_false) if_false
#define __shim_enabled_mq_timedsend(if_true, if_false) if_false
#define __shim_enabled_mq_timedreceive(if_true, if_false) if_false
#define __shim_enabled_mq_notify(if_true, if_false) if_false
#define __shim_enabled_mq_getsetattr(if_true, if_false) if_false
#define __shim_enabled_kexec_load(if_true, if_false) if_false
#define __shim_enabled_add_key(if_true, if_false) if_false
#define __shim_enabled_request_key(if_true, if_false) if_false
#define __shim_enabled_keyctl(if_true, if_false) if_false
#define __shim_enabled_waitid(if_true, if_false) if_false
#define __shim_enabled_ioprio_set(if_true, if_false) if_false
#define __shim_enabled_ioprio_get(if_true, if_false) if_false
#define __shim_enabled_inotify_init(if_true, if_false) if_false
#define __shim_enabled_inotify_add_watch(if_true, if_false) if_false
#define __shim_enabled_inotify_rm_watch(if_true, if_false) if_false
#define __shim_enabled_migrate_pages(if_true, if_false) if_false
#define __shim_enabled_openat(if_true, if_false) if_true
#define __shim_enabled_mkdirat(if_true, if_false) if_false
#define __shim_enabled_mknodat(if_true, if_false) if_false
#define __shim_enabled_fchownat(if_true, if_false) if_true
#define __shim_enabled_futimesat(if_true, if_false) if_false
#define __shim_enabled_newfstatat(if_true, if_false) if_false
#define __shim_enabled_unlinkat(if_true, if_false) if_false
#define __shim_enabled_renameat(if_true, if_false) if_false
#define __shim_enabled_linkat(if_true, if_false) if_false
#define __shim_enabled_symlinkat(if_true, if_false) if_false
#define __shim_enabled_readlinkat(if_true, if_false) if_false
#define __shim_enabled_fchmodat(if_true, if_false) if_false
#define __shim_enabled_faccessat(if_true, if_false) if_false
#define __shim_enabled_pselect6(if_true, if_false) if_false
#define __shim_enabled_ppoll(if_true, if_false) if_false
#define __shim_enabled_unshare(if_true, if_false) if_false
#define __shim_enabled_set_robust_list(if_true, if_false) if_false
#define __shim_enabled_get_robust_list(if_true, if_false) if_false
#define __shim_enabled_splice(if_true, if_false) if_false
#define __shim_enabled_sync_file_range(if_true, if_false) if_false
#define __shim_enabled_tee(if_true, if_false) if_false
#define __shim_enabled_vmsplice(if_true, if_false) if_false
#define __shim_enabled_move_pages(if_true, if_false) if_false
#define __shim_enabled_getcpu(if_true, if_false) if_false
#define __shim_enabled_epoll_pwait(if_true, if_false) if_false
#define __shim_enabled_utimes(if_true, if_false) if_false
#define __shim_enabled_fallocate(if_true, if_false) if_false
#define __shim_enabled_utimensat(if_true, if_false) if_false
#define __shim_enabled_signalfd(if_true, if_false) if_false
#define __shim_enabled_timerfd(if_true, if_false) if_false
#define __shim_enabled_eventfd(if_true, if_false) if_false
#define __shim_enabled_timerfd_create(if_true, if_false) if_false
#define __shim_enabled_timerfd_settime(if_true, if_false) if_false
#define __shim_enabled_timerfd_gettime(if_true, if_false) if_false
#define __shim_enabled_signalfd4(if_true, if_false) if_false
#define __shim_enabled_eventfd2(if_true, if_false) if_false
#define __shim_enabled_inotify_init1(if_true, if_false) if_false
#define __shim_enabled_pipe2(if_true, if_false) if_true
#define __shim_enabled_dup3(if_true, if_false) if_false
#define __shim_enabled_epoll_create1(if_true, if_false) if_false
#define __shim_enabled_preadv(if_true, if_false) if_false
#define __shim_enabled_pwritev(if_true, if_false) if_false
#define __shim_enabled_rt_tgsigqueueinfo(if_true, if_false) if_false
#define __shim_enabled_perf_event_open(if_true, if_false) if_false
#define __shim_enabled_fanotify_init(if_true, if_false) if_false
#define __shim_enabled_fanotify_mark(if_true, if_false) if_false
#define __shim_enabled_prlimit64(if_true, if_false) if_false
#define __shim_enabled_name_to_handle_at(if_true, if_false) if_false
#define __shim_enabled_open_by_handle_at(if_true, if_false) if_false
#define __shim_enabled_clock_adjtime(if_true, if_false) if_false
#define __shim_enabled_syncfs(if_true, if_false) if_false
#define __shim_enabled_setns(if_true, if_false) if_false
#define __shim_enabled_process_vm_readv(if_true, if_false) if_false
#define __shim_enabled_process_vm_writev(if_true, if_false) if_false
#define __shim_enabled_s390_runtime_instr(if_true, if_false) if_false
#define __shim_enabled_kcmp(if_true, if_false) if_false
#define __shim_enabled_finit_module(if_true, if_false) if_false
#define __shim_enabled_sched_setattr(if_true, if_false) if_false
#define __shim_enabled_sched_getattr(if_true, if_false) if_false
#define __shim_enabled_renameat2(if_true, if_false) if_false
#define __shim_enabled_seccomp(if_true, if_false) if_false
#define __shim_enabled_getrandom(if_true, if_false) if_false
#define __shim_enabled_memfd_create(if_true, if_false) if_false
#define __shim_enabled_bpf(if_true, if_false) if_false
#define __shim_enabled_s390_pci_mmio_write(if_true, if_false) if_false
#define __shim_enabled_s390_pci_mmio_read(if_true, if_false) if_false
#define __shim_enabled_execveat(if_true, if_false) if_false
#define __shim_enabled_userfaultfd(if_true, if_false) if_false
#define __shim_enabled_membarrier(if_true, if_false) if_false
#define __shim_enabled_recvmmsg(if_true, if_false) if_false
#define __shim_enabled_sendmmsg(if_true, if_false) if_false
#define __shim_enabled_socket(if_true, if_false) if_false
#define __shim_enabled_socketpair(if_true, if_false) if_false
#define __shim_enabled_bind(if_true, if_false) if_false
#define __shim_enabled_connect(if_true, if_false) if_false
#define __shim_enabled_listen(if_true, if_false) if_false
#define __shim_enabled_accept4(if_true, if_false) if_false
#define __shim_enabled_getsockopt(if_true, if_false) if_false
#define __shim_enabled_setsockopt(if_true, if_false) if_false
#define __shim_enabled_getsockname(if_true, if_false) if_false
#define __shim_enabled_getpeername(if_true, if_false) if_false
#define __shim_enabled_sendto(if_true, if_false) if_false
#define __shim_enabled_sendmsg(if_true, if_false) if_false
#define __shim_enabled_recvfrom(if_true, if_false) if_false
#define __shim_enabled_recvmsg(if_true, if_false) if_false
#define __shim_enabled_shutdown(if_true, if_false) if_false
#define __shim_enabled_mlock2(if_true, if_false) if_false
#define __shim_enabled_copy_file_range(if_true, if_false) if_false
#define __shim_enabled_preadv2(if_true, if_false) if_false
#define __shim_enabled_pwritev2(if_true, if_false) if_false
#define __shim_enabled_s390_guarded_storage(if_true, if_false) if_false
#define __shim_enabled_statx(if_true, if_false) if_false
#define __shim_enabled_s390_sthyi(if_true, if_false) if_false

/* z/OS-specific calls. These are calls for which either linux has no
   equivalent, or do have a linux equivalent, just not on s390x.  */
#define __shim_enabled_msgctl(if_true, if_false) if_false
#define __shim_enabled_msgget(if_true, if_false) if_false
#define __shim_enabled_msgrcv(if_true, if_false) if_false
#define __shim_enabled_msgsnd(if_true, if_false) if_false
#define __shim_enabled_semctl(if_true, if_false) if_false
#define __shim_enabled_semget(if_true, if_false) if_false
#define __shim_enabled_semop(if_true, if_false) if_false
#define __shim_enabled_shmat(if_true, if_false) if_false
#define __shim_enabled_shmctl(if_true, if_false) if_false
#define __shim_enabled_shmdt(if_true, if_false) if_false
#define __shim_enabled_shmget(if_true, if_false) if_false
#define __shim_enabled_accept(if_true, if_false) if_false
#define __shim_enabled_opendir(if_true, if_false) if_false


/* some things use these aliases */
#define __shim_enabled_fcntl64(if_true, if_false) __shim_enabled_fcntl(if_true, if_false)
#define __shim_enabled_fadvise64_64(if_true, if_false) __shim_enabled_fadvise64(if_true, if_false)
#define __shim_enabled_truncate64(if_true, if_false) __shim_enabled_truncate(if_true, if_false)
#define __shim_enabled_ftruncate64(if_true, if_false) __shim_enabled_ftruncate(if_true, if_false)
#define __shim_enabled_pwritev64(if_true, if_false) __shim_enabled_pwritev(if_true, if_false)
#define __shim_enabled_pwritev64v2(if_true, if_false) __shim_enabled_pwritev2(if_true, if_false)
#define __shim_enabled_preadv64(if_true, if_false) __shim_enabled_preadv(if_true, if_false)
#define __shim_enabled_preadv64v2(if_true, if_false) __shim_enabled_preadv2(if_true, if_false)

#endif /* _ASM_ZOS_BPX_OFFSETS_H */
