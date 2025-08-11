#include <linux/kernel.h>
#include <linux/sched.h>
#include "comm.h"
#include "process.h"
/*
https://t.me/BigWhiteUE4
*/

typedef unsigned short UTF16;
typedef char UTF8;
extern int LG_Pid;
extern uintptr_t libBase,Uworld,Ulevel,AActor,Matrix_Add,AcknowledgedPawn,ZSRootComponent;

phys_addr_t translate_linear_address(struct mm_struct* mm, uintptr_t va);

bool read_physical_address(phys_addr_t pa, void* buffer, size_t size);

bool write_physical_address(phys_addr_t pa, void* buffer, size_t size);

bool read_process_memory(pid_t pid, uintptr_t addr, void* buffer, size_t size);

bool write_process_memory(pid_t pid, uintptr_t addr, void* buffer, size_t size);

int valid_phys_addr_range_new(phys_addr_t addr, size_t count);

uintptr_t LG_GetAddr(uintptr_t addr);
uintptr_t LG_GetLibBase(void);
int LG_GetDword(uintptr_t addr);
bool LG_Read(uintptr_t addr, void *buffer, size_t size);
char *GetName_New(int index);