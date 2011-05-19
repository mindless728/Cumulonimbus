/**
 * File: deadspace.c
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: Our kernel starts at 0x10000 and grows upwards as more is added
 * and this is a problem. The range 0xA0000-0x100000 is a reserved/legacy memory
 * range and is read-only in some portions. This was causing corruption of BSS
 * data in that spot. Try removing this from being linked into the kernel,
 * things WILL break.
 */

// lets make it 1 meg to be safe...
char deadspace[1024][1024];
