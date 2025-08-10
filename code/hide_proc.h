#ifndef _HIDE_PROC_H
#define _HIDE_PROC_H

#include <linux/ioctl.h>

#define HIDE_PROC_DEVICE_NAME "hide_proc"
#define HIDE_PROC_DEVICE_PATH "/dev/hide_proc"

#define HIDE_PROC_MAGIC 'h'

#define HIDE_PROC_IOCTL_HIDE _IOW(HIDE_PROC_MAGIC, 1, int)
#define HIDE_PROC_IOCTL_UNHIDE _IOW(HIDE_PROC_MAGIC, 2, int)
#define HIDE_PROC_IOCTL_HIDE_SELF _IO(HIDE_PROC_MAGIC, 3)
#define HIDE_PROC_IOCTL_UNHIDE_SELF _IO(HIDE_PROC_MAGIC, 4)

#ifdef __KERNEL__
#include <linux/sched.h>
#include <linux/list.h>

struct hidden_proc {
    pid_t pid;
    struct task_struct *task;
    struct list_head list;
};

void hide_task(struct task_struct *task);
void unhide_task(struct task_struct *task);
#endif

#endif // _HIDE_PROC_H
