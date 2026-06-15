#pragma once

extern "C" {
    unsigned short read_cs(void);
    unsigned short read_ss(void);
    unsigned short read_ds(void);
    unsigned short read_es(void);
    unsigned short read_fs(void);
    unsigned short read_gs(void);
    unsigned short read_tr(void);
    unsigned short read_ldtr(void);
    void store_gdt(void* out);
    void store_idt(void* out);
}
