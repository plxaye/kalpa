#include "tools/profiler/ui/exe_main_ui.h"

#include <atlbase.h>
#include <atlapp.h>
#include <malloc.h>
#include <new.h>
#include <shlobj.h>

#include "base/i18n/icu_util.h"
#include "base/path_service.h"
#include "tools/profiler/ui/profiler_views_delegate.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/gfx/gfx_paths.h"

//////////////////////////////////////////////////////////////////////////

namespace {

CAppModule _Module;

#pragma optimize("", off)
  void InvalidParameter(const wchar_t* expression, const wchar_t* function,
    const wchar_t* file, unsigned int line,
    uintptr_t reserved) 
  {
    __debugbreak();
    _exit(1);
  }

  void PureCall() 
  {
    __debugbreak();
    _exit(1);
  }

#pragma warning(push)
#pragma warning(disable : 4748)
  void OnNoMemory() {
    __debugbreak();
    _exit(1);
  }
#pragma warning(pop)
#pragma optimize("", on)

  void RegisterInvalidParamHandler() {
    _set_invalid_parameter_handler(InvalidParameter);
    _set_purecall_handler(PureCall);
    std::set_new_handler(&OnNoMemory);
    _set_new_mode(1);
  }

	views::profiler::ProfilerViewsDelegate* sViewsDelegate = NULL;
}  // namespace

//////////////////////////////////////////////////////////////////////////

namespace profiler_ui {

  void LowLevelInit(void* instance) {
		// mem
    RegisterInvalidParamHandler();

		// wtl
    _Module.Init(NULL, static_cast<HINSTANCE>(instance));

		// ICU
		base::i18n::InitializeICU();

		// Path Provider
		ui::RegisterPathProvider();
		gfx::RegisterPathProvider();

		// viewdelegate
		sViewsDelegate = new views::profiler::ProfilerViewsDelegate;;

		ui::ResourceBundle::InitSharedInstanceWithLocale("", NULL);
  }

  void LowLevelShutdown() {
		// viewdelegate
		delete sViewsDelegate;

		// ui
		ui::ResourceBundle::CleanupSharedInstance();
		ui::Clipboard::DestroyClipboardForCurrentThread(); //Note!!!
    
		// wtl
		_Module.Term();
  }
}

//////////////////////////////////////////////////////////////////////////