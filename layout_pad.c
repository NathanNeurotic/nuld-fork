// Layout keeper, with smb_pad.c (which says why): 8 bytes of code right after launcher.o, so every object and library
// after it lands a multiple of 64 bytes from where it was in the build that booted on the console.
// ponytail: tied to launcher.c's size today; re-check with a link map when launcher.c grows.
void layout_pad(void) {}
