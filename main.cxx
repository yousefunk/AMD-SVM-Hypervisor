extern "C" {
#include <ntddk.h>
}

#include "Amd/amd_init.hxx"

extern "C"
NTSTATUS DriverEntry(PVOID, PVOID)
{
    return AmdHvInit();
}
