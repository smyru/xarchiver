/*
 *  Copyright (C) 2007 Giuseppe Torelli - <colossus73@gmail.com>
 *  Copyright (C) 2006 Benedikt Meurer - <benny@xfce.org>
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
#include "zip.h"

extern void xa_create_liststore ( unsigned short int nc, gchar *columns_names[], GType columns_types[], XArchive *archive);
static void xa_get_zip_line_content (gchar *line, gpointer data);

void xa_open_zip ( XArchive *archive )
{
	gchar *command = g_strconcat ( "zipinfo -t -l " , archive->escaped_path, NULL );
	archive->has_sfx = archive->has_properties = archive->can_add = archive->can_extract = archive->has_test = TRUE;
	archive->dummy_size = 0;
    archive->nr_of_files = 0;
    archive->nr_of_dirs = 0;
	archive->parse_output = xa_get_zip_line_content;
	archive->format ="ZIP";
	xa_spawn_async_process (archive,command,0);
	g_free ( command );
	if ( archive->child_pid == 0 )
		return;

	char *names[]= {(""),(_("Filename")),(_("Permissions")),(_("Version")),(_("OS")),(_("Original")),(_("Compressed")),(_("Method")),(_("Date")),(_("Time"))};
	GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING};
	xa_create_liststore (10,names,(GType *)types,archive);
}

void xa_get_zip_line_content (gchar *line, gpointer data)
{
	XArchive *archive = data;
	gchar **fields = NULL;
	gchar *filename = NULL;
	GtkTreeIter iter;

	if ((line[0] != 'd') && (line[0] != '-'))
		return;

	fields = split_line (line,9);
	filename = get_last_field (line,10);
	if ( g_str_has_suffix(filename , "/") == TRUE)
		archive->nr_of_dirs++;
	else
		archive->nr_of_files++;
	if ( filename != NULL )
	{
		gtk_list_store_append (archive->liststore, &iter);
		for (x = 2; x < 9; x++)
		{
			if (x == 2)
				gtk_list_store_set (archive->liststore, &iter,4,fields[2], -1);
			if (x == 3 || x == 5)
			{
				if (x == 3)
					gtk_list_store_set (archive->liststore, &iter,5, strtoll (fields[x],NULL,0), -1);
				else
					gtk_list_store_set (archive->liststore, &iter,6, strtoll (fields[x],NULL,0), -1);
			}
			else if (x == 4)
				gtk_list_store_set (archive->liststore, &iter,3,fields[1], -1);
			else
				gtk_list_store_set (archive->liststore, &iter,x+1,fields[x], -1);
		}
		archive->dummy_size += strtoll (fields[0],NULL,0);
		gtk_list_store_set (archive->liststore, &iter,0,GTK_STOCK_DIRECTORY,1,filename,-1);
		gtk_list_store_set (archive->liststore, &iter,2,fields[0],-1);
	}
	while ( gtk_events_pending() )
		gtk_main_iteration();
	g_strfreev (fields);
}
