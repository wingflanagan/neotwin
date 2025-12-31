/*
 *  term_backend.cpp  -- terminal backend preference handling
 */

#include "term_backend.h"

#include "log.h"
#include "stl/chars.h"

#include <ctype.h>
#include <stdlib.h>

term_backend_pref TermBackendPref = TERM_BACKEND_PREF_VTERM;

static bool term_backend_locked = false;

static bool EqualsIgnoreCase(const char *a, const char *b) {
  if (!a || !b) {
    return false;
  }
  while (*a && *b) {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
      return false;
    }
    a++;
    b++;
  }
  return *a == '\0' && *b == '\0';
}

static bool ParseTermBackendPref(const char *value, term_backend_pref *out) {
  if (!value || !*value || !out) {
    return false;
  }
  if (EqualsIgnoreCase(value, "auto") || EqualsIgnoreCase(value, "default")) {
    *out = TERM_BACKEND_PREF_AUTO;
    return true;
  }
  if (EqualsIgnoreCase(value, "vterm") || EqualsIgnoreCase(value, "xterm") ||
      EqualsIgnoreCase(value, "xterm-256color")) {
    *out = TERM_BACKEND_PREF_VTERM;
    return true;
  }
  if (EqualsIgnoreCase(value, "legacy") || EqualsIgnoreCase(value, "linux")) {
    *out = TERM_BACKEND_PREF_LEGACY;
    return true;
  }
  return false;
}

bool TermBackendApplyArg(const char *value) {
  term_backend_pref pref = TERM_BACKEND_PREF_AUTO;
  if (!ParseTermBackendPref(value, &pref)) {
    return false;
  }
  TermBackendPref = pref;
  term_backend_locked = true;
  return true;
}

void TermBackendApplyEnvIfUnset(void) {
  if (term_backend_locked) {
    return;
  }
  const char *value = getenv("NTWIN_TERM_BACKEND");
  if (!value || !*value) {
    return;
  }
  term_backend_pref pref = TERM_BACKEND_PREF_AUTO;
  if (!ParseTermBackendPref(value, &pref)) {
    log(WARNING) << "twin: ignoring invalid NTWIN_TERM_BACKEND="
                 << Chars::from_c(value) << "\n";
    return;
  }
  TermBackendPref = pref;
}

term_backend TermBackendDecide(dat scrollbacklines) {
  if (scrollbacklines <= 0) {
    return TERM_BACKEND_LEGACY;
  }
  if (TermBackendPref == TERM_BACKEND_PREF_LEGACY) {
    return TERM_BACKEND_LEGACY;
  }
  return TERM_BACKEND_VTERM;
}

const char *TermBackendName(term_backend backend) {
  switch (backend) {
  case TERM_BACKEND_LEGACY:
    return "legacy";
  case TERM_BACKEND_VTERM:
    return "vterm";
  default:
    return "unset";
  }
}

const char *TermBackendPrefName(term_backend_pref pref) {
  switch (pref) {
  case TERM_BACKEND_PREF_AUTO:
    return "auto";
  case TERM_BACKEND_PREF_VTERM:
    return "vterm";
  case TERM_BACKEND_PREF_LEGACY:
    return "legacy";
  default:
    return "auto";
  }
}

const char *TermBackendTermEnv(term_backend backend) {
  return backend == TERM_BACKEND_LEGACY ? "linux" : "xterm-256color";
}
