#include <linux/init.h>
#include <linux/module.h>

static int __init fastafs_init(void)
{
	pr_debug("fastafs module loaded\n");
	
	return 0;
}

static void __exit fastafs_exit(void)
{
	pr_debug("fastafs module unloaded\n");
}



module_init(fastafs_init);
module_exit(fastafs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youri Hoogstate");
