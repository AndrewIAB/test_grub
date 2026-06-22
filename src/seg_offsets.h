/*
 * Segment selector value
 * No LDT bit because its obsolete with paging
 * RING is the requested ring level to enter.
 * Ring 3 code cannot directly enter ring 0 without a syscall or interrupt
 */
#define SEGM_SEL(INDEX, RING) ((uint16_t)(INDEX) | ((uint16_t)(RING) & 3))

#define SEG_R0D 8 * 1
#define SEG_R0C 8 * 2
#define SEG_R3D 8 * 3
#define SEG_R3C 8 * 4
