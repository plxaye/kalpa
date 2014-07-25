#ifndef __ui_views_example_demo_activex_h__
#define __ui_views_example_demo_activex_h__

#pragma once

#include "ui/views/controls/button/button.h"
#include "ui/views/examples/example_base.h"

namespace views {
class FlashView;
class MediaPlayerView;
class TextButton;

namespace examples {
class DemoActiveX : public ExampleBase, public views::ButtonListener
{
public:
    DemoActiveX();
    virtual ~DemoActiveX();

		// Overridden from ExampleBase.
		virtual void CreateExampleView(View* container) OVERRIDE;

    // Overridden from view::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,const ui::Event& event) OVERRIDE;

private:
    views::FlashView* flash_view_;
    views::MediaPlayerView* media_player_;
    views::TextButton* play_flash_;
    views::TextButton* play_wmv_;
    views::TextButton* create_flash_popup_;

    DISALLOW_COPY_AND_ASSIGN(DemoActiveX);
};

}
}

#endif //__ui_views_example_demo_activex_h__