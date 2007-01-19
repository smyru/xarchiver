/*
 *  Copyright (C) 2006 Giuseppe Torelli - <colossus73@gmail.com>
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
#include "tar.h"
#include "string_utils.h"

void OpenTar ( XArchive *archive )
{
	gchar *command;
	gchar *tar;

	tar = g_find_program_in_path ("gtar");
	if (tar == NULL)
		tar = g_strdup ("tar");

	command = g_strconcat (tar, " tfv " , archive->escaped_path, NULL);
	archive->has_properties = archive->can_add = archive->can_extract = TRUE;
	archive->has_test = archive->has_sfx = FALSE;
	archive->dummy_size = 0;
	archive->nr_of_files = 0;
	archive->nr_of_dirs = 0;
	archive->format ="TAR";
	archive->parse_output = TarOpen;

	xa_spawn_async_process (archive,command,0);

	g_free (command);
	g_free (tar);

	if (archive->child_pid == 0)
		return;

	char *names[]= {(""),(_("Filename")),(_("Permissions")),(_("Symbolic Link")),(_("Owner/Group")),(_("Size")),(_("Date")),(_("Time"))};
	GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING};
	xa_create_liststore ( 8, names , (GType *)types, archive );
}

gboolean TarOpen (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	XArchive *archive = data;
	gchar **fields;
	gchar *filename;
	gchar *line = NULL;
	gchar *temp = NULL;
	GtkTreeIter iter;
	GIOStatus status = G_IO_STATUS_NORMAL;

	if (cond & (G_IO_IN | G_IO_PRI) )
	{
		do
		{
			status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
			if ( line == NULL )
				break;

			archive->cmd_line_output = g_list_append (archive->cmd_line_output,g_strdup(line));
			fields = split_line (line,5);
			filename = get_last_field (line,6);
			gtk_list_store_append (archive->liststore, &iter);

			if ( strstr(fields[0] , "d") == NULL )
				archive->nr_of_files++;
			else
				archive->nr_of_dirs++;

			temp = g_strrstr (filename,"->");
			if (temp)
			{
				gtk_list_store_set (archive->liststore, &iter,2,g_strstrip(&temp[3]),-1);
				temp = g_strstrip(g_strndup(filename, strlen(filename) - strlen(temp) ));
				gtk_list_store_set (archive->liststore, &iter,0,temp,-1);
				g_free (temp);
			}
			else
			{
				gtk_list_store_set (archive->liststore, &iter,2,NULL,-1);
				gtk_list_store_set (archive->liststore, &iter,1,filename,-1);
			}

			gtk_list_store_set (archive->liststore, &iter,2,fields[0],-1);
			gtk_list_store_set (archive->liststore, &iter,4,fields[1],-1);
			gtk_list_store_set (archive->liststore, &iter,5,strtoll(fields[2],NULL,0),-1);
			gtk_list_store_set (archive->liststore, &iter,6,fields[3],-1);
			gtk_list_store_set (archive->liststore, &iter,7,fields[4],-1);

			while (gtk_events_pending() )
				gtk_main_iteration();

			archive->dummy_size += strtoll(fields[2],NULL,0);
			g_strfreev ( fields );
			g_free (line);
		}
		while (status == G_IO_STATUS_NORMAL);

		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
			goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) )
	{
done:	g_io_channel_shutdown ( ioc,TRUE,NULL );
		g_io_channel_unref (ioc);
		gtk_tree_view_set_model (GTK_TREE_VIEW(archive->treeview), archive->model);
		g_object_unref (archive->model);
		return FALSE;
	}
	return TRUE;
}

gboolean isTar ( FILE *ptr )
{
	unsigned char magic[7];
	fseek ( ptr, 0 , SEEK_SET );
    if ( fseek ( ptr , 257 , SEEK_CUR) < 0 )
		return FALSE;
    if ( fread ( magic, 1, 7, ptr ) == 0 )
		return FALSE;
    if ( memcmp ( magic,"\x75\x73\x74\x61\x72\x00\x30",7 ) == 0 || memcmp (magic,"\x75\x73\x74\x61\x72\x20\x20",7 ) == 0)
		return TRUE;
    else
		return FALSE;
}
