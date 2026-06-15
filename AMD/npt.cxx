extern "C" {
#include <ntddk.h>
}

#include "npt.hxx"

constexpr uint64_t NPT_PRESENT = 1ull << 0;
constexpr uint64_t NPT_RW      = 1ull << 1;
constexpr uint64_t NPT_US      = 1ull << 2;
constexpr uint64_t NPT_PS      = 1ull << 7;

static void* AllocPage()
{
    PHYSICAL_ADDRESS lo{}, hi{};
    hi.QuadPart = MAXULONG64;
    void* p = MmAllocateContiguousMemorySpecifyCache(0x1000, lo, hi, lo, MmCached);
    if (p) RtlZeroMemory(p, 0x1000);
    return p;
}

bool BuildIdentityNpt(Npt* npt)
{
    npt->pml4 = static_cast<uint64_t*>(AllocPage());
    if (!npt->pml4) return false;

    npt->pml4_pa = MmGetPhysicalAddress(npt->pml4);

    
    
    
    
    for (uint64_t pml4i = 0; pml4i < 512; ++pml4i) {
        auto* pdpt = static_cast<uint64_t*>(AllocPage());
        if (!pdpt) {
            FreeNpt(npt);
            return false;
        }

        auto pdpt_pa = MmGetPhysicalAddress(pdpt).QuadPart;
        npt->pml4[pml4i] = static_cast<uint64_t>(pdpt_pa)
                          | NPT_PRESENT | NPT_RW | NPT_US;

        for (uint64_t pdpti = 0; pdpti < 512; ++pdpti) {
            uint64_t gpa = (pml4i << 39) | (pdpti << 30);
            pdpt[pdpti]  = gpa | NPT_PRESENT | NPT_RW | NPT_US | NPT_PS;
        }
    }

    return true;
}

void FreeNpt(Npt* npt)
{
    if (npt->pml4) {
        for (int i = 0; i < 512; ++i) {
            uint64_t entry = npt->pml4[i];
            if (entry & NPT_PRESENT) {
                PHYSICAL_ADDRESS pa{};
                pa.QuadPart = static_cast<LONGLONG>(entry & ~0xFFFull);
                void* pdpt = MmGetVirtualForPhysical(pa);
                if (pdpt) MmFreeContiguousMemory(pdpt);
            }
        }
        MmFreeContiguousMemory(npt->pml4);
        npt->pml4 = nullptr;
    }
    npt->pml4_pa.QuadPart = 0;
}
