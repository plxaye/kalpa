
#ifndef __ui_views_controls_activex_mediaplayer_view_h__
#define __ui_views_controls_activex_mediaplayer_view_h__

#include "ui/views/controls/activex/wmp.tlh"

#include "ui/views/controls/activex/ax_host.h"
#include "ui/views/view.h"

namespace views
{
	  // todo(hege),fix visiblechanged,maybe should use nativecontrolhost
    class VIEWS_EXPORT MediaPlayerView : public View, public AxHostDelegate
    {
    public:
        explicit MediaPlayerView();
        virtual ~MediaPlayerView();

        WMPLib::IWMPPlayer4* wmp() const { return wmp_; }
        bool Play(const std::wstring& url);

        // Overridden from View:
        //virtual bool OnSetCursor(const gfx::Point& p) OVERRIDE;
        virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
        virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
        virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
        virtual void OnMouseMoved(const ui::MouseEvent& event) OVERRIDE;
        virtual bool OnKeyPressed(const ui::KeyEvent& event) OVERRIDE;
        virtual bool OnKeyReleased(const ui::KeyEvent& event) OVERRIDE;

    protected:
        // Overridden from View:
        virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
        virtual void Layout() OVERRIDE;
        virtual void ViewHierarchyChanged(const ViewHierarchyChangedDetails& details)OVERRIDE;

        // Overridden from AxHostDelegate:
        virtual HWND GetAxHostWindow() const OVERRIDE;
        virtual void OnAxCreate(AxHost* host) OVERRIDE;
        virtual void OnAxInvalidate(const gfx::Rect& rect) OVERRIDE;

    private:
        scoped_ptr<AxHost> ax_host_;
        base::win::ScopedComPtr<WMPLib::IWMPPlayer4> wmp_;

        bool initialized_;
    };

} //namespace view

#endif //__ui_views_controls_activex_mediaplayer_view_h__