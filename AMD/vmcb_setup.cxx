extern "C" {
#include <ntddk.h>
#include <intrin.h>
}

#include "vmcb.hxx"
#include "vmcb_setup.hxx"
#include "../arch.hxx"

constexpr ULONG MSR_EFER           = 0xC0000080;
constexpr ULONG MSR_STAR           = 0xC0000081;
constexpr ULONG MSR_LSTAR          = 0xC0000082;
constexpr ULONG MSR_CSTAR          = 0xC0000083;
constexpr ULONG MSR_SFMASK         = 0xC0000084;
constexpr ULONG MSR_FS_BASE        = 0xC0000100;
constexpr ULONG MSR_GS_BASE        = 0xC0000101;
constexpr ULONG MSR_KERNEL_GS_BASE = 0xC0000102;
constexpr ULONG MSR_SYSENTER_CS    = 0x00000174;
constexpr ULONG MSR_SYSENTER_ESP   = 0x00000175;
constexpr ULONG MSR_SYSENTER_EIP   = 0x00000176;
constexpr ULONG MSR_DEBUGCTL       = 0x000001D9;
constexpr ULONG MSR_PAT            = 0x00000277;

constexpr uint64_t EFER_SVME_BIT   = 1ull << 12;

#pragma pack(push, 1)
struct Dtr {
    uint16_t limit;
    uint64_t base;
};

struct GdtEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  limit_high_flags;
    uint8_t  base_high;
};

struct SysGdtEntry {
    GdtEntry lo;
    uint32_t base_upper;
    uint32_t reserved;
};
#pragma pack(pop)

static uint16_t PackAttrib(const GdtEntry* e)
{
    return static_cast<uint16_t>(e->access)
         | (static_cast<uint16_t>(e->limit_high_flags & 0xF0) << 4);
}

static void FillSeg(VmcbSegment* out, uint16_t selector, uint64_t gdt_base)
{
    out->selector = selector;
    uint16_t idx = selector & 0xFFF8;
    if (idx == 0) {
        out->attrib = 0;
        out->limit  = 0;
        out->base   = 0;
        return;
    }
    auto* e = reinterpret_cast<GdtEntry*>(gdt_base + idx);
    out->limit  = static_cast<uint32_t>(e->limit_low)
                | (static_cast<uint32_t>(e->limit_high_flags & 0x0F) << 16);
    out->base   = static_cast<uint64_t>(e->base_low)
                | (static_cast<uint64_t>(e->base_mid)  << 16)
                | (static_cast<uint64_t>(e->base_high) << 24);
    out->attrib = PackAttrib(e);
}

static void FillSysSeg(VmcbSegment* out, uint16_t selector, uint64_t gdt_base)
{
    out->selector = selector;
    uint16_t idx = selector & 0xFFF8;
    if (idx == 0) {
        out->attrib = 0;
        out->limit  = 0;
        out->base   = 0;
        return;
    }
    auto* e = reinterpret_cast<SysGdtEntry*>(gdt_base + idx);
    out->limit  = static_cast<uint32_t>(e->lo.limit_low)
                | (static_cast<uint32_t>(e->lo.limit_high_flags & 0x0F) << 16);
    out->base   = static_cast<uint64_t>(e->lo.base_low)
                | (static_cast<uint64_t>(e->lo.base_mid)    << 16)
                | (static_cast<uint64_t>(e->lo.base_high)   << 24)
                | (static_cast<uint64_t>(e->base_upper)     << 32);
    out->attrib = PackAttrib(&e->lo);
}

void BuildGuestVmcb(Vmcb* vmcb, uint64_t iopm_pa, uint64_t msrpm_pa, uint64_t npt_pml4_pa)
{
    Dtr gdtr{}, idtr{};
    store_gdt(&gdtr);
    store_idt(&idtr);

    auto& s = vmcb->state;
    auto& c = vmcb->control;

    FillSeg(&s.cs, read_cs(), gdtr.base);
    FillSeg(&s.ss, read_ss(), gdtr.base);
    FillSeg(&s.ds, read_ds(), gdtr.base);
    FillSeg(&s.es, read_es(), gdtr.base);
    FillSeg(&s.fs, read_fs(), gdtr.base);
    FillSeg(&s.gs, read_gs(), gdtr.base);
    FillSysSeg(&s.tr,   read_tr(),   gdtr.base);
    FillSysSeg(&s.ldtr, read_ldtr(), gdtr.base);

    s.fs.base = __readmsr(MSR_FS_BASE);
    s.gs.base = __readmsr(MSR_GS_BASE);

    s.gdtr.selector = 0;
    s.gdtr.attrib   = 0;
    s.gdtr.limit    = gdtr.limit;
    s.gdtr.base     = gdtr.base;

    s.idtr.selector = 0;
    s.idtr.attrib   = 0;
    s.idtr.limit    = idtr.limit;
    s.idtr.base     = idtr.base;

    s.efer   = __readmsr(MSR_EFER) | EFER_SVME_BIT;
    s.cr0    = __readcr0();
    s.cr2    = __readcr2();
    s.cr3    = __readcr3();
    s.cr4    = __readcr4();
    s.rflags = __readeflags();
    s.dr6    = __readdr(6);
    s.dr7    = __readdr(7);
    s.cpl    = 0;

    s.rax    = 0;
    s.rip    = 0;
    s.rsp    = 0;

    s.star            = __readmsr(MSR_STAR);
    s.lstar           = __readmsr(MSR_LSTAR);
    s.cstar           = __readmsr(MSR_CSTAR);
    s.sfmask          = __readmsr(MSR_SFMASK);
    s.kernel_gs_base  = __readmsr(MSR_KERNEL_GS_BASE);
    s.sysenter_cs     = __readmsr(MSR_SYSENTER_CS);
    s.sysenter_esp    = __readmsr(MSR_SYSENTER_ESP);
    s.sysenter_eip    = __readmsr(MSR_SYSENTER_EIP);
    s.g_pat           = __readmsr(MSR_PAT);
    s.dbgctl          = __readmsr(MSR_DEBUGCTL);

    c.iopm_base_pa     = iopm_pa;
    c.msrpm_base_pa    = msrpm_pa;
    c.guest_asid       = 1;
    
    
    
    c.intercept_misc1 |= SVM_INTERCEPT1_SHUTDOWN;
    c.intercept_misc2 |= SVM_INTERCEPT2_VMRUN | SVM_INTERCEPT2_VMMCALL;
    
    c.tlb_control      = SVM_TLB_CTL_FLUSH_ALL;
    c.n_cr3            = npt_pml4_pa;
    c.misc_ctl        |= SVM_MISC_CTL_NP_ENABLE;
}
