/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfcegui4/libxfcegui4.h>
#include <xfce-mcs-manager/manager-plugin.h>



static void
run_dialog (McsPlugin *plugin)
{
  GtkWidget *message;
  GError    *error = NULL;
  gchar     *argv[] = { "exo-preferred-applications", NULL };

  if (!g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error))
    {
      message = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        "%s.", _("Failed to launch exo-preferred-applications"));
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (message));
      gtk_widget_destroy (message);
      g_error_free (error);
    }
}



McsPluginInitResult
mcs_plugin_init (McsPlugin *plugin)
{
  GtkIconTheme *icon_theme;

  /* setup i18n support */
#ifdef GETTEXT_PACKAGE
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* initialize plugin structure */
  plugin->plugin_name = g_strdup ("exo-preferred-applications");
  plugin->caption = g_strdup (_("Preferred Applications"));
  plugin->run_dialog = run_dialog;

  /* lookup the icon (on the default icon theme, *sigh*) */
  icon_theme = gtk_icon_theme_get_default ();
  plugin->icon = gtk_icon_theme_load_icon (icon_theme, "preferences-desktop-default-applications", 48, 0, NULL);

  /* if that didn't work, we know where we installed the icon, so load it directly */
  if (G_UNLIKELY (plugin->icon == NULL))
    plugin->icon = gdk_pixbuf_new_from_file (DATADIR "/icons/hicolor/48x48/preferences-desktop-default-applications.png", NULL);

  return MCS_PLUGIN_INIT_OK;
}



MCS_PLUGIN_CHECK_INIT;
