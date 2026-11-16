// Layout keeper (#6 SMB, console hang 2026-11-15). Linking libsmb2 among the other libraries moved them by 188 KB and
// the console froze at the end of the splash (PCSX2 did not); the same code with libsmb2 and smb.c linked LAST boots.
// So smb.c and libsmb2 go in libnuldsmb.a / -lsmb2 at the end of EE_LIBS, and this file keeps the objects the size they
// had in the build that booted (it is that build's smb.c stub, renamed, never called).
// ponytail: the real fault (some library sensitive to where it lands) is still there; find it before adding big code.
#include "smb.h"
const char *smb_pad_list(const char *srv, const char *share, const char *user, const char *pass, smb_add_fn add, void *ctx)
{ (void)srv, (void)share, (void)user, (void)pass, (void)add, (void)ctx; return "SMB off (bisect)"; }
int smb_pad_toml(const char *ndir, const char *ip, const char *mask, const char *gw, const char *srv, const char *user,
             const char *pass, const char *share)
{ (void)ndir, (void)ip, (void)mask, (void)gw, (void)srv, (void)user, (void)pass, (void)share; return 0; }
