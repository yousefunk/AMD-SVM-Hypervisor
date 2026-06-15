#pragma once

#include <stdint.h>

struct Vmcb;

void BuildGuestVmcb(Vmcb* vmcb, uint64_t iopm_pa, uint64_t msrpm_pa, uint64_t npt_pml4_pa);
