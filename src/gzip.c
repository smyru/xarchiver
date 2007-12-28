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

#include "config.h"
#include "gzip.h"
#include "bzip2.h"

extern void xa_create_liststore (XArchive *archive, gchar *columns_names[]);

void xa_open_gzip (XArchive *archive)
{
	gchar *command;
	gchar *tar;
	unsigned short int i;

	if (g_str_has_suffix (archive->escaped_path,".tar.gz") || g_str_has_suffix (archive->escaped_path,".tgz"))
	{
		archive->type = XARCHIVETYPE_TAR_GZ;
	    tar = g_find_program_in_path ("gtar");
	    if (tar == NULL)
    		tar = g_strdup ("tar");

		command = g_strconcat (tar, " tzvf " , archive->escaped_path, NULL );
		archive->has_properties = archive->can_add = archive->can_extract = TRUE;
		archive->has_test = archive->has_sfx = FALSE;
		archive->dummy_size = 0;
		archive->nr_of_files = 0;
		archive->nr_of_dirs = 0;
		archive->format ="TAR.GZIP";
		archive->nc = 7;
		archive->parse_output = xa_get_tar_line_content;
		xa_spawn_async_process (archive,command);

		g_free (command);
		g_free (tar);

		if (archive->child_pid == 0)
			return;

		GType types[]= {GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_POINTER};
		archive->column_types = g_malloc0(sizeof(types));
		for (i = 0; i < 9; i++)
			archive->column_types[i] = types[i];

		char *names[]= {(_("Points to")),(_("Permissions")),(_("Owner/Group")),(_("Size")),(_("Date")),(_("Time")),NULL};
		xa_create_liststore (archive,names);
	}
	else
	{
		archive->can_add = archive->has_test = archive->has_sfx = FALSE;
		archive->has_properties = archive->can_extract = TRUE;
		archive->nc = 4;
		archive->parse_output = xa_get_gzip_line_content;
		archive->nr_of_files = 1;
		archive->nr_of_dirs = 0;
		archive->format = "GZIP";

		GType types[]= {GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_POINTER};
		archive->column_types = g_malloc0(sizeof(types));
		for (i = 0; i < 6; i++)
			archive->column_types[i] = types[i];

		char *names[]= {(_("Compressed")),(_("Size")),(_("Ratio"))};
		xa_create_liststore (archive,names);
		
		command = g_strconcat ("gzip -l ",archive->escaped_path,NULL);
		xa_spawn_async_process (archive,command);
		g_free (command);

		if (archive->child_pid == 0)
			return;
	}
}

void xa_get_gzip_line_content (gchar *line, gpointer data)
{
	XArchive *archive = data;
	XEntry *entry;
	gchar *filename;
	gchar *basename;
	gpointer item[2];
	gint n = 0, a = 0 ,linesize = 0;

	linesize = strlen(line);
	if (line[9] == 'c')
		return;

	/* Size */
	for(n=0; n < linesize && line[n] == ' '; n++);
	a = n;
	for(; n < linesize && line[n] != ' '; n++);
	line[n]='\0';
	item[0] = line + a;
	n++;

	/* Compressed */
	for(; n < linesize && line[n] == ' '; n++);
	a = n;
	for(; n < linesize && line[n] != ' '; n++);
	line[n]='\0';
	item[1] = line + a;
	archive->dummy_size += strtoll(item[1],NULL,0);
	n++;

	/* Ratio */
	for(; n < linesize && line[n] == ' '; n++);
	a = n;
	for(; n < linesize && line[n] != ' '; n++);
	line[n] = '\0';
	item[2] = line + a;
	n++;
	
	line[linesize-1] = '\0';
	filename = line+n;
	
	basename = g_path_get_basename(filename);
	if (basename == NULL)
		basename = g_strdup(filename);

	entry = xa_set_archive_entries_for_each_row (archive,basename,item);
	g_free(basename);
}
