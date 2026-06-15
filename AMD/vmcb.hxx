#pragma once













#include <stdint.h>
#include <stddef.h>

#pragma pack(push, 1)






enum : uint16_t {
    SVM_INTERCEPT_CR0 = 1u << 0,
    SVM_INTERCEPT_CR3 = 1u << 3,
    SVM_INTERCEPT_CR4 = 1u << 4,
    SVM_INTERCEPT_CR8 = 1u << 8,
};


enum : uint32_t {
    SVM_INTERCEPT1_INTR        = 1u << 0,
    SVM_INTERCEPT1_NMI         = 1u << 1,
    SVM_INTERCEPT1_SMI         = 1u << 2,
    SVM_INTERCEPT1_INIT        = 1u << 3,
    SVM_INTERCEPT1_VINTR       = 1u << 4,
    SVM_INTERCEPT1_CR0_SEL_WR  = 1u << 5,
    SVM_INTERCEPT1_IDTR_READ   = 1u << 6,
    SVM_INTERCEPT1_GDTR_READ   = 1u << 7,
    SVM_INTERCEPT1_LDTR_READ   = 1u << 8,
    SVM_INTERCEPT1_TR_READ     = 1u << 9,
    SVM_INTERCEPT1_IDTR_WRITE  = 1u << 10,
    SVM_INTERCEPT1_GDTR_WRITE  = 1u << 11,
    SVM_INTERCEPT1_LDTR_WRITE  = 1u << 12,
    SVM_INTERCEPT1_TR_WRITE    = 1u << 13,
    SVM_INTERCEPT1_RDTSC       = 1u << 14,
    SVM_INTERCEPT1_RDPMC       = 1u << 15,
    SVM_INTERCEPT1_PUSHF       = 1u << 16,
    SVM_INTERCEPT1_POPF        = 1u << 17,
    SVM_INTERCEPT1_CPUID       = 1u << 18,
    SVM_INTERCEPT1_RSM         = 1u << 19,
    SVM_INTERCEPT1_IRET        = 1u << 20,
    SVM_INTERCEPT1_INTN        = 1u << 21,
    SVM_INTERCEPT1_INVD        = 1u << 22,
    SVM_INTERCEPT1_PAUSE       = 1u << 23,
    SVM_INTERCEPT1_HLT         = 1u << 24,
    SVM_INTERCEPT1_INVLPG      = 1u << 25,
    SVM_INTERCEPT1_INVLPGA     = 1u << 26,
    SVM_INTERCEPT1_IOIO_PROT   = 1u << 27,
    SVM_INTERCEPT1_MSR_PROT    = 1u << 28,
    SVM_INTERCEPT1_TASK_SWITCH = 1u << 29,
    SVM_INTERCEPT1_FERR_FREEZE = 1u << 30,
    SVM_INTERCEPT1_SHUTDOWN    = 1u << 31,
};


enum : uint32_t {
    SVM_INTERCEPT2_VMRUN   = 1u << 0,
    SVM_INTERCEPT2_VMMCALL = 1u << 1,
    SVM_INTERCEPT2_VMLOAD  = 1u << 2,
    SVM_INTERCEPT2_VMSAVE  = 1u << 3,
    SVM_INTERCEPT2_STGI    = 1u << 4,
    SVM_INTERCEPT2_CLGI    = 1u << 5,
    SVM_INTERCEPT2_SKINIT  = 1u << 6,
    SVM_INTERCEPT2_RDTSCP  = 1u << 7,
    SVM_INTERCEPT2_ICEBP   = 1u << 8,
    SVM_INTERCEPT2_WBINVD  = 1u << 9,
    SVM_INTERCEPT2_MONITOR = 1u << 10,
    SVM_INTERCEPT2_MWAIT   = 1u << 11,
    SVM_INTERCEPT2_MWAIT_C = 1u << 12,
    SVM_INTERCEPT2_XSETBV  = 1u << 13,
    SVM_INTERCEPT2_RDPRU   = 1u << 14,
    SVM_INTERCEPT2_EFER_W  = 1u << 15,
};


enum : uint64_t {
    SVM_MISC_CTL_NP_ENABLE       = 1ull << 0,
    SVM_MISC_CTL_SEV_ENABLE      = 1ull << 1,
    SVM_MISC_CTL_SEV_ES_ENABLE   = 1ull << 2,
    SVM_MISC_CTL_GMET            = 1ull << 3,
    SVM_MISC_CTL_SSS_CHECK_EN    = 1ull << 4,
    SVM_MISC_CTL_VTE             = 1ull << 5,
    SVM_MISC_CTL_RO_GPT          = 1ull << 6,
    SVM_MISC_CTL_INVLPGB_TLBSYNC = 1ull << 7,
};


enum : uint8_t {
    SVM_TLB_CTL_DO_NOTHING        = 0x00,
    SVM_TLB_CTL_FLUSH_ALL         = 0x01,
    SVM_TLB_CTL_FLUSH_GUEST       = 0x03,
    SVM_TLB_CTL_FLUSH_GUEST_NONG  = 0x07,
};




struct VmcbControlArea {
    uint16_t intercept_cr_read;            
    uint16_t intercept_cr_write;           
    uint16_t intercept_dr_read;            
    uint16_t intercept_dr_write;           
    uint32_t intercept_exceptions;         
    uint32_t intercept_misc1;              
    uint32_t intercept_misc2;              
    uint32_t intercept_misc3;              
    uint8_t  reserved_018[0x03C - 0x018];  
    uint16_t pause_filter_threshold;       
    uint16_t pause_filter_count;           
    uint64_t iopm_base_pa;                 
    uint64_t msrpm_base_pa;                
    uint64_t tsc_offset;                   
    uint32_t guest_asid;                   
    uint8_t  tlb_control;                  
    uint8_t  erap_control;                 
    uint8_t  reserved_05e[2];              
    uint32_t int_ctl;                      
    uint32_t int_vector;                   
    uint32_t int_state;                    
    uint8_t  reserved_06c[4];              
    uint64_t exitcode;                     
    uint64_t exitinfo1;                    
    uint64_t exitinfo2;                    
    uint32_t exitintinfo;                  
    uint32_t exitintinfo_err;              
    uint64_t misc_ctl;                     
    uint64_t avic_apic_bar;                
    uint64_t ghcb_gpa;                     
    uint32_t eventinj;                     
    uint32_t eventinj_err;                 
    uint64_t n_cr3;                        
    uint64_t misc_ctl2;                    
    uint32_t vmcb_clean;                   
    uint32_t reserved_0c4;                 
    uint64_t nrip;                         
    uint8_t  num_bytes_fetched;            
    uint8_t  guest_instruction_bytes[15];  
    uint64_t avic_apic_backing_page_ptr;   
    uint8_t  reserved_0e8[8];              
    uint64_t avic_logical_table_ptr;       
    uint64_t avic_physical_table_ptr;      
    uint8_t  reserved_100[8];              
    uint64_t vmsa_ptr;                     
    uint8_t  reserved_110[16];             
    uint16_t bus_lock_counter;             
    uint8_t  reserved_122[22];             
    uint64_t allowed_sev_features;         
    uint64_t guest_sev_features;           
    uint8_t  reserved_148[0x400 - 0x148];  
};

static_assert(sizeof(VmcbControlArea) == 0x400, "VMCB control area must be 1024 bytes");




struct VmcbSegment {
    uint16_t selector;
    uint16_t attrib;   
    uint32_t limit;
    uint64_t base;
};
static_assert(sizeof(VmcbSegment) == 16, "VmcbSegment must be 16 bytes");






struct VmcbStateSaveArea {
    VmcbSegment es;                 
    VmcbSegment cs;                 
    VmcbSegment ss;                 
    VmcbSegment ds;                 
    VmcbSegment fs;                 
    VmcbSegment gs;                 
    VmcbSegment gdtr;               
    VmcbSegment ldtr;               
    VmcbSegment idtr;               
    VmcbSegment tr;                 
    uint8_t     reserved_0a0[42];   
    uint8_t     vmpl;               
    uint8_t     cpl;                
    uint8_t     reserved_0cc[4];    
    uint64_t    efer;               
    uint8_t     reserved_0d8[112];  
    uint64_t    cr4;                
    uint64_t    cr3;                
    uint64_t    cr0;                
    uint64_t    dr7;                
    uint64_t    dr6;                
    uint64_t    rflags;             
    uint64_t    rip;                
    uint8_t     reserved_180[88];   
    uint64_t    rsp;                
    uint64_t    s_cet;              
    uint64_t    ssp;                
    uint64_t    isst_addr;          
    uint64_t    rax;                
    uint64_t    star;               
    uint64_t    lstar;              
    uint64_t    cstar;              
    uint64_t    sfmask;             
    uint64_t    kernel_gs_base;     
    uint64_t    sysenter_cs;        
    uint64_t    sysenter_esp;       
    uint64_t    sysenter_eip;       
    uint64_t    cr2;                
    uint8_t     reserved_248[32];   
    uint64_t    g_pat;              
    uint64_t    dbgctl;             
    uint64_t    br_from;            
    uint64_t    br_to;              
    uint64_t    last_excp_from;     
    uint64_t    last_excp_to;       
    uint8_t     reserved_298[72];   
    uint64_t    spec_ctrl;          
    
    
    uint8_t     reserved_tail[0xC00 - 0x2E8];
};

static_assert(sizeof(VmcbStateSaveArea) == 0xC00,
              "VMCB state save area block (incl. tail padding) must fill page");


static_assert(offsetof(VmcbStateSaveArea, spec_ctrl) + sizeof(uint64_t) == 744,
              "Defined state-save portion must be 744 bytes");




struct alignas(4096) Vmcb {
    VmcbControlArea   control;     
    VmcbStateSaveArea state;       
};

static_assert(sizeof(Vmcb) == 0x1000, "VMCB must be exactly 4096 bytes");


static_assert(offsetof(VmcbControlArea, intercept_misc1)   == 0x00C, "");
static_assert(offsetof(VmcbControlArea, pause_filter_threshold) == 0x03C, "");
static_assert(offsetof(VmcbControlArea, iopm_base_pa)      == 0x040, "");
static_assert(offsetof(VmcbControlArea, msrpm_base_pa)     == 0x048, "");
static_assert(offsetof(VmcbControlArea, guest_asid)        == 0x058, "");
static_assert(offsetof(VmcbControlArea, tlb_control)       == 0x05C, "");
static_assert(offsetof(VmcbControlArea, int_ctl)           == 0x060, "");
static_assert(offsetof(VmcbControlArea, exitcode)          == 0x070, "");
static_assert(offsetof(VmcbControlArea, exitintinfo)       == 0x088, "");
static_assert(offsetof(VmcbControlArea, misc_ctl)          == 0x090, "");
static_assert(offsetof(VmcbControlArea, ghcb_gpa)          == 0x0A0, "");
static_assert(offsetof(VmcbControlArea, eventinj)          == 0x0A8, "");
static_assert(offsetof(VmcbControlArea, n_cr3)             == 0x0B0, "");
static_assert(offsetof(VmcbControlArea, vmcb_clean)        == 0x0C0, "");
static_assert(offsetof(VmcbControlArea, nrip)              == 0x0C8, "");
static_assert(offsetof(VmcbControlArea, avic_apic_backing_page_ptr) == 0x0E0, "");
static_assert(offsetof(VmcbControlArea, vmsa_ptr)          == 0x108, "");
static_assert(offsetof(VmcbControlArea, bus_lock_counter)  == 0x120, "");
static_assert(offsetof(VmcbControlArea, guest_sev_features) == 0x140, "");

static_assert(offsetof(VmcbStateSaveArea, cpl)             == 0x0CB, "");
static_assert(offsetof(VmcbStateSaveArea, efer)            == 0x0D0, "");
static_assert(offsetof(VmcbStateSaveArea, cr4)             == 0x148, "");
static_assert(offsetof(VmcbStateSaveArea, cr0)             == 0x158, "");
static_assert(offsetof(VmcbStateSaveArea, rip)             == 0x178, "");
static_assert(offsetof(VmcbStateSaveArea, rsp)             == 0x1D8, "");
static_assert(offsetof(VmcbStateSaveArea, rax)             == 0x1F8, "");
static_assert(offsetof(VmcbStateSaveArea, cr2)             == 0x240, "");
static_assert(offsetof(VmcbStateSaveArea, g_pat)           == 0x268, "");
static_assert(offsetof(VmcbStateSaveArea, spec_ctrl)       == 0x2E0, "");

static_assert(offsetof(Vmcb, state) == 0x400, "State save area must begin at 0x400");

#pragma pack(pop)
