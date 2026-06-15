extern "C" {
#include <ntddk.h>
#include <intrin.h>
}

#include "vmcb.hxx"
#include "vmexit.hxx"

struct VcpuContext {
    Vmcb*            vmcb;
    PHYSICAL_ADDRESS vmcb_pa;
    void*            host_save;
    PHYSICAL_ADDRESS host_save_pa;
    void*            iopm;
    PHYSICAL_ADDRESS iopm_pa;
    void*            msrpm;
    PHYSICAL_ADDRESS msrpm_pa;
    void*            host_stack;
    void*            host_stack_top;
};

static void InjectUd(Vmcb* vmcb)
{
    auto& c = vmcb->control;
    c.eventinj     = 6u | (3u << 8) | (1u << 31);
    c.eventinj_err = 0;
}

static bool HandleVmmcall(Vmcb* vmcb, GuestRegisters* r)
{
    
    
    
    
    if (r->rcx == HV_VMMCALL_MAGIC) {
        vmcb->state.rax = HV_VMMCALL_REPLY;
        return true;
    }
    return false;
}

extern "C" int HandleVmexit(VcpuContext* v, GuestRegisters* r)
{
    auto& c = v->vmcb->control;
    auto& s = v->vmcb->state;

    switch (c.exitcode) {
    case VMEXIT_VMMCALL:
        if (HandleVmmcall(v->vmcb, r)) {
            s.rip = c.nrip;
        } else {
            InjectUd(v->vmcb);
        }
        break;

    case VMEXIT_VMRUN:
    case VMEXIT_VMLOAD:
    case VMEXIT_VMSAVE:
    case VMEXIT_STGI:
    case VMEXIT_CLGI:
    case VMEXIT_SKINIT:
        InjectUd(v->vmcb);
        break;

    case VMEXIT_NPF:
        
        
        
        s.rip = c.nrip;
        break;

    case VMEXIT_INVALID:
        
        for (;;) { _disable(); __halt(); }

    case VMEXIT_SHUTDOWN:
        
        for (;;) { _disable(); __halt(); }

    default:
        s.rip = c.nrip;
        break;
    }

    return 0;
}
