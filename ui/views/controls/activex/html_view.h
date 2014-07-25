
#ifndef __ui_views_controls_activex_html_view_h__
#define __ui_views_controls_activex_html_view_h__

#include <mshtml.h>

#include "ui/views/controls/activex/ax_host.h"
#include "ui/views/view.h"

namespace views
{

 	  // todo(hege),fix visiblechanged,maybe should use nativecontrolhost
    class VIEWS_EXPORT HtmlView : public View, public AxHostDelegate
    {
    public:
        explicit HtmlView();
        virtual ~HtmlView();

        bool SetHtml(const std::string& html);

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
        base::win::ScopedComPtr<IHTMLDocument2> html_document_;

        bool initialized_;
    };

} //namespace view

#endif //__view_html_view_h__