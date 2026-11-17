// Layout keeper, with smb_pad.c (which says why): 8 bytes of code right after launcher.o, so every object and library
// after it lands a multiple of 64 bytes from where it was in the build that booted on the console.
// ponytail: tied to launcher.c's size today; run tools/layout_check.sh when launcher.c grows.
void layout_pad(void) {}
// [juegos] logo: launcher.o's code grew by 88 bytes; 40 more make it 128
__asm__(".text\n.space 40\n");
// and its strings (the config.ini template) by 656: a 56-byte string more, for what follows the strings in .rodata
// (.rodata.cst8, .eh_frame). Checked with the link maps of both builds: every section after launcher.o moved by 128, 832 or 896
__asm__(".section .rodata.str1.8,\"aMS\",@progbits,1\n.align 3\n.space 55, 0x23\n.byte 0\n.text\n");
