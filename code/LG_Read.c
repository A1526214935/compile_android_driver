#include "LG_Read.h"
#include <linux/tty.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/cpu.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
/*
https://t.me/BigWhiteUE4
*/

// 在 .c 文件中
int LG_Pid = 0;
int GameID = 0;
uintptr_t libBase=0,Gname=0,Uworld=0,Ulevel=0,AActor=0,Matrix_Add=0,AcknowledgedPawn=0,ZSRootComponent=0;


extern struct mm_struct *get_task_mm(struct task_struct *task);

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 61))
extern void mmput(struct mm_struct *);

phys_addr_t translate_linear_address(struct mm_struct* mm, uintptr_t va) {

    pgd_t *pgd;
    p4d_t *p4d;
    pmd_t *pmd;
    pte_t *pte;
    pud_t *pud;
	
    phys_addr_t page_addr;
    uintptr_t page_offset;
    
    pgd = pgd_offset(mm, va);
    if(pgd_none(*pgd) || pgd_bad(*pgd)) {
        return 0;
    }
    p4d = p4d_offset(pgd, va);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
    	return 0;
    }
	pud = pud_offset(p4d,va);
	if(pud_none(*pud) || pud_bad(*pud)) {
        return 0;
    }
	pmd = pmd_offset(pud,va);
	if(pmd_none(*pmd)) {
        return 0;
    }
	pte = pte_offset_kernel(pmd,va);
	if(pte_none(*pte)) {
        return 0;
    }
	if(!pte_present(*pte)) {
        return 0;
    }
	//页物理地址
	page_addr = (phys_addr_t)(pte_pfn(*pte) << PAGE_SHIFT);
	//页内偏移
	page_offset = va & (PAGE_SIZE-1);
	
	return page_addr + page_offset;
}
#else
phys_addr_t translate_linear_address(struct mm_struct* mm, uintptr_t va) {

    pgd_t *pgd;
    pmd_t *pmd;
    pte_t *pte;
    pud_t *pud;
	
    phys_addr_t page_addr;
    uintptr_t page_offset;
    
    pgd = pgd_offset(mm, va);
    if(pgd_none(*pgd) || pgd_bad(*pgd)) {
        return 0;
    }
	pud = pud_offset(pgd,va);
	if(pud_none(*pud) || pud_bad(*pud)) {
        return 0;
    }
	pmd = pmd_offset(pud,va);
	if(pmd_none(*pmd)) {
        return 0;
    }
	pte = pte_offset_kernel(pmd,va);
	if(pte_none(*pte)) {
        return 0;
    }
	if(!pte_present(*pte)) {
        return 0;
    }
	//页物理地址
	page_addr = (phys_addr_t)(pte_pfn(*pte) << PAGE_SHIFT);
	//页内偏移
	page_offset = va & (PAGE_SIZE-1);
	
	return page_addr + page_offset;
}
#endif


int valid_phys_addr_range_new(phys_addr_t addr, size_t count) {
    return addr + count <= __pa(high_memory);
}


bool read_physical_address(phys_addr_t pa, void* buffer, size_t size) {
    void* mapped;

    if (!pfn_valid(__phys_to_pfn(pa))) {
        return false;
    }
    if (!valid_phys_addr_range_new(pa, size)) {
        return false;
    }
    mapped = ioremap_cache(pa, size);
    if (!mapped) {
        return false;
    }
    memcpy(buffer, mapped, size);

    iounmap(mapped);
    return true;
}


bool write_physical_address(phys_addr_t pa, void* buffer, size_t size) {
    void* mapped;

    if (!pfn_valid(__phys_to_pfn(pa))) {
        return false;
    }
    if (!valid_phys_addr_range_new(pa, size)) {
        return false;
    }
    mapped = ioremap_cache(pa, size);
    if (!mapped) {
        return false;
    }
    if(copy_from_user(mapped, buffer, size)) {
        iounmap(mapped);
        return false;
    }
    iounmap(mapped);
    return true;
}

bool read_process_memory(pid_t pid, uintptr_t addr, void* buffer, size_t size) {
    struct task_struct* task;
    struct mm_struct* mm;
    struct pid* pid_struct;
    phys_addr_t pa;
    bool result;
    pid_struct = find_get_pid(pid);
    if (!pid_struct) {
        return false;
    }
    task = get_pid_task(pid_struct, PIDTYPE_PID);
    if (!task) {
        return false;
    }
    mm = get_task_mm(task);
    if (!mm) {
        return false;
    }
    mmput(mm);
    pa = translate_linear_address(mm, addr);
    if (!pa) {
        return false;
    }
    result = read_physical_address(pa, buffer, size);
    return result;
}


bool write_process_memory(
    pid_t pid, 
    uintptr_t addr, 
    void* buffer, 
    size_t size) {
    
    struct task_struct* task;
    struct mm_struct* mm;
    struct pid* pid_struct;
    phys_addr_t pa;

    pid_struct = find_get_pid(pid);
    if (!pid_struct) {
        return false;
    }
    task = get_pid_task(pid_struct, PIDTYPE_PID);
    if (!task) {
        return false;
    }
    mm = get_task_mm(task);
    if (!mm) {
        return false;
    }
    mmput(mm);
    pa = translate_linear_address(mm, addr);
    if (!pa) {
        return false;
    }
    return write_physical_address(pa,buffer,size);
}


// 获取模块地址
uintptr_t LG_GetLibBase(void)
{
    uintptr_t var = get_module_base(LG_Pid,"libUE4.so");
    return (var);
}

bool LG_Read(uintptr_t addr, void *buffer, size_t size) {
    bool var = read_process_memory(LG_Pid,addr,buffer,size);
    return var;
}

// 获取D类内存
int LG_GetDword(uintptr_t addr)
{
    int var = 0;
    read_process_memory(LG_Pid, addr, &var, 4);
    return (var);
}

// 获取指针(64位游戏)
uintptr_t LG_GetAddr(uintptr_t addr)
{
    uintptr_t var = 0;
    read_process_memory(LG_Pid, addr, &var, 8);
    return (var);
}


char *GetName_New(int index)
{
    unsigned int Block = index >> 16;
    unsigned short int Offset = index & 65535;

    uintptr_t FNamePool = Gname;

    uintptr_t NamePoolChunk = LG_GetAddr(FNamePool + 0x40 + (Block * 0x8));

    uintptr_t FNameEntry = NamePoolChunk + (0x2 * Offset);

    short int FNameEntryHeader = LG_GetDword(FNameEntry);
    uintptr_t StrPtr = FNameEntry + 0x2;

    int StrLength = FNameEntryHeader >> 6;

    if (StrLength > 0 && StrLength < 250)
    {
        char *name = kmalloc(StrLength + 1, GFP_KERNEL);
        if (name)
        {
            LG_Read(StrPtr, name, StrLength * sizeof(char));
            name[StrLength] = '\0';
            return name;
        }
    }

    return "None";
}
