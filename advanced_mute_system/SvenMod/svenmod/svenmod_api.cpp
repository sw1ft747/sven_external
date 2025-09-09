#include <ISvenModAPI.h>
#include <svenmod_version.h>

class CSvenModAPI : public ISvenModAPI
{
public:
	//-----------------------------------------------------------------------------
	// Basic info
	//-----------------------------------------------------------------------------

	virtual void					GetVersion(int *major, int *minor);

	virtual int						GetEngineBuild();
	virtual int						GetProtocolVersion();
	virtual client_version_s		*GetClientVersion();

	virtual int						GetClientState();

	virtual const char				*GetBaseDirectory();

	//-----------------------------------------------------------------------------
	// Application's API
	//-----------------------------------------------------------------------------

	virtual ICommandLine			*CommandLine();
	virtual IFileSystem				*FileSystem();
	virtual IRegistry				*Registry();

	//-----------------------------------------------------------------------------
	// SvenMod's API
	//-----------------------------------------------------------------------------

	virtual IMemoryUtils			*MemoryUtils();
	virtual IDetoursAPI				*DetoursAPI();
	virtual ILoggingSystem			*LoggingSystem();
	virtual IConfigManager			*ConfigManager();
	virtual IPluginHelpers			*PluginHelpers();

	virtual modules_s				*Modules();

	virtual ICvar					*CVar();
	virtual IHooks					*Hooks();

	virtual IPlayerUtils			*PlayerUtils();
	virtual IInventory				*Inventory();

	//-----------------------------------------------------------------------------
	// Game's API
	//-----------------------------------------------------------------------------

	virtual IVGUI					*VGUI();
	virtual IVGameUI				*VGameUI();

	virtual cl_clientfuncs_t		*ClientFuncs();
	virtual cl_enginefuncs_t		*EngineFuncs();

	virtual IEngineClient			*EngineClient();

	virtual engine_studio_api_t		*EngineStudio();
	virtual r_studio_interface_t	*StudioAPI();
	virtual CStudioModelRenderer	*StudioRenderer();

	virtual triangleapi_t			*TriangleAPI();
	virtual efx_api_t				*EffectsAPI();
	virtual event_api_t				*EventAPI();
	virtual demo_api_t				*DemoAPI();
	virtual net_api_t				*NetAPI();
	virtual IVoiceTweak				*VoiceTweak();

	virtual playermove_t			*PlayerMove();

	virtual IVideoMode				*VideoMode();
};

//-----------------------------------------------------------------------------
// Version query
//-----------------------------------------------------------------------------

void CSvenModAPI::GetVersion(int *major, int *minor)
{
	if (major)
		*major = SVENMOD_API_MAJOR_VERSION;

	if (minor)
		*minor = SVENMOD_API_MINOR_VERSION;
}

int CSvenModAPI::GetEngineBuild()
{
	extern int g_iEngineBuild;

	return g_iEngineBuild;
}

int CSvenModAPI::GetProtocolVersion()
{
	extern int g_iProtocolVersion;

	return g_iProtocolVersion;
}

client_version_s *CSvenModAPI::GetClientVersion()
{
	extern client_version_s g_ClientVersion;

	return &g_ClientVersion;
}

int CSvenModAPI::GetClientState()
{
	extern int *g_pClientState;

	return *g_pClientState;
}

const char *CSvenModAPI::GetBaseDirectory()
{
	extern const char *g_pszBaseDirectory;

	return g_pszBaseDirectory;
}

//-----------------------------------------------------------------------------
// Application's API
//-----------------------------------------------------------------------------

ICommandLine *CSvenModAPI::CommandLine()
{
	return g_pCommandLine;
}

IFileSystem *CSvenModAPI::FileSystem()
{
	return g_pFileSystem;
}

IRegistry *CSvenModAPI::Registry()
{
	return g_pRegistry;
}

//-----------------------------------------------------------------------------
// SvenMod's API
//-----------------------------------------------------------------------------

IMemoryUtils *CSvenModAPI::MemoryUtils()
{
	return g_pMemoryUtils;
}

IDetoursAPI *CSvenModAPI::DetoursAPI()
{
	return g_pDetoursAPI;
}

ILoggingSystem *CSvenModAPI::LoggingSystem()
{
	return g_pLoggingSystem;
}

IConfigManager *CSvenModAPI::ConfigManager()
{
	return g_pConfigManager;
}

IPluginHelpers *CSvenModAPI::PluginHelpers()
{
	return g_pPluginHelpers;
}

modules_s *CSvenModAPI::Modules()
{
	extern modules_s g_Modules;

	return &g_Modules;
}

ICvar *CSvenModAPI::CVar()
{
	return g_pCVar;
}

IHooks *CSvenModAPI::Hooks()
{
	return g_pHooks;
}

IPlayerUtils *CSvenModAPI::PlayerUtils()
{
	return g_pPlayerUtils;
}

IInventory *CSvenModAPI::Inventory()
{
	return g_pInventory;
}

//-----------------------------------------------------------------------------
// Game's API
//-----------------------------------------------------------------------------

IVGUI *CSvenModAPI::VGUI()
{
	return g_pVGUI;
}

IVGameUI *CSvenModAPI::VGameUI()
{
	return g_pVGameUI;
}

cl_clientfuncs_t *CSvenModAPI::ClientFuncs()
{
	return g_pClientFuncs;
}

cl_enginefuncs_t *CSvenModAPI::EngineFuncs()
{
	return g_pEngineFuncs;
}

IEngineClient *CSvenModAPI::EngineClient()
{
	return g_pEngineClient;
}

engine_studio_api_t *CSvenModAPI::EngineStudio()
{
	return g_pEngineStudio;
}

r_studio_interface_t *CSvenModAPI::StudioAPI()
{
	return g_pStudioAPI;
}

CStudioModelRenderer *CSvenModAPI::StudioRenderer()
{
	return g_pStudioRenderer;
}

triangleapi_t *CSvenModAPI::TriangleAPI()
{
	return g_pTriangleAPI;
}

efx_api_t *CSvenModAPI::EffectsAPI()
{
	return g_pEffectsAPI;
}

event_api_t *CSvenModAPI::EventAPI()
{
	return g_pEventAPI;
}

demo_api_t *CSvenModAPI::DemoAPI()
{
	return g_pDemoAPI;
}

net_api_t *CSvenModAPI::NetAPI()
{
	return g_pNetAPI;
}

IVoiceTweak *CSvenModAPI::VoiceTweak()
{
	return g_pVoiceTweak;
}

playermove_t *CSvenModAPI::PlayerMove()
{
	return g_pPlayerMove;
}

IVideoMode *CSvenModAPI::VideoMode()
{
	return ::VideoMode();
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CSvenModAPI g_SvenModAPI;
ISvenModAPI *g_pSvenModAPI = &g_SvenModAPI;

ISvenModAPI *SvenModAPI()
{
	return g_pSvenModAPI;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSvenModAPI, ISvenModAPI, SVENMOD_API_INTERFACE_VERSION, g_SvenModAPI);