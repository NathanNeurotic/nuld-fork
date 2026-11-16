#pragma once
// SMB game source (#6): the PS2 ISOs of an SMB2 share, listed with libsmb2 over the launcher's own network; the game
// reads the share itself through Neutrino's smb_ioman / smb_fhi (read-only: no virtual memory cards there).
typedef void (*smb_add_fn)(void *ctx, const char *path, const char *serial); // "DVD/x.iso", "SLUS_209.46" or ""
// Lists DVD/ and CD/ of \\srv\share, reading each ISO's SYSTEM.CNF. NULL, or why not (for the SELECT overlay)
const char *smb_list(const char *srv, const char *share, const char *user, const char *pass, smb_add_fn add, void *ctx);
// neutrino/config/bsd-smb.toml with this network and login (ministack, smb_ioman, smb_fhi take the same arguments)
int smb_toml(const char *ndir, const char *ip, const char *mask, const char *gw, const char *srv, const char *user,
             const char *pass, const char *share);
