/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#if !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file is not part of the public API."
#endif

#ifndef __EXO_PRIVATE_H__
#define __EXO_PRIVATE_H__

#include <gtk/gtk.h>

#include <glib/gi18n-lib.h>

G_BEGIN_DECLS;

/* support macros for the slice allocator */
#if GLIB_CHECK_VERSION(2,10,0)
#define _exo_slice_alloc(block_size)             (g_slice_alloc ((block_size)))
#define _exo_slice_alloc0(block_size)            (g_slice_alloc0 ((block_size)))
#define _exo_slice_free1(block_size, mem_block)  G_STMT_START{ g_slice_free1 ((block_size), (mem_block)); }G_STMT_END
#define _exo_slice_new(type)                     (g_slice_new (type))
#define _exo_slice_new0(type)                    (g_slice_new0 (type))
#define _exo_slice_free(type, ptr)               G_STMT_START{ g_slice_free (type, (ptr)); }G_STMT_END
#else
#define _exo_slice_alloc(block_size)             (g_malloc ((block_size)))
#define _exo_slice_alloc0(block_size)            (g_malloc0 ((block_size)))
#define _exo_slice_free1(block_size, mem_block)  G_STMT_START{ g_free ((mem_block)); }G_STMT_END
#define _exo_slice_new(type)                     (g_new (type, 1))
#define _exo_slice_new0(type)                    (g_new0 (type, 1))
#define _exo_slice_free(type, ptr)               G_STMT_START{ g_free ((ptr)); }G_STMT_END
#endif

void  _exo_i18n_init                    (void) G_GNUC_INTERNAL;

void  _exo_gtk_widget_send_focus_change (GtkWidget *widget,
                                         gboolean   in) G_GNUC_INTERNAL;

GType _exo_g_type_register_simple       (GType        type_parent,
                                         const gchar *type_name_static,
                                         guint        class_size,
                                         gpointer     class_init,
                                         guint        instance_size,
                                         gpointer     instance_init) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__EXO_PRIVATE_H__ */
