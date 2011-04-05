/**
 * File: fpu.h
 * @author Benjamin Mayes
 * @description Declarations of functions wrapping FPU instructions for some 
 * common tasks.
 */

#ifndef FPU_H_
#define FPU_H_
#include <types.h>

// Register access functions
extern inline uint16_t get_fpu_status(void);
extern inline uint16_t get_fpu_control(void);

// mathematical functions
extern inline double abs( double x );
extern inline double sqrt( double x );
extern inline double rndint( double x );
extern inline double sin( double x );
extern inline double cos( double x );
extern inline double patan( double x, double y );
extern inline double log2( double x );
extern inline double logn( double x, double base );
extern inline double pow( double x, double y );

// The context for the FPU registers
typedef struct _fpu_context_t {
    uint16_t fpu_control_word;
    uint16_t unused1;
    uint16_t fpu_status_word;
    uint16_t unused2;
    uint16_t fpu_tag_word;
    uint16_t unused3;
    uint32_t fpu_instruction_pointer;
    uint16_t fpu_code_segment;
    uint16_t unused4;
    uint32_t fpu_last_instruction_operand_addr;
    uint16_t fpu_data_segment;
    uint16_t unused5;
    uint8_t st[8][10];
} fpu_context_t;

// Functions for saving and restoring fpu context
extern inline void _save_fpu_context( fpu_context_t *context );
extern inline void _load_fpu_context( fpu_context_t *context );
#endif
