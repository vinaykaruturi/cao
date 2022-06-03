/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#ifndef _MACROS_H_
#include "apex_macros.h"
#endif

#ifndef _XXYZ_ISSUE_QUEUE_
#include "issue_queue.h"
#endif

#ifndef _XXYZ_REORDER_BUFFER_
#include "rob.h"
#endif

#ifndef _XXYZ_LOAD_STORE_QUEUE_
#include "lsq.h"
#endif

#ifndef _XXYZ_PHY_REG_
#include "physical_register.h"
#endif

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
} APEX_Instruction;


//btb entry
typedef struct btb_entry{
    int pc_value;
    int target_address;
    int is_taken;
    int is_valid;
    int is_predicted;
    int predicted_pc;
}btb_entry;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;
    int rs1;
    int phy_rs1;
    int phy_rs2;
    int rs2;
    int rd;
    int phy_rd; //physical register allocated from free physical list  
    int imm;
    int rs1_ready;
    int rs2_ready;
    int rs1_value;
    int rs2_value;
    int result_buffer;
    int memory_address;
    int has_insn;
    int is_physical_register_required;
    int is_src1_register_required;
    int is_src2_register_required;
    int is_memory_insn;
    int is_stage_stalled;
    int issue_queue_index;
    int memory_instruction_type;
    int fu;
    int rob_index;
    int lsq_index;
    int cycles;
    int positive_flag;
    int zero_flag;
    int need_to_flush;
    int insn_type;
    int pc_value_to_be_taken;


    load_store_queue_entry temp_lsq_entry;
    reorder_buffer_entry temp_rob_entry;
    issue_queue_entry temp_iq_entry;
    
} CPU_Stage;

////////ARCHECTURAL_REGISTER_FILE///////////////

typedef struct  architectural_register_content{
    int value;
    int zero_flag;
    int positive_flag;
} architectural_register_content;

typedef struct archictectural_register_file{
    architectural_register_content architectural_register_file[ARCHITECTURAL_REGISTERS_SIZE+1];
}archictectural_register_file;




/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                        /* Current program counter */
    int clock;                     /* Clock cycles elapsed */
    int insn_completed;            /* Instructions retired */
    int code_memory_size;          /* Number of instruction in the input file */
    APEX_Instruction *code_memory; /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int mri[ARCHITECTURAL_REGISTERS_SIZE+1];
    int mri_bkp[ARCHITECTURAL_REGISTERS_SIZE+1];
    int single_step;               /* Wait for user input after every cycle */
    int zero_flag;                 /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int positive_flag;
    int fetch_from_next_cycle;
    int is_branch_unresolved;

    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode_rename;
    CPU_Stage rename_dispatch;
    CPU_Stage queue_entry;
    CPU_Stage bu_fu;
    CPU_Stage int_fu;
    CPU_Stage mul1_fu;
    CPU_Stage mul2_fu;
    CPU_Stage mul3_fu;
    CPU_Stage mul4_fu;
    CPU_Stage process_iq;
    CPU_Stage int_writeback;
    CPU_Stage mul_writeback;
    CPU_Stage mem_writeback;
    CPU_Stage branch_writeback;
    CPU_Stage memory;
    CPU_Stage int_fwd;
    CPU_Stage mul_fwd;
    CPU_Stage bu_fwd;
    CPU_Stage rob_commit_writeback;
    CPU_Stage rob_commit;
    CPU_Stage memory_fwd;
    CPU_Stage writeback;

    btb_entry btb[200];
    btb_entry btb_bkp[200];

    physical_register_file prf;
    archictectural_register_file arf;
    free_physical_registers_queue free_prf_q;
    rename_table_mapping rnt;
    rename_table_mapping rnt_bkp;
    issue_queue_buffer iq;
    load_store_queue lsq;
    reorder_buffer rob;


} APEX_CPU;

APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
void push_information_to_fu(APEX_CPU *cpu, int index, int fu);
int  APEX_rob_commit(APEX_CPU *cpu);


void APEX_memory(APEX_CPU *cpu);
void APEX_int_fu(APEX_CPU *cpu);
void APEX_mul_fu_1(APEX_CPU *cpu);
void APEX_mul_fu_2(APEX_CPU *cpu);
void APEX_mul_fu_3(APEX_CPU *cpu);
void APEX_mul_fu_4(APEX_CPU *cpu);
void APEX_process_iq(APEX_CPU *cpu);
void create_rename_table_backup( APEX_CPU *cpu);
void create_mri_backup( APEX_CPU *cpu);
void update_rename_table_with_backup( APEX_CPU *cpu, int physical_register_address);
void set_mri_from_backup( APEX_CPU *cpu, int physical_register_address);
void flush_instructions(APEX_CPU *cpu, int rob_index);
int is_branch_instruction(int opcode);
int check_free_physical_register(APEX_CPU *cpu, int physical_register_address);
void revert_btb_from_backup(APEX_CPU *cpu, int index);
void create_btb_backup(APEX_CPU *cpu);
#endif

