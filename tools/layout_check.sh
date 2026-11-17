#!/bin/bash
# Layout check (layout_pad.c / smb_pad.c say why): builds the launcher and compares its link map with the one of the
# build that booted on the console. Every loadable input section after launcher.o must sit a multiple of 64 bytes
# from where it was there; if not, pad in layout_pad.c until this prints LAYOUT OK.
# usage (from Windows): wsl bash /mnt/d/ps2/ps2-launcher/tools/layout_check.sh
# After a new build boots on the console, set BASE / BASE_SIZE to its commit and its stripped launcher.elf size.
BASE=b74945d
BASE_SIZE=3628820
export PS2DEV=/usr/local/ps2dev PS2SDK=/usr/local/ps2dev/ps2sdk
export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2SDK/bin
cd "$(dirname "$0")/.." || exit 1
B=$HOME/.cache/nuld_layout/$BASE # the baseline: BASE checked out and built once (not /tmp: WSL empties it)
mkdir -p $B

# make in the current directory, then the same link again with a map: $1.map, $1.elf in $B
build() {
	make APP=launcher "${@:2}" > $B/$1.log 2>&1
	local link=$(grep -- '-o launcher.elf' $B/$1.log | tail -1)
	[ -n "$link" ] || { echo "$1: build failed"; grep -E 'error|Error' $B/$1.log | head; exit 1; }
	eval "${link/-o launcher.elf/-o $B/$1.elf} -Wl,-Map=$B/$1.map" || exit 1
}
# "<section> <object>#<n> <address>" for every non-empty input section of a map
sections() { awk '
	/^ [.A-Za-z_*]/ && NF == 1 { pend = $1; next }
	pend != "" { $0 = " " pend " " $0; pend = "" }
	/^ [.A-Za-z_*][^ ]* +0x[0-9a-f]+ +0x[0-9a-f]+ [^ ]/ { if (strtonum($3) == 0) next
		f = $4; sub(/^\/tmp\/cc[^.]*/, "lto", f); k = $1 " " f; c[k]++; print k "#" c[k], strtonum($2) }' "$1"; }

if [ ! -f $B/base.txt ]; then
	echo "building the baseline ($BASE)..."
	rm -rf $B/src && mkdir $B/src && git archive $BASE | tar -x -C $B/src || exit 1
	(cd $B/src && build base) || exit 1
	mips64r5900el-ps2-elf-strip --strip-all $B/base.elf
	[ "$(stat -c %s $B/base.elf)" = $BASE_SIZE ] || { echo "baseline is $(stat -c %s $B/base.elf) bytes, not $BASE_SIZE"; exit 1; }
	sections $B/base.map > $B/base.txt
fi
build new -W launcher.c -W layout_pad.c # -W: D: is exFAT, its clock skews against WSL's
make APP=launcher > /dev/null 2>&1      # strip, as a plain build leaves it
sections $B/new.map > $B/new.txt
# loadable sections only (the ELF is stripped); launcher.o and layout_pad.o are the ones that change
awk 'NR == FNR { a[$1 " " $2] = $3; next }
	$1 ~ /^\.(debug|pdr|comment|mdebug|gnu|reginfo|MIPS)/ || $2 ~ /^(launcher|layout_pad)\.o/ { next }
	{ k = $1 " " $2; if (!(k in a)) { miss++; next } d = $3 - a[k]; m = ((d % 64) + 64) % 64
	  n[$1 " moved by " d " (mod 64 = " m ")"]++; if (m) bad++ }
	END { for (k in n) print n[k], k; if (miss) print miss, "sections not in the baseline"
	      print bad ? "LAYOUT OFF: " bad " sections" : "LAYOUT OK"; exit bad != 0 }' $B/base.txt $B/new.txt | sort -k2,2 -k1,1rn
st=${PIPESTATUS[0]}
ls -l launcher.elf
exit $st
