#pragma once

extern "C" {
#include <ntddk.h>
}

#include <stdint.h>

struct Npt {
    uint64_t*        pml4;
    PHYSICAL_ADDRESS pml4_pa;
};

bool BuildIdentityNpt(Npt* npt);
void FreeNpt(Npt* npt);
