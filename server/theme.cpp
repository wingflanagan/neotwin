/*
 *  theme.cpp  --  theme config loader
 *
 *  Copyright (C) 2025
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "theme.h"

#include "alloc.h"
#include "data.h"
#include "log.h"
#include "util.h"

#include "stl/chars.h"
#include "stl/utf8.h"

#include <Tutf/Tutf.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

namespace {

const char kThemeFileName[] = "twtheme";
const size_t kMaxLine = 4096;
const size_t kMaxTokens = 32;

bool EqualsIgnoreCase(const char *a, const char *b) {
  if (!a || !b) {
    return false;
  }
  while (*a && *b) {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
      return false;
    }
    ++a;
    ++b;
  }
  return *a == '\0' && *b == '\0';
}

bool HasControlBytes(const char *text) {
  const unsigned char *p = reinterpret_cast<const unsigned char *>(text);
  while (*p) {
    if (*p < 0x20 || *p == 0x7F) {
      return true;
    }
    ++p;
  }
  return false;
}

bool DecodeUtf8Runes(const char *text, trune *out, size_t out_len, size_t *out_count) {
  Chars remaining = Chars::from_c(text);
  size_t count = 0;
  while (remaining.size() > 0) {
    Utf8 u;
    Chars next;
    if (!u.parse(remaining, &next)) {
      return false;
    }
    if (count < out_len) {
      out[count++] = u.rune();
    }
    remaining = next;
  }
  if (out_count) {
    *out_count = count;
  }
  return true;
}

size_t DecodeRunes(const char *text, trune *out, size_t out_len) {
  if (!text || !*text || out_len == 0) {
    return 0;
  }
  size_t count = 0;
  if (!HasControlBytes(text) && DecodeUtf8Runes(text, out, out_len, &count)) {
    return count;
  }
  const unsigned char *p = reinterpret_cast<const unsigned char *>(text);
  while (*p && count < out_len) {
    out[count++] = Tutf_CP437_to_UTF_32[*p++];
  }
  return count;
}

void StripComment(char *line) {
  bool in_quote = false;
  for (char *p = line; *p; ++p) {
    if (*p == '"' && (p == line || p[-1] != '\\')) {
      in_quote = !in_quote;
    }
    if (*p == '#' && !in_quote) {
      *p = '\0';
      return;
    }
  }
}

char *TrimLeft(char *s) {
  while (*s && isspace((unsigned char)*s)) {
    ++s;
  }
  return s;
}

void TrimRight(char *s) {
  size_t len = strlen(s);
  while (len > 0 && isspace((unsigned char)s[len - 1])) {
    s[--len] = '\0';
  }
}

#define IS_OCTDIGIT(c) ((c) >= '0' && (c) <= '7')
#define IS_HEXDIGIT(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#define HEX2INT(c)     ((c) >= 'a' ? (c) - 'a' + 10 : (c) >= 'A' ? (c) - 'A' + 10 : (c) - '0')

char *ParseQuoted(char *p, char **out_end) {
  char *out = p;
  char *start = out;
  while (*p && *p != '"') {
    if (*p != '\\' || !p[1]) {
      *out++ = *p++;
      continue;
    }
    ++p;
    switch (*p) {
    case 'a':
      *out++ = '\a';
      ++p;
      break;
    case 'b':
      *out++ = '\b';
      ++p;
      break;
    case 'e':
      *out++ = '\033';
      ++p;
      break;
    case 'f':
      *out++ = '\f';
      ++p;
      break;
    case 'n':
      *out++ = '\n';
      ++p;
      break;
    case 'r':
      *out++ = '\r';
      ++p;
      break;
    case 't':
      *out++ = '\t';
      ++p;
      break;
    case 'v':
      *out++ = '\v';
      ++p;
      break;
    case '0':
      *out++ = '\0';
      ++p;
      break;
    case 'x':
      if (IS_HEXDIGIT(p[1]) && IS_HEXDIGIT(p[2])) {
        *out++ = (HEX2INT(p[1]) << 4) | HEX2INT(p[2]);
        p += 3;
      } else {
        *out++ = *p++;
      }
      break;
    default:
      if (IS_OCTDIGIT(p[0]) && IS_OCTDIGIT(p[1]) && IS_OCTDIGIT(p[2])) {
        *out++ = ((p[0] - '0') << 6) | ((p[1] - '0') << 3) | (p[2] - '0');
        p += 3;
      } else {
        *out++ = *p++;
      }
      break;
    }
  }
  *out = '\0';
  if (*p == '"') {
    ++p;
  }
  if (out_end) {
    *out_end = p;
  }
  return start;
}

int TokenizeLine(char *line, char **tokens, size_t max_tokens) {
  size_t count = 0;
  char *p = line;
  while (*p && count < max_tokens) {
    while (*p && isspace((unsigned char)*p)) {
      ++p;
    }
    if (!*p) {
      break;
    }
    if (*p == '#') {
      break;
    }
    if (*p == '"') {
      ++p;
      tokens[count++] = ParseQuoted(p, &p);
      continue;
    }
    tokens[count++] = p;
    while (*p && !isspace((unsigned char)*p)) {
      ++p;
    }
    if (*p) {
      *p++ = '\0';
    }
  }
  return static_cast<int>(count);
}

bool IsHigh(const char *token) {
  return EqualsIgnoreCase(token, "High") || EqualsIgnoreCase(token, "Bold");
}

bool IsOn(const char *token) {
  return EqualsIgnoreCase(token, "On");
}

bool ParseColorName(const char *token, trgb *out) {
  if (EqualsIgnoreCase(token, "Black")) {
    *out = tblack;
  } else if (EqualsIgnoreCase(token, "Blue")) {
    *out = tblue;
  } else if (EqualsIgnoreCase(token, "Green")) {
    *out = tgreen;
  } else if (EqualsIgnoreCase(token, "Cyan")) {
    *out = tcyan;
  } else if (EqualsIgnoreCase(token, "Red")) {
    *out = tred;
  } else if (EqualsIgnoreCase(token, "Magenta")) {
    *out = tmagenta;
  } else if (EqualsIgnoreCase(token, "Yellow")) {
    *out = tyellow;
  } else if (EqualsIgnoreCase(token, "White")) {
    *out = twhite;
  } else {
    return false;
  }
  return true;
}

bool ParseColorSpec(char **tokens, int count, int start, tcolor *out) {
  int i = start;
  if (i >= count) {
    return false;
  }
  if (IsOn(tokens[i])) {
    trgb fg = twhite;
    bool bg_high = false;
    ++i;
    if (i < count && IsHigh(tokens[i])) {
      bg_high = true;
      ++i;
    }
    if (i >= count) {
      return false;
    }
    trgb bg = 0;
    if (!ParseColorName(tokens[i], &bg)) {
      return false;
    }
    if (bg_high) {
      bg |= thigh;
    }
    ++i;
    if (i != count) {
      return false;
    }
    *out = TCOL(fg, bg);
    return true;
  }

  bool fg_high = false;
  if (IsHigh(tokens[i])) {
    fg_high = true;
    ++i;
  }
  if (i >= count) {
    return false;
  }
  trgb fg = 0;
  if (!ParseColorName(tokens[i], &fg)) {
    return false;
  }
  if (fg_high) {
    fg |= thigh;
  }
  ++i;
  if (i >= count) {
    *out = TCOL(fg, tblack);
    return true;
  }
  if (!IsOn(tokens[i])) {
    return false;
  }
  ++i;
  bool bg_high = false;
  if (i < count && IsHigh(tokens[i])) {
    bg_high = true;
    ++i;
  }
  if (i >= count) {
    return false;
  }
  trgb bg = 0;
  if (!ParseColorName(tokens[i], &bg)) {
    return false;
  }
  if (bg_high) {
    bg |= thigh;
  }
  ++i;
  if (i != count) {
    return false;
  }
  *out = TCOL(fg, bg);
  return true;
}

bool ApplyChromeRunes(trune *dest, size_t len, const char *shape, const char *path, unsigned line_no) {
  trune buf[9] = {0};
  size_t count = DecodeRunes(shape, buf, len);
  if (count < len) {
    log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
                 << ": Chrome value too short\n";
    return false;
  }
  for (size_t i = 0; i < len; ++i) {
    dest[i] = buf[i];
  }
  return true;
}

bool ApplyChrome(const char *target, const char *shape, const char *path, unsigned line_no) {
  if (EqualsIgnoreCase(target, "GadgetResize")) {
    return ApplyChromeRunes(GadgetResize, 2, shape, path, line_no);
  }
  if (EqualsIgnoreCase(target, "ScrollBarX")) {
    return ApplyChromeRunes(ScrollBarX, 3, shape, path, line_no);
  }
  if (EqualsIgnoreCase(target, "ScrollBarY")) {
    return ApplyChromeRunes(ScrollBarY, 3, shape, path, line_no);
  }
  if (EqualsIgnoreCase(target, "TabX")) {
    return ApplyChromeRunes(&TabX, 1, shape, path, line_no);
  }
  if (EqualsIgnoreCase(target, "TabY")) {
    return ApplyChromeRunes(&TabY, 1, shape, path, line_no);
  }
  if (EqualsIgnoreCase(target, "ScreenBack")) {
    return ApplyChromeRunes(Screen_Back, 2, shape, path, line_no);
  }

  log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
               << ": unknown Chrome target '" << Chars::from_c(target) << "'\n";
  return false;
}

bool ApplyDefaultColor(const char *target, tcolor color, const char *path, unsigned line_no) {
  if (EqualsIgnoreCase(target, "Gadgets")) {
    DEFAULT_ColGadgets = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "Arrows")) {
    DEFAULT_ColArrows = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "Bars")) {
    DEFAULT_ColBars = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "Tabs")) {
    DEFAULT_ColTabs = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "Border")) {
    DEFAULT_ColBorder = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "Disabled")) {
    DEFAULT_ColDisabled = color;
    return true;
  }
  if (EqualsIgnoreCase(target, "SelectDisabled")) {
    DEFAULT_ColSelectDisabled = color;
    return true;
  }

  log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
               << ": unknown DefaultColors target '" << Chars::from_c(target) << "'\n";
  return false;
}

void ParseThemeLine(const char *path, unsigned line_no, char *line) {
  StripComment(line);
  char *p = TrimLeft(line);
  TrimRight(p);
  if (!*p) {
    return;
  }
  char *tokens[kMaxTokens];
  int count = TokenizeLine(p, tokens, kMaxTokens);
  if (count <= 0) {
    return;
  }
  if (EqualsIgnoreCase(tokens[0], "Chrome")) {
    if (count < 3) {
      log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
                   << ": Chrome expects <Target> <String>\n";
      return;
    }
    ApplyChrome(tokens[1], tokens[2], path, line_no);
    return;
  }
  if (EqualsIgnoreCase(tokens[0], "DefaultColors")) {
    if (count < 3) {
      log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
                   << ": DefaultColors expects <Target> <ColorSpec>\n";
      return;
    }
    tcolor color = 0;
    if (!ParseColorSpec(tokens, count, 2, &color)) {
      log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
                   << ": invalid color spec\n";
      return;
    }
    ApplyDefaultColor(tokens[1], color, path, line_no);
    return;
  }

  log(WARNING) << "twin: " << Chars::from_c(path) << ":" << line_no
               << ": unknown theme directive '" << Chars::from_c(tokens[0]) << "'\n";
}

}  // namespace

bool LoadThemeConfig(void) {
  uldat file_size = 0;
  char *path = FindConfigFile(kThemeFileName, &file_size);
  if (!path) {
    return true;
  }
  FILE *fp = fopen(path, "r");
  if (!fp) {
    log(WARNING) << "twin: failed to open theme file " << Chars::from_c(path) << "\n";
    FreeMem(path);
    return false;
  }

  char line[kMaxLine];
  unsigned line_no = 0;
  while (fgets(line, sizeof(line), fp)) {
    ++line_no;
    if (!strchr(line, '\n') && !feof(fp)) {
      int ch;
      while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
      }
    }
    ParseThemeLine(path, line_no, line);
  }
  fclose(fp);
  FreeMem(path);
  return true;
}
