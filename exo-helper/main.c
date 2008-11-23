/* $Id$ */
/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo-helper/exo-helper-chooser-dialog.h>
#include <exo-helper/exo-helper-launcher-dialog.h>
#include <exo-helper/exo-helper-utils.h>



static const gchar *CATEGORY_EXEC_ERRORS[] =
{
  N_("Failed to execute default Web Browser"),
  N_("Failed to execute default Mail Reader"),
  N_("Failed to execute default Terminal Emulator"),
};



int
main (int argc, char **argv)
{
  ExoHelperCategory  category;
  ExoHelperDatabase *database;
  ExoHelper         *helper;
  GtkWidget         *dialog;
  GError            *error = NULL;
  gint               result = EXIT_SUCCESS;

  gboolean           opt_version = FALSE;
  gboolean           opt_configure = FALSE;
  gchar             *opt_launch_type = NULL;
  GdkNativeWindow    opt_socket_id = 0;

  GOptionContext    *opt_ctx;
  GOptionGroup      *gtk_option_group;
  GOptionEntry       option_entries[] = {
    { "version", 'V', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_NONE, &opt_version, N_("Print version information and exit"), NULL, },
    { "configure", 'c', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_NONE, &opt_configure, N_("Open the Preferred Applications\nconfiguration dialog"), NULL, },
    { "socket-id", 's', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_INT, &opt_socket_id, N_("Settings manager socket"), N_("SOCKET ID"), },
    { NULL, },
    { "launch", 'l', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_STRING, &opt_launch_type, N_("Launch the default helper of TYPE with the optional PARAMETER, where TYPE is one of the following values."), N_("TYPE [PARAMETER]"), },
  };

  /* sanity check helper categories */
  g_assert (EXO_HELPER_N_CATEGORIES == G_N_ELEMENTS (CATEGORY_EXEC_ERRORS));

#ifdef GETTEXT_PACKAGE
  /* setup i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* set up options */
  opt_ctx = g_option_context_new (NULL);

  gtk_option_group = gtk_get_option_group (FALSE);
  g_option_context_add_group (opt_ctx, gtk_option_group);

  g_option_context_add_main_entries (opt_ctx, option_entries, NULL);
  /* Note to Translators: Do not translate the TYPEs (WebBrowser, MailReader, TerminalEmulator),
   * since the exo-helper utility will not accept localized TYPEs.
   */
  g_option_context_set_description (opt_ctx,
                                    _("The following TYPEs are supported for the --launch command:\n\n"
                                      "  WebBrowser       - The preferred Web Browser.\n"
                                      "  MailReader       - The preferred Mail Reader.\n"
                                      "  TerminalEmulator - The preferred Terminal Emulator."));

  if (!g_option_context_parse (opt_ctx, &argc, &argv, &error))
    {
      if (G_LIKELY (error)) {
            g_printerr ("%s: %s.\n", G_LOG_DOMAIN, error->message);
            g_printerr (_("Type '%s --help' for usage."), G_LOG_DOMAIN);
            g_printerr ("\n");
            g_error_free (error);
        } else
            g_error ("Unable to open display.");

        return EXIT_FAILURE;
    }

  /* initialize Gtk+ */
  gtk_init (&argc, &argv);

  /* setup default window icon */
  gtk_window_set_default_icon_name ("preferences-desktop-default-applications");

  /* check for the action to perform */
  if (opt_configure == TRUE)
    {
      dialog = exo_helper_chooser_dialog_new ();

      if (opt_socket_id != 0)
        {
          GtkWidget *plug, *plug_child;

          plug = gtk_plug_new (opt_socket_id);
          gtk_widget_show (plug);
          g_signal_connect (plug, "delete-event", G_CALLBACK (gtk_main_quit), NULL);

          plug_child = exo_helper_chooser_dialog_get_plug_child (EXO_HELPER_CHOOSER_DIALOG (dialog));
          gtk_widget_reparent (plug_child, plug);
          gtk_widget_show (plug_child);

          /* End startup notification */
          gdk_notify_startup_complete ();

          gtk_main ();
          gtk_widget_destroy (plug);
        }
      else
        {
          gtk_dialog_run (GTK_DIALOG (dialog));
        }

      gtk_widget_destroy (dialog);
    }
  else if (opt_launch_type != NULL)
    {
      /* try to parse the type */
      if (!exo_helper_category_from_string (opt_launch_type, &category))
        {
          g_warning (_("Invalid helper type \"%s\""), opt_launch_type);
          return EXIT_FAILURE;
        }

      /* determine the default helper for the category */
      database = exo_helper_database_get ();
      helper = exo_helper_database_get_default (database, category);

      /* check if we have a valid helper */
      if (G_UNLIKELY (helper == NULL))
        {
          /* ask the user to choose a default helper for category */
          dialog = exo_helper_launcher_dialog_new (category);
          if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
            helper = exo_helper_database_get_default (database, category);
          gtk_widget_destroy (dialog);
        }

      /* release our reference on the database */
      g_object_unref (G_OBJECT (database));

      /* check if we have a valid helper now */
      if (G_LIKELY (helper != NULL))
        {
          /* try to execute the helper with the given parameter */
          if (!exo_helper_execute (helper, NULL, (argc > 1) ? argv[1] : NULL, &error))
            {
              dialog = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               "%s.", _(CATEGORY_EXEC_ERRORS[category]));
              gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);
              g_error_free (error);
              result = EXIT_FAILURE;
            }
          g_object_unref (G_OBJECT (helper));
        }
    }
  else if (opt_version == TRUE)
    {
      g_print (_("%s (Xfce %s)\n\n"
                 "Copyright (c) 2003-2006\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"
                 "Built with Gtk+-%d.%d.%d, running Gtk+-%d.%d.%d.\n\n"
                 "Please report bugs to <%s>.\n"),
                 PACKAGE_STRING, xfce_version_string (),
                 GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION,
                 gtk_major_version, gtk_minor_version, gtk_micro_version,
                 PACKAGE_BUGREPORT);
    }
  else
    {
      result = EXIT_FAILURE;
#if GLIB_CHECK_VERSION(2, 14, 0)
      g_printerr ("%s", g_option_context_get_help (opt_ctx, FALSE, NULL));
#else
      g_printerr (_("Unknown helper arguments.\n"));
#endif
    }

  g_option_context_free (opt_ctx);

  return result;
}
