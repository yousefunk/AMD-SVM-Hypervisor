#pragma once

#include <stdint.h>

struct VcpuContext;

struct GuestRegisters {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
};

constexpr uint64_t VMEXIT_CPUID    = 0x72;
constexpr uint64_t VMEXIT_MSR      = 0x7C;
constexpr uint64_t VMEXIT_VMRUN    = 0x80;
constexpr uint64_t VMEXIT_VMMCALL  = 0x81;
constexpr uint64_t VMEXIT_VMLOAD   = 0x82;
constexpr uint64_t VMEXIT_VMSAVE   = 0x83;
constexpr uint64_t VMEXIT_STGI     = 0x84;
constexpr uint64_t VMEXIT_CLGI     = 0x85;
constexpr uint64_t VMEXIT_SKINIT   = 0x86;
constexpr uint64_t VMEXIT_SHUTDOWN = 0x7F;
constexpr uint64_t VMEXIT_NPF      = 0x400;
constexpr uint64_t VMEXIT_INVALID  = static_cast<uint64_t>(-1);

constexpr uint32_t HV_SIGNATURE_EBX = 'BvmH';




constexpr uint64_t HV_VMMCALL_MAGIC = 0xC0FFEED00DBEEFull;
constexpr uint64_t HV_VMMCALL_REPLY = 0x42766D4842766D48ull;  

extern "C" int HandleVmexit(VcpuContext* v, GuestRegisters* regs);
