
#ifndef __ui_views_controls_activex_webbrowser_view_h__
#define __ui_views_controls_activex_webbrowser_view_h__

#include <windows.h>
#include <ole2.h>
#include <exdisp.h>

#include "ui/views/controls/activex/ax_host.h"
#include "ui/views/view.h"

namespace views
{

	  // todo(hege),fix visiblechanged,maybe should use nativecontrolhost
    class VIEWS_EXPORT WebBrowserView : public View, public AxHostDelegate
    {
    public:
        explicit WebBrowserView();
        virtual ~WebBrowserView();

        IWebBrowser2* webbrowser() const { return webbrowser_; }
        bool Navigate(const std::wstring& url);

    protected:
        // Overridden from View:
        virtual void Layout() OVERRIDE;
        virtual void ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) OVERRIDE;
        virtual void VisibilityChanged(View* starting_from, bool is_visible) OVERRIDE;

        // Overridden from AxHostDelegate:
        virtual HWND GetAxHostWindow() const OVERRIDE;
        virtual void OnAxCreate(AxHost* host) OVERRIDE;
        virtual void OnAxInvalidate(const gfx::Rect& rect) OVERRIDE;

    private:
        scoped_ptr<AxHost> ax_host_;
        base::win::ScopedComPtr<IWebBrowser2> webbrowser_;

        bool initialized_;
    };

} //namespace view

#endif //__ui_views_controls_activex_webbrowser_view_h__