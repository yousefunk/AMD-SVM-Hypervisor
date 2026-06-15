extern "C" {
#include <ntddk.h>
#include <intrin.h>
}

#include "vmcb.hxx"
#include "vmcb_setup.hxx"
#include "npt.hxx"
#include "../arch.hxx"

extern "C" {
    
    int               SvmLaunch(void* vcpu);
    unsigned __int64  HvVerify(void);
}

constexpr ULONG  TAG_HV     = 'VmcB';
constexpr SIZE_T PAGE_4K    = 0x1000;
constexpr SIZE_T IOPM_SIZE       = 0x3000;
constexpr SIZE_T MSRPM_SIZE      = 0x2000;
constexpr SIZE_T HOST_STACK_SIZE = 0x4000;

constexpr ULONG  CPUID_VENDOR       = 0x00000000;
constexpr ULONG  CPUID_EXT_FEATURES = 0x80000001;
constexpr ULONG  CPUID_SVM_FEATURES = 0x8000000A;

constexpr ULONG  MSR_VM_CR       = 0xC0010114;
constexpr ULONG  MSR_EFER        = 0xC0000080;
constexpr ULONG  MSR_VM_HSAVE_PA = 0xC0010117;
constexpr ULONG64 VM_CR_SVMDIS   = 1ull << 4;
constexpr ULONG64 EFER_SVME      = 1ull << 12;

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
static_assert(__alignof(VcpuContext) <= 8, "VcpuContext alignment");
static_assert(sizeof(void*) == 8 && sizeof(PHYSICAL_ADDRESS) == 8, "size sanity");

struct GlobalContext {
    ULONG        cpu_count;
    VcpuContext* vcpus;
    ULONG        asid_count;
    bool         np_supported;
    Npt          npt;
};

static GlobalContext g_ctx{};

static void* AllocContig(SIZE_T bytes)
{
    PHYSICAL_ADDRESS lo{}, hi{};
    hi.QuadPart = MAXULONG64;
    void* p = MmAllocateContiguousMemorySpecifyCache(bytes, lo, hi, lo, MmCached);
    if (p) RtlZeroMemory(p, bytes);
    return p;
}

static void FreeContig(void* p)
{
    if (p) MmFreeContiguousMemory(p);
}

static bool IsAmdCpu()
{
    int r[4]{};
    __cpuid(r, CPUID_VENDOR);
    return r[1] == 'htuA' && r[3] == 'itne' && r[2] == 'DMAc';
}

static bool HypervisorAlreadyPresent()
{
    int r[4]{};
    __cpuid(r, 1);
    
    if ((r[2] & (1u << 31)) == 0) return false;

    
    __cpuid(r, 0x40000000);
    return true;
}

static bool SvmeAlreadyEnabled()
{
    return (__readmsr(MSR_EFER) & EFER_SVME) != 0;
}

static bool CpuSupportsSvm()
{
    int r[4]{};
    __cpuid(r, CPUID_EXT_FEATURES);
    return (r[2] & (1u << 2)) != 0;
}

static bool SvmNotLockedByBios()
{
    return (__readmsr(MSR_VM_CR) & VM_CR_SVMDIS) == 0;
}

static void QuerySvmCaps(ULONG& asid_count, bool& np)
{
    int r[4]{};
    __cpuid(r, CPUID_SVM_FEATURES);
    asid_count = static_cast<ULONG>(r[1]);
    np = (r[3] & (1u << 0)) != 0;
}

static NTSTATUS AllocVcpu(VcpuContext* v)
{
    v->vmcb = static_cast<Vmcb*>(AllocContig(sizeof(Vmcb)));
    if (!v->vmcb) return STATUS_INSUFFICIENT_RESOURCES;
    v->vmcb_pa = MmGetPhysicalAddress(v->vmcb);

    v->host_save = AllocContig(PAGE_4K);
    if (!v->host_save) return STATUS_INSUFFICIENT_RESOURCES;
    v->host_save_pa = MmGetPhysicalAddress(v->host_save);

    v->iopm = AllocContig(IOPM_SIZE);
    if (!v->iopm) return STATUS_INSUFFICIENT_RESOURCES;
    v->iopm_pa = MmGetPhysicalAddress(v->iopm);

    v->msrpm = AllocContig(MSRPM_SIZE);
    if (!v->msrpm) return STATUS_INSUFFICIENT_RESOURCES;
    v->msrpm_pa = MmGetPhysicalAddress(v->msrpm);

    v->host_stack = AllocContig(HOST_STACK_SIZE);
    if (!v->host_stack) return STATUS_INSUFFICIENT_RESOURCES;
    v->host_stack_top = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(v->host_stack) + HOST_STACK_SIZE);

    return STATUS_SUCCESS;
}

static void FreeVcpu(VcpuContext* v)
{
    FreeContig(v->host_stack); v->host_stack = nullptr; v->host_stack_top = nullptr;
    FreeContig(v->msrpm);      v->msrpm = nullptr;
    FreeContig(v->iopm);       v->iopm = nullptr;
    FreeContig(v->host_save);  v->host_save = nullptr;
    FreeContig(v->vmcb);       v->vmcb = nullptr;
}

static void EnableSvmCurrentCpu(VcpuContext* v)
{
    __writemsr(MSR_EFER, __readmsr(MSR_EFER) | EFER_SVME);
    __writemsr(MSR_VM_HSAVE_PA, static_cast<ULONG64>(v->host_save_pa.QuadPart));
}

static KDEFERRED_ROUTINE EnableSvmDpc;
VOID EnableSvmDpc(PKDPC, PVOID, PVOID arg1, PVOID)
{
    ULONG idx = KeGetCurrentProcessorNumberEx(nullptr);
    VcpuContext* v = &g_ctx.vcpus[idx];

    EnableSvmCurrentCpu(v);
    BuildGuestVmcb(v->vmcb,
                   static_cast<uint64_t>(v->iopm_pa.QuadPart),
                   static_cast<uint64_t>(v->msrpm_pa.QuadPart),
                   static_cast<uint64_t>(g_ctx.npt.pml4_pa.QuadPart));

    int launch = SvmLaunch(v);

    
    
    
    
    unsigned __int64 reply = 0;
    __try {
        reply = HvVerify();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        reply = 0;
    }
    bool virtualized = (reply == 0x42766D4842766D48ull);

    KeSetEvent(static_cast<PKEVENT>(arg1), 0, FALSE);
}

static NTSTATUS EnableSvmAllCpus()
{
    for (ULONG i = 0; i < g_ctx.cpu_count; ++i) {
        KDPC   dpc;
        KEVENT done;
        PROCESSOR_NUMBER pn{};

        KeInitializeEvent(&done, NotificationEvent, FALSE);
        KeInitializeDpc(&dpc, EnableSvmDpc, nullptr);
        KeSetImportanceDpc(&dpc, HighImportance);

        NTSTATUS s = KeGetProcessorNumberFromIndex(i, &pn);
        if (!NT_SUCCESS(s)) return s;
        KeSetTargetProcessorDpcEx(&dpc, &pn);

        KeInsertQueueDpc(&dpc, &done, nullptr);
        KeWaitForSingleObject(&done, Executive, KernelMode, FALSE, nullptr);
    }
    return STATUS_SUCCESS;
}

static void TeardownAll()
{
    FreeNpt(&g_ctx.npt);
    if (g_ctx.vcpus) {
        for (ULONG i = 0; i < g_ctx.cpu_count; ++i) FreeVcpu(&g_ctx.vcpus[i]);
        ExFreePoolWithTag(g_ctx.vcpus, TAG_HV);
        g_ctx.vcpus = nullptr;
    }
}




extern "C"
NTSTATUS AmdHvInit()
{
    if (!CpuSupportsSvm()) {
        return STATUS_NOT_SUPPORTED;
    }
    if (!SvmNotLockedByBios()) {
        return STATUS_NOT_SUPPORTED;
    }
    if (HypervisorAlreadyPresent()) {
        return STATUS_NOT_SUPPORTED;
    }
    if (SvmeAlreadyEnabled()) {
        return STATUS_NOT_SUPPORTED;
    }

    QuerySvmCaps(g_ctx.asid_count, g_ctx.np_supported);

    g_ctx.cpu_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    g_ctx.vcpus = static_cast<VcpuContext*>(
        ExAllocatePool2(POOL_FLAG_NON_PAGED,
                        sizeof(VcpuContext) * g_ctx.cpu_count, TAG_HV));
    if (!g_ctx.vcpus) return STATUS_INSUFFICIENT_RESOURCES;

    for (ULONG i = 0; i < g_ctx.cpu_count; ++i) {
        NTSTATUS s = AllocVcpu(&g_ctx.vcpus[i]);
        if (!NT_SUCCESS(s)) {
            TeardownAll();
            return s;
        }
    }

    if (!BuildIdentityNpt(&g_ctx.npt)) {
        TeardownAll();
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    NTSTATUS s = EnableSvmAllCpus();
    if (!NT_SUCCESS(s)) {
        TeardownAll();
        return s;
    }

    return STATUS_SUCCESS;
}
