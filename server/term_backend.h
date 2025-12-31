#ifndef TWIN_TERM_BACKEND_H
#define TWIN_TERM_BACKEND_H

#include <Tw/datatypes.h>

enum term_backend {
  TERM_BACKEND_UNSET = 0,
  TERM_BACKEND_LEGACY,
  TERM_BACKEND_VTERM,
};

enum term_backend_pref {
  TERM_BACKEND_PREF_AUTO = 0,
  TERM_BACKEND_PREF_VTERM,
  TERM_BACKEND_PREF_LEGACY,
};

extern term_backend_pref TermBackendPref;

bool TermBackendApplyArg(const char *value);
void TermBackendApplyEnvIfUnset(void);

term_backend TermBackendDecide(dat scrollbacklines);

const char *TermBackendName(term_backend backend);
const char *TermBackendPrefName(term_backend_pref pref);
const char *TermBackendTermEnv(term_backend backend);

#endif /* TWIN_TERM_BACKEND_H */
