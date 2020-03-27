#ifndef CUMBIA_MACROS_H
#define CUMBIA_MACROS_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define perr(x, args...) do {  fprintf(stderr, "\e[1;31m!! \e[0;4merror\e[0m: "); printf(x, ##args); printf("\n"); }while(0)

// qstoc  and qslisttoc outside #ifdef CUMBIA_DEBUG_OUTPUT

#ifdef QT_CORE_LIB

#define qstoc(x) 			 	x.toStdString().c_str()

#define qslisttoc(x)	do { if(getenv("CUMBIA_PRINT")) { printf("  { "); for(int i = 0; i < x.size(); i++) printf("\"%s\", ", qstoc(x[i])); printf("\b\b }\n"); } }while(0)

#endif

// coloured versions of printf intended to be used temporarily in code

#define pgreentmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx]\e[1;32m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define predtmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;31m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pyellowtmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;33m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbluetmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;34m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pviolettmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;35m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define plbluetmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;36m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pgreen2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;32m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pyellow2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;33m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblue2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;34m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pviolet2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;35m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define plblue2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;36m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pbgreentmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;32;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbredtmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;31;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbyellowtmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;33;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbbluetmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;34;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbviolettmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;35;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblbluetmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;36;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pbgreen2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;32;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbred2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;31;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbyellow2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;33;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbblue2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;34;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbviolet2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;35;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblblue2tmp(x, args...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;36;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)




#ifdef CUMBIA_DEBUG_OUTPUT


#define pwarn(x, args...) do { if(getenv("CUMBIA_PRINT")) {printf("\e[0;32m[thread:0x%lx] \e[1;33m:-o \e[0;4mwarning\e[0m: ", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pinfo(x, args...) do {  if(getenv("CUMBIA_PRINT")) {printf("\e[0;32m[thread:0x%lx] \e[1;35m* \e[0;4minfo\e[0m: ", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

/* like pinfo but without newline */
#define pinfononl(x, args...) do {  if(getenv("CUMBIA_PRINT")) {printf("\e[0;32m[thread:0x%lx] \e[1;35m* \e[0;4minfo\e[0m: ", pthread_self()), printf(x, ##args); } }while(0)

#define pok(x, args...) do {  if(getenv("CUMBIA_PRINT")) {printf("\033[1;32m:-) \033[0m", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define padd(x, args...) do { if(getenv("CUMBIA_PRINT")) {printf("\033[1;32m+ \033[0m", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pwrn(x, args...) do { if(getenv("CUMBIA_PRINT")) {printf("\033[1;33m:-o \033[0m", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pdelete(x, args...) do {   if(getenv("CUMBIA_PRINT")) {printf("\e[0;32m[thread:0x%lx] \e[1;32mX \e[0m", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pnodelete(x, args...) do { if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;31mX \e[0m", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pstep(x, args...) do { if(getenv("CUMBIA_PRINT")) { printf("   \e[1;37m[thread:0x%lx] \e[0;37m-\e[0m ", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pstepl2(x, args ...) do {  if(getenv("CUMBIA_PRINT")) { printf("    \e[1;37m[thread:0x%lx] \e[0;37m-\e[0m ", pthread_self()), printf(x, ##args), printf("\n"); } }while(0)

#define pgreen(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;32m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pred(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;32m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pyellow(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;33m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblue(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;34m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pviolet(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;35m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define plblue(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;36m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pgreen2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;32m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pyellow2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;33m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblue2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;34m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pviolet2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;35m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define plblue2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;36m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pbgreen(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;32;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbred(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;31;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbyellow(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;33;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbblue(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;34;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbviolet(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;35;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblblue(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[1;36;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define pbgreen2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;32;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbred2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;31;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbyellow2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;33;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbblue2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;34;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pbviolet2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;35;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)
#define pblblue2(x, args...) do {  if(getenv("CUMBIA_PRINT")) { printf("\e[0;32m[thread:0x%lx] \e[0;36;4m", pthread_self()), printf(x, ##args), printf("\e[0m\n"); } }while(0)

#define cuprintf(x, args ...) do  {  if(getenv("CUMBIA_CUPRINTF")) {printf(x, ##args); } }while(0)

#ifndef CUMBIA_NO_DEBUG_OUTPUT
        #define pr_thread()  do { if(getenv("CUMBIA_THREAD_PRINT")) { char pthself[128]; snprintf(pthself, 127, "[THREAD] 0x%lx obj: %p", pthread_self(), this); } }while(0)
#else
        #define pr_thread() do {}while(0)
#endif

    #else


        #define pwarn(x, args...) 		do {}while(0)

        #define pinfo(x, args...) 			do {}while(0)

        #define pinfononl(x, args...)    do {}while(0)

        #define pok(x, args...) 			do {}while(0)

        #define pwrn(x, args...) 			do {}while(0)

        #define pdelete(x, args...) 		do {}while(0)

        #define padd(x, args...) 	do {}while(0)

        #define pnodelete(x, args...) 		do {}while(0)

        #define qobjinfo(o)			""

        #define qobjname(o)			""

        #define objinfo(o)			""

        #define pstep(x, args...) 		do {}while(0)

        #define pstepl2(x, args...) 		do {}while(0)

        #define cuprintf(x, args...)		do {}while(0)

        #define pr_thread()                     do {}while(0)


        #define pr_thread() do {}while(0)

        #define pgreen(x, args...) do {}while(0)
        #define pred(x, args...) do {}while(0)
        #define pyellow(x, args...) do {}while(0)
        #define pblue(x, args...) do {}while(0)
        #define pviolet(x, args...) do {}while(0)
        #define plblue(x, args...) do {}while(0)

        #define pgreen2(x, args...) do {}while(0)
        #define pyellow2(x, args...) do {}while(0)
        #define pblue2(x, args...) do {}while(0)
        #define pviolet2(x, args...) do {}while(0)
        #define plblue2(x, args...) do {}while(0)

        #define pbgreen(x, args...) do {}while(0)
        #define pbred(x, args...) do {}while(0)
        #define pbyellow(x, args...) do {   } while(0)
        #define pbblue(x, args...) do {   }while(0)
        #define pbviolet(x, args...) do {   } while(0)
        #define pblblue(x, args...) do { }while(0)

        #define pbgreen2(x, args...) do { }while(0)
        #define pbred2(x, args...) do { }while(0)
        #define pbyellow2(x, args...) do {  } while(0)
        #define pbblue2(x, args...) do { }while(0)
        #define pbviolet2(x, args...) do {  }while(0)
        #define pblblue2(x, args...) do { }while(0)


    #endif



#endif

