#!/usr/bin/env bash
set -euo pipefail

# Rebuild & repackage NTwin as a .deb using DESTDIR + dpkg-deb.
# Run this from inside the NTwin source tree (the directory with configure).

PKG_NAME="ntwin"
PKG_RELEASE="4"          # produces version like 0.9.1-2
PREFIX="/usr"            # IMPORTANT: avoids /usr/local baked-in paths
STAGE_ROOT="/tmp/ntwin-root"
PKG_ROOT="/tmp/ntwin-pkg"

die() { echo "ERROR: $*" >&2; exit 1; }

# --- sanity checks ---
[[ -f "./configure" ]] || die "Run this from the NTwin source root (missing ./configure)."

ARCH="$(dpkg --print-architecture)"

# Best-effort version detection
detect_version() {
  # 1) Try twin-current.lsm (often includes "Version:")
  if [[ -f "twin-current.lsm" ]]; then
    local v
    v="$(awk -F: '/^[Vv]ersion[[:space:]]*:/ {gsub(/^[[:space:]]+|[[:space:]]+$/, "", $2); print $2; exit}' twin-current.lsm || true)"
    [[ -n "${v:-}" ]] && { echo "$v"; return; }
  fi

  # 2) Try docs/Changelog.txt first line patterns
  if [[ -f "Changelog.txt" ]]; then
    local v
    v="$(grep -Eo '([0-9]+\.){1,3}[0-9]+' Changelog.txt | head -n1 || true)"
    [[ -n "${v:-}" ]] && { echo "$v"; return; }
  fi

  # 3) Try configure.ac AC_INIT([...], [x.y.z])
  if [[ -f "configure.ac" ]]; then
    local v
    v="$(grep -Eo 'AC_INIT\([^,]+,\s*\[[0-9]+\.[0-9]+(\.[0-9]+)?\]' configure.ac \
        | grep -Eo '[0-9]+\.[0-9]+(\.[0-9]+)?' | head -n1 || true)"
    [[ -n "${v:-}" ]] && { echo "$v"; return; }
  fi

  # Fallback
  echo "0.9.1"
}

VERSION="$(detect_version)"
DEB_VERSION="${VERSION}-${PKG_RELEASE}"
OUT_DEB="/tmp/${PKG_NAME}_${DEB_VERSION}_${ARCH}.deb"

echo "== NTwin rebuild & .deb package =="
echo "Source:   $(pwd)"
echo "Version:  ${VERSION}"
echo "Release:  ${PKG_RELEASE}"
echo "Arch:     ${ARCH}"
echo "Prefix:   ${PREFIX}"
echo "Output:   ${OUT_DEB}"
echo

# --- Step 1: ensure the tree isn't polluted by root-owned build artifacts ---
# (This happens if you ever ran sudo make/install in-tree.)
if find . -path '*/.libs/*' -o -path '*/.deps/*' -o -name 'libtool' -o -name 'config.status' 2>/dev/null \
    | head -n1 >/dev/null; then
  echo ">> Fixing ownership (if needed) so the build can write its own files..."
  sudo chown -R "$USER":"$USER" . || true
fi

# --- Step 2: clean build outputs ---
echo ">> Cleaning build tree..."
make distclean >/dev/null 2>&1 || true

# --- Step 3: configure & build ---
echo ">> Configuring..."
./configure --prefix="${PREFIX}"

echo ">> Building..."
make -j"$(nproc)"

# --- Step 4: staged install (DESTDIR) ---
echo ">> Staging install to ${STAGE_ROOT} ..."
sudo rm -rf "${STAGE_ROOT}"
mkdir -p "${STAGE_ROOT}"
make install DESTDIR="${STAGE_ROOT}"

# --- Step 5: create .deb structure ---
echo ">> Creating package root at ${PKG_ROOT} ..."
sudo rm -rf "${PKG_ROOT}"
mkdir -p "${PKG_ROOT}"

# We configured with --prefix=/usr, so staged files should be under ${STAGE_ROOT}/usr/...
[[ -d "${STAGE_ROOT}/usr" ]] || die "Expected ${STAGE_ROOT}/usr to exist. Something is off with PREFIX/DESTDIR."

cp -a "${STAGE_ROOT}/usr" "${PKG_ROOT}/"

mkdir -p "${PKG_ROOT}/DEBIAN"

# Minimal control file. Add Depends later if you want to be fancy.
cat > "${PKG_ROOT}/DEBIAN/control" <<EOF
Package: ${PKG_NAME}
Version: ${DEB_VERSION}
Section: x11
Priority: optional
Architecture: ${ARCH}
Maintainer: Wing <you@example.com>
Description: Text-mode windowing environment (NTwin)
 Text-mode windowing system with overlapping windows, mouse support,
 and networked clients, running entirely in a terminal.
EOF

# Ensure ldconfig runs via the standard Debian trigger after install.
cat > "${PKG_ROOT}/DEBIAN/triggers" <<EOF
activate-noawait ldconfig
EOF

# Permissions: control files must be owned by root in the package; dpkg-deb can be picky.
echo ">> Fixing DEBIAN permissions..."
chmod 0755 "${PKG_ROOT}/DEBIAN"
chmod 0644 "${PKG_ROOT}/DEBIAN/control"
chmod 0644 "${PKG_ROOT}/DEBIAN/triggers"

# --- Step 6: build the .deb ---
echo ">> Building .deb..."
rm -f "${OUT_DEB}"
dpkg-deb --build "${PKG_ROOT}" "${OUT_DEB}"

echo
echo "✅ Built: ${OUT_DEB}"
echo "Install with:"
echo "  sudo dpkg -i ${OUT_DEB}"
echo "ldconfig will run automatically via dpkg trigger."
