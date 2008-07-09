/*
 *  Copyright (c) 2008 Giuseppe Torelli <colossus73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 * *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __XARCHIVER_ARCHIVE_H__
#define __XARCHIVER_ARCHIVE_H__

int status;

typedef enum
{
	XARCHIVETYPE_UNKNOWN = 0,
	XARCHIVETYPE_7ZIP,
	XARCHIVETYPE_ARJ,
	XARCHIVETYPE_DEB,
	XARCHIVETYPE_BZIP2,
	XARCHIVETYPE_GZIP,
	XARCHIVETYPE_LZMA,
	XARCHIVETYPE_RAR,
	XARCHIVETYPE_RPM,
	XARCHIVETYPE_TAR,
	XARCHIVETYPE_TAR_BZ2,
	XARCHIVETYPE_TAR_GZ,
	XARCHIVETYPE_TAR_LZMA,
	XARCHIVETYPE_ZIP,
	XARCHIVETYPE_LHA,
	XARCHIVETYPE_COUNT
} XArchiveType;

typedef enum
{
	XA_ARCHIVESTATUS_IDLE = 0,
	XA_ARCHIVESTATUS_EXTRACT,
	XA_ARCHIVESTATUS_ADD,
	XA_ARCHIVESTATUS_DELETE,
	XA_ARCHIVESTATUS_OPEN,
	XA_ARCHIVESTATUS_TEST,
	XA_ARCHIVESTATUS_SFX
} XArchiveStatus;

typedef struct _XEntry XEntry;

struct _XEntry
{
	gchar *filename;
	gchar *mime_type;
	gpointer columns;
	gboolean is_dir;
	gboolean is_encrypted;
	XEntry *child;
	XEntry *prev;
	XEntry *next;
};

typedef struct _XArchive XArchive;

typedef void (*parse_output_func)	(gchar *line, gpointer);
typedef void (*delete_func)		(XArchive *,GString *);
typedef void (*add_func)		(XArchive *,GString *,gchar *);
typedef void (*extract_func)		(XArchive *,GString *);
typedef void (*test_func)		(XArchive *);

struct _XArchive
{
	XArchiveType type;
	XArchiveStatus status;
	XEntry *root_entry;
	XEntry *current_entry;
	GSList *back;
	GSList *forward;
	gchar *path;
	gchar *escaped_path;
	gchar *tmp;
	gchar *format;
	gchar *extraction_path;
	gchar *passwd;
	gchar *location_entry_path;
	GtkTreeModel *model;
	GtkCellRenderer *renderer;
	GtkListStore *liststore;
	GtkWidget *treeview;
	GtkWidget *scrollwindow;
	gboolean has_passwd;
	gboolean has_comment;
	gboolean has_test;
	gboolean has_sfx;
	gboolean can_add;
	gboolean can_extract;
	gboolean has_properties;
	gboolean list_reversed;
	GString *comment;
	GSList *error_output;
	GType *column_types;
	gboolean add_recurse;
	gboolean overwrite;
	gboolean full_path;
	gboolean freshen;
	gboolean update;
	gboolean tar_touch;
	gboolean solid_archive;
	gboolean remove_files;
	unsigned short int compression_level;
	unsigned short int nc;
	gint nr_of_files;
	gint nr_of_dirs;
	gint output_fd;
	gint error_fd;
	guint pb_source;
	GPid child_pid;
	unsigned long long int dummy_size;
	parse_output_func parse_output;
	delete_func delete;
	add_func add;
	extract_func extract;
	test_func test;
};

void xa_spawn_async_process (XArchive *archive, gchar *command);
XArchive *xa_init_archive_structure(gint);
void xa_clean_archive_structure (XArchive *archive);
gboolean xa_dump_child_error_messages (GIOChannel *ioc, GIOCondition cond, gpointer data);
gboolean xa_create_temp_directory(XArchive *archive);
void xa_delete_temp_directory(XArchive *archive,gboolean flag);
gboolean xa_run_command (XArchive *archive,GSList *commands);
gint xa_find_archive_index (gint page_num);
gint xa_get_new_archive_idx();
XEntry *xa_alloc_memory_for_each_row ( guint nc,GType column_types[]);
void xa_free_entry(XArchive *archive,XEntry *entry);
XEntry *xa_find_child_entry(XEntry *entry, gchar *string);
XEntry *xa_set_archive_entries_for_each_row(XArchive *archive,gchar *filename,gpointer *items);
gpointer *xa_fill_archive_entry_columns_for_each_row (XArchive *archive,XEntry *entry,gpointer *items);
XEntry* xa_find_entry_from_path(XEntry *root_entry,const gchar *fullpathname);
gchar *xa_build_full_path_name_from_entry(XEntry *entry);
void xa_fill_list_with_recursed_entries(XEntry *entry,GString **p_file_list,gchar *current_path);
void xa_entries_to_filelist(XEntry *, GString **, gchar *);
gboolean xa_detect_encrypted_archive (XArchive *archive);
void xa_browse_dir_sidebar (XEntry *, GtkTreeStore *,gchar *,GtkTreeIter *);
void xa_fill_dir_sidebar(XArchive *,gboolean);
void xa_sidepane_row_selected(GtkTreeSelection *selection, gpointer data);
void xa_sidepane_select_row(XEntry *entry);
gboolean _xa_sidepane_select_row(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter,gpointer data);
XArchive *archive[100];
#endif
