/*
 *  Copyright (C) 2008 Giuseppe Torelli - <colossus73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "archive.h"
#ifndef HAVE_STRCASESTR
const char *strcasestr(const char *haystack, const char *needle);
#endif
#ifndef HAVE_MKDTEMP
char *mkdtemp (gchar *tmpl);
#endif
gchar *xa_escape_bad_chars ( gchar *string , gchar *pattern);
char *xa_escape_common_chars (const char *str, const char *meta_chars, const char  prefix, const char  postfix);
gchar *remove_level_from_path (const gchar *path);
gboolean file_extension_is (const char *filename, const char *ext);
gchar *extract_local_path (gchar *path);
void xa_set_window_title ( GtkWidget *window , gchar *title);
gboolean match_patterns (char **patterns,const char *string,int flags);
gchar *xa_remove_path_from_archive_name(gchar *name);
void xa_cat_filenames (XArchive *,GSList *,GString *);
void xa_cat_filenames_basename (XArchive *,GSList *,GString *);
void xa_escape_filename (gchar *,GString *data);
#endif

