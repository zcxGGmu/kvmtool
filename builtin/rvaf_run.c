#include <stdio.h>
#include <string.h>

#include <linux/err.h>

#include <kvm/util.h>
#include <kvm/rvaf_run.h>
#include <kvm/kvm.h>


static struct kvm *
kvm_cmd_run_init(int argc, const char **argv)
{
    struct kvm *kvm = calloc(1, sizeof(*kvm));

    kvm->kvm_fd = -1;
    kvm->vm_fd = -1;
    
    return kvm;
}

static int
kvm_cmd_run_work(struct kvm *kvm)
{
    return 0;
}

static void
kvm_cmd_run_exit(struct kvm *kvm, int guest_ret)
{
    if (guest_ret == 0)
        pr_info("KVM session ended noramlly.")
}

int kvm_cmd_run(int argc, const char **argv, const char *prefix)
{
    int ret = -EFAULT;
    struct kvm *kvm;

    // fill kvm by parse user options
    kvm = kvm_cmd_run_init(argc, argv);

    // vcpu run loop
    ret = kvm_cmd_run_work(kvm);

    kvm_cmd_run_exit(kvm, ret);

    return ret;
}
