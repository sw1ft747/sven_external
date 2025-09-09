#include <IClientPlugin.h>
#include <ICvar.h>
#include <convar.h>
#include <svenmod_version.h>
#include <dbg.h>

#include "plugins_manager.h"

CON_COMMAND(sm, "Options for SvenMod\nsm <command> [arg1] [arg2]..")
{
	if (args.ArgC() > 1)
	{
		const char *pszOption = args[1];

		if ( !stricmp(pszOption, "plugins") )
		{
			if (args.ArgC() > 2)
			{
				const char *pszCommand = args[2];

				if ( !stricmp(pszCommand, "list") )
				{
					g_PluginsManager.PrintPlugins();
				}
				else if ( !stricmp(pszCommand, "info") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);
						g_PluginsManager.PrintPluginInfo( nPluginIndex );
					}
					else
					{
						Msg("Usage: sm plugins info <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "load") )
				{
					if (args.ArgC() > 3)
					{
						const char *pszFileName = args[3];

						g_PluginsManager.LoadPlugin( pszFileName, false );
					}
					else
					{
						Msg("Usage: sm plugins load <filename>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unload") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.UnloadPlugin( nPluginIndex );
					}
					else
					{
						Msg("Usage: sm plugins unload <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unload_all") )
				{
					g_PluginsManager.UnloadPlugins();
					Msg("[SvenMod] Unloaded all plugins\n");
				}
				else if ( !stricmp(pszCommand, "pause") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.PausePlugin( nPluginIndex );
					}
					else
					{
						Msg("Usage: sm plugins pause <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "unpause") )
				{
					if (args.ArgC() > 3)
					{
						int nPluginIndex = atoi(args[3]);

						g_PluginsManager.UnpausePlugin( nPluginIndex );
					}
					else
					{
						Msg("Usage: sm plugins unpause <index>\n");
					}
				}
				else if ( !stricmp(pszCommand, "pause_all") )
				{
					if ( g_PluginsManager.PausePlugins() )
						Msg("[SvenMod] All plugins have been paused\n");
					else
						Msg("[SvenMod] None of plugins have been paused\n");
				}
				else if ( !stricmp(pszCommand, "unpause_all") )
				{
					if ( g_PluginsManager.UnpausePlugins() )
						Msg("[SvenMod] All plugins have been unpaused\n");
					else
						Msg("[SvenMod] None of plugins have been unpaused\n");
				}
				else if ( !stricmp(pszCommand, "refresh") )
				{
					g_PluginsManager.UnloadPlugins();
					g_PluginsManager.LoadPlugins();

					Msg("[SvenMod] Reloaded all plugins from file \"../svenmod/plugins.txt\"");
				}
				else
				{
					goto L_SHOW_PL_HELP;
				}
			}
			else
			{
			L_SHOW_PL_HELP:
					Msg("* sm plugins list        - Print list of indexed plugins\n");
					Msg("* sm plugins info        - Print detailed information about a plugin\n");
				#ifdef PLATFORM_WINDOWS
					Msg("* sm plugins load        - Load a plugin from directory \"../svenmod/plugins/*.dll\"\n");
				#else
					Msg("* sm plugins load        - Load a plugin from directory \"../svenmod/plugins/*.so\"\n");
				#endif
					Msg("* sm plugins unload      - Unload a loaded plugin\n");
					Msg("* sm plugins unload_all  - Unload all loaded plugins\n");
					Msg("* sm plugins pause       - Pause a loaded plugin\n");
					Msg("* sm plugins unpause     - Unpause a loaded plugin\n");
					Msg("* sm plugins pause_all   - Pause all running plugins\n");
					Msg("* sm plugins unpause_all - Unpause all paused plugins\n");
					Msg("* sm plugins refresh     - Reload all plugins from file \"../svenmod/plugins.txt\"\n");
			}
		}
		else if ( !stricmp(pszOption, "info") )
		{
			Msg("[SvenMod] Information:\n");
			Msg("----------------------------------\n");
			Msg("* Version:          %s\n", SVENMOD_VERSION_STRING);
			Msg("* API Version:      %s\n", SVENMOD_API_VERSION_STRING);
			Msg("* Plugin Interface: %s\n", CLIENT_PLUGIN_INTERFACE_VERSION);
			Msg("* URL:              https://github.com/sw1ft747/SvenMod\n");
			Msg("* Compiled in:      %s\n", SVENMOD_BUILD_TIMESTAMP);
			Msg("----------------------------------\n");
		}
		else
		{
			sm_command.PrintUsage();

			Msg("* sm plugins - Plugin commands\n");
			Msg("* sm info    - Query info\n");
		}
	}
	else
	{
		sm_command.PrintUsage();

		Msg("* sm plugins - Plugin commands\n");
		Msg("* sm info    - Query info\n");
	}
}