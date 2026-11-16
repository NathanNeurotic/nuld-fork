// SMB game source (#6). Listing: libsmb2 (ps2sdk ports) on the launcher's lwIP, in the loader thread like the server
// catalog, so covers keep working. Playing: Neutrino's own smb_ioman / smb_fhi (upstream 1310a9a, usmb2 on its tiny
// TCP/IP stack over smap + ministack), configured by the module arguments in config/bsd-smb.toml (Neutrino README
// "SMB"): the launcher writes that file before each SMB game, as it keeps bsd-udpfs.toml's ip= (sources.c).
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include "smb.h"
#include "iso.h"
#include "lang.h"

int gethostname(char *name, size_t len) // libsmb2's NTLM workstation name; ps2sdk has no gethostname
{
	snprintf(name, len, "NULD");
	return 0;
}

typedef struct { struct smb2_context *smb2; struct smb2fh *fh; } smb_file;

static int smb_read(void *ctx, unsigned lba, void *buf, unsigned n) // iso_read_fn
{
	smb_file *f = ctx;
	for (unsigned done = 0; done < n;) {
		int r = smb2_pread(f->smb2, f->fh, (uint8_t *)buf + done, n - done, (uint64_t)lba * 2048 + done);
		if (r <= 0) return 0;
		done += r;
	}
	return 1;
}

static int has_iso(const char *name)
{
	int n = strlen(name);
	return n > 4 && !strcasecmp(name + n - 4, ".iso");
}

const char *smb_list(const char *srv, const char *share, const char *user, const char *pass, smb_add_fn add, void *ctx)
{
	static char why[128];
	struct smb2_context *smb2 = smb2_init_context();
	if (!smb2) return L("Sin memoria para SMB", "Not enough memory for SMB");
	smb2_set_user(smb2, *user ? user : "guest");
	smb2_set_password(smb2, pass);
	smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
	// SMB 3.0.2, not 3.1.1: libsmb2 6.1 signs 3.1.1's tree connect even for a guest session, which has no session key,
	// and Samba answers ACCESS_DENIED (seen with nuld's guest share); 3.0.2 signs only when the server requires it
	smb2_set_version(smb2, SMB2_VERSION_0302);
	smb2_set_timeout(smb2, 8); // ponytail: a silent server holds the loader thread this long per request
	if (smb2_connect_share(smb2, srv, share, *user ? user : "guest") < 0) {
		snprintf(why, sizeof(why), L("No se conectó a \\\\%s\\%s: %s", "Could not connect to \\\\%s\\%s: %s"), srv, share, smb2_get_error(smb2));
		smb2_destroy_context(smb2);
		return why;
	}
	static const char *dirs[2] = {"DVD", "CD"};
	char path[160], raw[16];
	for (int k = 0; k < 2; k++) {
		struct smb2dir *d = smb2_opendir(smb2, dirs[k]);
		struct smb2dirent *e;
		while (d && (e = smb2_readdir(smb2, d))) {
			if (!has_iso(e->name) || snprintf(path, sizeof(path), "%s/%s", dirs[k], e->name) >= (int)sizeof(path)) continue;
			smb_file f = {smb2, smb2_open(smb2, path, O_RDONLY)};
			int ok = f.fh && iso_serial(smb_read, &f, 0, raw); // SYSTEM.CNF; the OPL name after
			if (f.fh) smb2_close(smb2, f.fh);
			if (!ok) ok = name_serial(e->name, raw);
			add(ctx, path, ok ? raw : "");
		}
		if (d) smb2_closedir(smb2, d);
	}
	smb2_disconnect_share(smb2);
	smb2_destroy_context(smb2);
	return NULL;
}

int smb_toml(const char *ndir, const char *ip, const char *mask, const char *gw, const char *srv, const char *user,
             const char *pass, const char *share)
{
	char path[96], args[256];
	// ponytail: a '"' in the password would end the TOML string; such a password does not work here
	snprintf(args, sizeof(args), "\"ip=%s\", \"mask=%s\", \"gw=%s\", \"srv=%s\", \"user=%s\", \"pass=%s\", \"share=%s\"",
	         ip, *mask ? mask : "255.255.255.0", *gw ? gw : "0.0.0.0", srv, *user ? user : "guest", pass, share);
	snprintf(path, sizeof(path), "%s/config/bsd-smb.toml", ndir);
	FILE *f = fopen(path, "wb");
	if (!f) return 0;
	fprintf(f, "# Written by NULD before each SMB game, from config.ini [smb] and [red]\n"
	           "name = \"SMB driver\"\ndepends = [\"i_dev9_hidden\"]\n\n"
	           "[[module]]\nfile = \"iomanX.irx\"\nenv = [\"LE\"]\n"
	           "[[module]]\nfile = \"fileXio.irx\"\nenv = [\"LE\"]\n"
	           "[[module]]\nfile = \"smap.irx\"\nenv = [\"LE\", \"EE\"]\n"
	           "[[module]]\nfile = \"ministack.irx\"\nargs = [\"ip=%s\"]\nenv = [\"LE\", \"EE\"]\n"
	           "[[module]]\nfile = \"smb_ioman.irx\"\nargs = [%s]\nenv = [\"LE\"]\n"
	           "[[module]]\nfile = \"smb_fhi.irx\"\nargs = [%s]\nenv = [\"EE\"]\n", ip, args, args);
	return fclose(f) == 0;
}
