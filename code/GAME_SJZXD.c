#include "LG_Read.h"
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#include <linux/proc_fs.h>
/*
https://t.me/BigWhiteUE4
*/

static struct task_struct *LG_thread;
int AActorCount = 0, i = 0, j = 0, k = 0;
int matrix[16];
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LG");
MODULE_DESCRIPTION("My LG ESP.");
MODULE_VERSION("0.1");

static int GameSJZ(struct seq_file *m, void *v)
{
    int needComma = 0; // 用于控制逗号的添加
    int z_x, z_y, z_z, myteam, IsFiring, IsADS,FOV,bulletFireSpeed;
    Matrix_Add = LG_GetAddr(LG_GetAddr(libBase + 0x17c7b310) + 0x20) + 0x280;
    Uworld = LG_GetAddr(libBase + 0x17cb3018);

    
    Ulevel = LG_GetAddr(Uworld + 0x108);
    AActor = LG_GetAddr(Ulevel + 0xa8);
    AActorCount = LG_GetDword(Ulevel + 0xb0);
    AcknowledgedPawn = LG_GetAddr(LG_GetAddr(LG_GetAddr(LG_GetAddr(LG_GetAddr(LG_GetAddr(libBase + 0x17cb3018) + 0x1a0) + 0x48) + 0x0) + 0x38) + 0x3a8);
    ZSRootComponent = LG_GetAddr(AcknowledgedPawn + 0x188);
    memset(matrix, 0, 16);
    LG_Read(Matrix_Add, matrix, 16 * 4);
    seq_printf(m, "{\n");

    seq_printf(m, "\"Msg\": 123,\n");

    seq_printf(m, "\"Matrix\":[");
    for (j = 0; j < 15; j++)
    {
        seq_printf(m, "%d,", matrix[j]);
    }
    // 打印最后一个元素，但不添加逗号
    seq_printf(m, "%d", matrix[15]);
    seq_printf(m, "],");

    z_x = LG_GetDword(ZSRootComponent + 0x16c);
    z_y = LG_GetDword(ZSRootComponent + 0x16c + 0x4);
    z_z = LG_GetDword(ZSRootComponent + 0x16c + 0x8);
    
    myteam = LG_GetDword(LG_GetAddr(AcknowledgedPawn + 0xeb0) + 0x110);
    
    IsFiring = LG_GetDword(AcknowledgedPawn + 0x1e30);
    IsADS = LG_GetDword(AcknowledgedPawn + 0x14f8);
    FOV = 0;
    bulletFireSpeed = 0;

    seq_printf(m, "\"MyInfo\":[");
    seq_printf(m, "{\"X\":%d,\"Y\":%d,\"Z\":%d,\"TeamID\":%d,\"IsFiring\":%d,\"IsADS\":%d,\"FOV\":%d,\"bulletFireSpeed\":%d}",z_x,z_y,z_z,myteam,IsFiring,IsADS,FOV,bulletFireSpeed);
    seq_printf(m, "],");
    seq_printf(m, "\"Object\":[");

for (i = 0; i <= AActorCount; ++i)
{
    int d_x, d_y, d_z, TeamID, Health, NOTHealth, IsBot, State,UID;
    uintptr_t Objaddr, RootComponent;
    uintptr_t Mesh, MeshComponent, BoneArray;
    int meshtrans[11], headtrans[11], chesttrans[11], pelvistrans[11], lshtrans[11], rshtrans[11], lelbtrans[11], relbtrans[11], lwtrans[11], rwtrans[11], Llshtrans[11], Lrshtrans[11], Llelbtrans[11], Lrelbtrans[11], Llwtrans[11], Lrwtrans[11];
    UTF16 PlayerName[16] = {0};
    Objaddr = LG_GetAddr(AActor + 8 * i);
    if (Objaddr == 0)
        continue;
    if (AcknowledgedPawn == Objaddr)
        continue;


    IsBot = LG_GetDword(LG_GetAddr(Objaddr + 0xeb0) + 0x110);//人机判断

    State = LG_GetDword(Objaddr + 0x1368); // 状态

    TeamID = LG_GetDword(LG_GetAddr(Objaddr + 0xeb0) + 0x110);    // 队伍id
    Health = LG_GetDword(LG_GetAddr(LG_GetAddr(Objaddr + 0xea8) + 0x240) + 0x5c);    // 血量
    NOTHealth = LG_GetDword(Objaddr + 0xde0); // 最大血量
    UID = LG_GetDword(LG_GetAddr(Objaddr + 0xea8) + 0x210); // 最大血量

    RootComponent = LG_GetAddr(Objaddr + 0x188);
    d_x = LG_GetDword(RootComponent + 0x16c);
    d_y = LG_GetDword(RootComponent + 0x16c + 0x4);
    d_z = LG_GetDword(RootComponent + 0x16c + 0x8);

    Mesh = LG_GetAddr(Objaddr + 0x3d8);
    MeshComponent = Mesh + 0x210;
    BoneArray = LG_GetAddr(Mesh + 0x708);


	

    if (d_x == 0 || d_y == 0 || d_z == 0)
    {
        continue;
    }

    if (i < AActorCount)
    {
        if (needComma)
        {
            seq_printf(m, "\n,"); // 添加逗号和换行符
        }
        seq_printf(m, "{\"ID\":%d,\"X\":%d,\"Y\":%d,\"Z\":%d,\"TeamID\":%d,\"IsBot\":%d,\"Health\":%d,\"NOTHealth\":%d,\"State\":%d,",
                   i,
                   d_x,
                   d_y,
                   d_z,
                   TeamID,
                   IsBot,
                   Health,
                   NOTHealth,
                   State);
        
        LG_Read(LG_GetAddr(LG_GetAddr(Objaddr + 0x398) + 0x478), PlayerName, 28);
        
        seq_printf(m, "\"PlayerName\":[");
        for (k = 0; k < 15; k++)
        {
            seq_printf(m, "%hd,", PlayerName[k]);
        }
        seq_printf(m, "%hd", PlayerName[15]);
        seq_printf(m, "],");

        memset(meshtrans, 0, 11);
        LG_Read(MeshComponent, meshtrans, 11 * 4);
        seq_printf(m, "\"meshtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", meshtrans[k]);
        }
        seq_printf(m, "%d", meshtrans[10]);
        seq_printf(m, "],");

        memset(headtrans, 0, 11);
        LG_Read(BoneArray + 31 * 48, headtrans, 11 * 4);
        seq_printf(m, "\"headtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", headtrans[k]);
        }
        seq_printf(m, "%d", headtrans[10]);
        seq_printf(m, "],");

        memset(chesttrans, 0, 11);
        LG_Read(BoneArray + 30 * 48, chesttrans, 11 * 4);
        seq_printf(m, "\"chesttrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", chesttrans[k]);
        }
        seq_printf(m, "%d", chesttrans[10]);
        seq_printf(m, "],");

        memset(pelvistrans, 0, 11);
        LG_Read(BoneArray + 1 * 48, pelvistrans, 11 * 4);
        seq_printf(m, "\"pelvistrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", pelvistrans[k]);
        }
        seq_printf(m, "%d", pelvistrans[10]);
        seq_printf(m, "],");

        memset(lshtrans, 0, 11);
        LG_Read(BoneArray + 34 * 48, lshtrans, 11 * 4);
        seq_printf(m, "\"lshtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", lshtrans[k]);
        }
        seq_printf(m, "%d", lshtrans[10]);
        seq_printf(m, "],");

        memset(rshtrans, 0, 11);
        LG_Read(BoneArray + 6 * 48, rshtrans, 11 * 4);
        seq_printf(m, "\"rshtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", rshtrans[k]);
        }
        seq_printf(m, "%d", rshtrans[10]);
        seq_printf(m, "],");

        memset(lelbtrans, 0, 11);
        LG_Read(BoneArray + 35 * 48, lelbtrans, 11 * 4);
        seq_printf(m, "\"lelbtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", lelbtrans[k]);
        }
        seq_printf(m, "%d", lelbtrans[10]);
        seq_printf(m, "],");

        memset(relbtrans, 0, 11);
        LG_Read(BoneArray + 7 * 48, relbtrans, 11 * 4);
        seq_printf(m, "\"relbtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", relbtrans[k]);
        }
        seq_printf(m, "%d", relbtrans[10]);
        seq_printf(m, "],");

        memset(lwtrans, 0, 11);
        LG_Read(BoneArray + 36 * 48, lwtrans, 11 * 4);
        seq_printf(m, "\"lwtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", lwtrans[k]);
        }
        seq_printf(m, "%d", lwtrans[10]);
        seq_printf(m, "],");

        memset(rwtrans, 0, 11);
        LG_Read(BoneArray + 8 * 48, rwtrans, 11 * 4);
        seq_printf(m, "\"rwtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", rwtrans[k]);
        }
        seq_printf(m, "%d", rwtrans[10]);
        seq_printf(m, "],");

        memset(Llshtrans, 0, 11);
        LG_Read(BoneArray + 58 * 48, Llshtrans, 11 * 4);
        seq_printf(m, "\"Llshtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Llshtrans[k]);
        }
        seq_printf(m, "%d", Llshtrans[10]);
        seq_printf(m, "],");

        memset(Lrshtrans, 0, 11);
        LG_Read(BoneArray + 62 * 48, Lrshtrans, 11 * 4);
        seq_printf(m, "\"Lrshtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Lrshtrans[k]);
        }
        seq_printf(m, "%d", Lrshtrans[10]);
        seq_printf(m, "],");

        memset(Llelbtrans, 0, 11);
        LG_Read(BoneArray + 59 * 48, Llelbtrans, 11 * 4);
        seq_printf(m, "\"Llelbtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Llelbtrans[k]);
        }
        seq_printf(m, "%d", Llelbtrans[10]);
        seq_printf(m, "],");

        memset(Lrelbtrans, 0, 11);
        LG_Read(BoneArray + 63 * 48, Lrelbtrans, 11 * 4);
        seq_printf(m, "\"Lrelbtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Lrelbtrans[k]);
        }
        seq_printf(m, "%d", Lrelbtrans[10]);
        seq_printf(m, "],");

        memset(Llwtrans, 0, 11);
        LG_Read(BoneArray +  60 * 48, Llwtrans, 11 * 4);
        seq_printf(m, "\"Llwtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Llwtrans[k]);
        }
        seq_printf(m, "%d", Llwtrans[10]);
        seq_printf(m, "],");

        memset(Lrwtrans, 0, 11);
        LG_Read(BoneArray + 64 * 48, Lrwtrans, 11 * 4);
        seq_printf(m, "\"Lrwtrans\":[");
        for (k = 0; k < 10; k++)
        {
            seq_printf(m, "%d,", Lrwtrans[k]);
        }
        seq_printf(m, "%d", Lrwtrans[10]);
        seq_printf(m, "]");

        seq_printf(m, "}");
        needComma = 1; // 设置标志，表示后续需要逗号
    }
}
seq_printf(m, "]");
seq_printf(m, "}");

return 0;
}
static int proc_show(struct seq_file *m, void *v)
{
    if (LG_Pid > 0)
    {
        GameSJZ(m, v);
    }
    else
    {
        seq_printf(m, "Wait for the game to start");
    }
    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

// 在较新的内核版本中，使用proc_ops
static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// 在较旧的内核版本中，使用file_operations
// static const struct file_operations proc_fops = {
//     .owner = THIS_MODULE,
//     .open = proc_open,
//     .read = seq_read,
//     .llseek = seq_lseek,
//     .release = single_release,
// };

static int LG_thread_fn(void *data)
{
    struct task_struct *task;
    struct pid *vpid;

    while (!kthread_should_stop())
    {
        if (LG_Pid == 0)
        {
            for_each_process(task)
            {
                //printk(KERN_INFO "[LGHUB]Nmae:%s PID:%d UUID:%d\n",task->comm, task->pid,task->real_cred->uid);
                if (strstr(task->comm, "tmgp.dfm") != NULL)
                {
                    proc_create("Z_SJZ", S_IRUGO, NULL, &proc_fops);
                    LG_Pid = task->pid;
                    libBase = LG_GetLibBase();
                    break;
                }
            }
            
            // printk(KERN_INFO "[LGHUB]Nmae:%s PID:%d UUID:%d\n",task->comm, task->pid,task->real_cred->uid);
            // printk(KERN_INFO "[LGHUB]Base:%lx\n",libBase);
        }
        else
        {
            // 如果LG_Pid不为0，检查进程是否还在运行
            vpid = find_vpid(LG_Pid);
            if (pid_task(vpid, PIDTYPE_PID) == NULL)
            {
                // 进程已经结束，将LG_Pid设置为0
                printk(KERN_INFO "[LGHUB]Process %d ended\n", LG_Pid);
                LG_Pid = 0;
                remove_proc_entry("Z_SJZ", NULL);
            }
        }

        // 延时一段时间再次检查
        msleep(1000);
    }

    return 0;
}

static int __init LG_init(void)
{
    printk(KERN_ERR "[LGHUB]init!\n");
    LG_thread = kthread_run(LG_thread_fn, NULL, "LG_thread");
    if (IS_ERR(LG_thread))
    {
        printk(KERN_ERR "[LGHUB]Failed to create LG_thread\n");
        return PTR_ERR(LG_thread);
    }
    remove_proc_subtree("sched_debug", NULL); //移除/proc/sched_debug。
    remove_proc_entry("uevents_records", NULL); //移除/proc/uevents_records。
    list_del_init(&__this_module.list); //摘除链表，/proc/modules 中不可见。
   // kobject_del(&THIS_MODULE->mkobj.kobj); //摘除kobj，/sys/modules/中不可见。
    return 0;
}

static void __exit LG_exit(void)
{
    kthread_stop(LG_thread);
    remove_proc_entry("Z_SJZ", NULL);
    printk(KERN_INFO "[LGHUB]Goodbye, World!\n");
}

module_init(LG_init);
module_exit(LG_exit);
