
#include "ui/views/controls/activex/mediaplay_view.h"

#include "base/win/scoped_bstr.h"
#include "skia/ext/platform_canvas.h"
#include "ui/gfx/canvas.h"
#include "ui/views/widget/widget.h"

namespace views
{

    MediaPlayerView::MediaPlayerView() : ax_host_(new AxHost(this)),
        initialized_(false) {}

    MediaPlayerView::~MediaPlayerView() {}

    bool MediaPlayerView::Play(const std::wstring& url)
    {
        if(!wmp_)
        {
            return false;
        }

        wmp_->close();
        base::win::ScopedBstr scoped_url(url.c_str());
        wmp_->put_URL(scoped_url);
        return true;
    }

    // Overridden from View:
/* todo(hege)
    bool MediaPlayerView::OnSetCursor(const gfx::Point& p)
    {
        gfx::Point point(p);
        ConvertPointToWidget(this, &point);
        return ax_host_->OnSetCursor(point);
    }
*/
    bool MediaPlayerView::OnMousePressed(const ui::MouseEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowlessMouseMessage(msg.message, msg.wParam, msg.lParam);
        return true;
    }

    bool MediaPlayerView::OnMouseDragged(const ui::MouseEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowlessMouseMessage(msg.message, msg.wParam, msg.lParam);
        return true;
    }

    void MediaPlayerView::OnMouseReleased(const ui::MouseEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowlessMouseMessage(msg.message, msg.wParam, msg.lParam);
    }

    void MediaPlayerView::OnMouseMoved(const ui::MouseEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowlessMouseMessage(msg.message, msg.wParam, msg.lParam);
    }

    bool MediaPlayerView::OnKeyPressed(const ui::KeyEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowMessage(msg.message, msg.wParam, msg.lParam);
        return true;
    }

    bool MediaPlayerView::OnKeyReleased(const ui::KeyEvent& event)
    {
        MSG msg = event.native_event();
        ax_host_->OnWindowMessage(msg.message, msg.wParam, msg.lParam);
        return true;
    }

    // Overridden from View:
    void MediaPlayerView::OnPaint(gfx::Canvas* canvas)
    {
        HDC dc = canvas->BeginPlatformPaint();

        skia::PlatformCanvas* platform_canvas = skia::CreatePlatformCanvas(width(), height(), true);
        HDC mem_dc = skia::BeginPlatformPaint(platform_canvas);
        ax_host_->Draw(mem_dc, GetLocalBounds());
        skia::EndPlatformPaint(platform_canvas);

        skia::DrawToNativeContext(platform_canvas, dc, 0, 0,&GetLocalBounds().ToRECT());
				delete platform_canvas;
	
        canvas->EndPlatformPaint();
    }

    void MediaPlayerView::Layout()
    {
        View::Layout();
        gfx::Rect rect = View::ConvertRectToWidget(GetLocalBounds());
        ax_host_->SetRect(rect);
    }

    void MediaPlayerView::ViewHierarchyChanged(const ViewHierarchyChangedDetails& details)
    {
        if(details.is_add && !initialized_ && GetWidget())
        {
            initialized_ = true;
            ax_host_->CreateControl(__uuidof(WMPLib::WindowsMediaPlayer));
        }
    }

    // Overridden from AxHostDelegate:
    HWND MediaPlayerView::GetAxHostWindow() const
    {
        if(!GetWidget())
        {
            return NULL;
        }

        return GetWidget()->GetNativeView();
    }

    void MediaPlayerView::OnAxCreate(AxHost* host)
    {
        DCHECK_EQ(ax_host_.get(), host);

        HRESULT hr = wmp_.QueryFrom(ax_host_->activex_control());
        if(wmp_)
        {
            hr = wmp_->put_windowlessVideo(VARIANT_TRUE);
        }
    }

    void MediaPlayerView::OnAxInvalidate(const gfx::Rect& rect)
    {
        gfx::Point origin = rect.origin();
        ConvertPointFromWidget(this, &origin);
        gfx::Rect invalidate = rect;
				invalidate.set_origin(origin);
        SchedulePaintInRect(invalidate);
    }

} //namespace view