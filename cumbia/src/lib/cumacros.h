#ifndef CUMBIA_MACROS_H
#define CUMBIA_MACROS_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define perr(...) do {  fprintf(stderr, "\e[1;31m!! \e[0;4merror\e[0m: "); printf(__VA_ARGS__); printf("\n"); }while(0)

#define stoc(x) x.c_str()

#define pretty_pri(...) do {  fprintf(stdout, "\e[0;32m%s\e[0m: [thread \e[1;36m0x%lx\e[0m] [this \e[0;34m%p\e[0m]: ", __PRETTY_FUNCTION__, pthread_self(), this); printf(__VA_ARGS__); printf("\n"); }while(0)

#define pfatal(...) do {  fprintf(stdout, "\e[0;31m%s\e[0m: \e[1;31;4mfatal\e[0m: [thread \e[1;36m0x%lx\e[0m] [this %p]:", __PRETTY_FUNCTION__, pthread_self(), this); printf(__VA_ARGS__); printf("\n"); abort(); }while(0)


// CuVariant when it's a string type
// example
// CuData d("astring", "hello");
// const char *hello = vtoc(d["astring"]);
#define vtoc(x)  x.toString().c_str()

// CuVariant when it's a string type, second form
// example
// CuData d("astring", "hello");
// const char *hello = vtoc(d, "astring");
#define vtoc2(x, n) x[n].toString().c_str()

#define datos(x) x.toString().c_str()

// qstoc  and qslisttoc outside #ifdef CUMBIA_DEBUG_OUTPUT

#ifdef QT_CORE_LIB

#define qstoc(x) 			 	x.toStdString().c_str()

#define qslisttoc(x)	do { if(getenv("CUMBIA_PRINT")) { printf("  { "); for(int i = 0; i < x.size(); i++) printf("\"%s\", ", qstoc(x[i])); printf("\b\b }\n"); } }while(0)

#endif

// coloured versions of printf intended to be used temporarily in code

#define pgreentmp(...) do {  { printf("\e[0;32m[thread:0x%lx]\e[1;32m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define predtmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;31m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pyellowtmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;33m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbluetmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;34m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pviolettmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;35m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define plbluetmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;36m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pgreen2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;32m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pyellow2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;33m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblue2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;34m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pviolet2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;35m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define plblue2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;36m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pbgreentmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;32;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbredtmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;31;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbyellowtmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;33;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbbluetmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;34;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbviolettmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;35;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblbluetmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[1;36;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pbgreen2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;32;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbred2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;31;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbyellow2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;33;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbblue2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;34;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbviolet2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;35;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblblue2tmp(...) do {  { printf("\e[0;32m[thread:0x%lx] \e[0;36;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)




#ifdef CUMBIA_DEBUG_OUTPUT


#define pwarn(...) do { if(getenv("CUMBIA_PRINT")) { printf("w: "); printf("\e[0;32m[thread:0x%lx] \e[1;33m:-o \e[0;4mwarning\e[0m: ", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pinfo(...) do {  if(getenv("CUMBIA_PRINT")) { printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;35m* \e[0;4minfo\e[0m: ", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

/* like pinfo but without newline */
#define pinfononl(...) do {  if(getenv("CUMBIA_PRINT")) { printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;35m* \e[0;4minfo\e[0m: ", pthread_self()), printf(__VA_ARGS__); } }while(0)

#define pok(...) do {  if(getenv("CUMBIA_PRINT")) { printf("I: "); printf("\033[1;32m:-) \033[0m", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define padd(...) do { if(getenv("CUMBIA_PRINT")) { printf("I: "); printf("\033[1;32m+ \033[0m", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pwrn(...) do { if(getenv("CUMBIA_PRINT")) { printf("w: "); printf("\033[1;33m:-o \033[0m", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pdelete(...) do {   if(getenv("CUMBIA_PRINT")) { printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;32mX \e[0m", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pnodelete(...) do { if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;31mX \e[0m", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pstep(...) do { if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("   \e[1;37m[thread:0x%lx] \e[0;37m-\e[0m ", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pstepl2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("    \e[1;37m[thread:0x%lx] \e[0;37m-\e[0m ", pthread_self()), printf(__VA_ARGS__), printf("\n"); } }while(0)

#define pgreen(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;32m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pyellow(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;33m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblue(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;34m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pviolet(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;35m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define plblue(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;36m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pgreen2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;32m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pyellow2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;33m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblue2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;34m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pviolet2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;35m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define plblue2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;36m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pbgreen(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;32;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbred(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;31;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbyellow(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;33;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbblue(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;34;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbviolet(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;35;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblblue(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[1;36;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define pbgreen2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;32;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbred2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;31;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbyellow2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;33;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbblue2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;34;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pbviolet2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;35;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)
#define pblblue2(...) do {  if(getenv("CUMBIA_PRINT")) {  printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0;36;4m", pthread_self()), printf(__VA_ARGS__), printf("\e[0m\n"); } }while(0)

#define cuprintf(...) do  {  if(getenv("CUMBIA_CUPRINTF")) { printf("I: "); printf("\e[0;32m[thread:0x%lx] \e[0m", pthread_self()), printf(__VA_ARGS__); } }while(0)

#ifndef CUMBIA_NO_DEBUG_OUTPUT
        #define pr_thread()  do { if(getenv("CUMBIA_THREAD_PRINT")) { char pthself[128]; snprintf(pthself, 127, "[THREAD] 0x%lx obj: %p", pthread_self(), this); } }while(0)
#else
        #define pr_thread() do {}while(0)
#endif

    #else


        #define pwarn(...) 		do {}while(0)

        #define pinfo(...) 			do {}while(0)

        #define pinfononl(...)    do {}while(0)

        #define pok(...) 			do {}while(0)

        #define pwrn(...) 			do {}while(0)

        #define pdelete(...) 		do {}while(0)

        #define padd(...) 	do {}while(0)

        #define pnodelete(...) 		do {}while(0)

        #define qobjinfo(o)			""

        #define qobjname(o)			""

        #define objinfo(o)			""

        #define pstep(...) 		do {}while(0)

        #define pstepl2(...) 		do {}while(0)

        #define cuprintf(...)		do {}while(0)

        #define pr_thread()                     do {}while(0)


        #define pr_thread() do {}while(0)

        #define pgreen(...) do {}while(0)
        #define pbred(...) do {}while(0)
        #define pyellow(...) do {}while(0)
        #define pblue(...) do {}while(0)
        #define pviolet(...) do {}while(0)
        #define plblue(...) do {}while(0)

        #define pgreen2(...) do {}while(0)
        #define pyellow2(...) do {}while(0)
        #define pblue2(...) do {}while(0)
        #define pviolet2(...) do {}while(0)
        #define plblue2(...) do {}while(0)

        #define pbgreen(...) do {}while(0)
        #define pbyellow(...) do {   } while(0)
        #define pbblue(...) do {   }while(0)
        #define pbviolet(...) do {   } while(0)
        #define pblblue(...) do { }while(0)

        #define pbgreen2(...) do { }while(0)
        #define pbred2(...) do { }while(0)
        #define pbyellow2(...) do {  } while(0)
        #define pbblue2(...) do { }while(0)
        #define pbviolet2(...) do {  }while(0)
        #define pblblue2(...) do { }while(0)


    #endif



#endif

