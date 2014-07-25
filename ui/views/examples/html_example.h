
#ifndef __ui_views_example_demo_html_h__
#define __ui_views_example_demo_html_h__

#pragma once

#include "ui/views/controls/button/button.h"
#include "ui/views/examples/example_base.h"

namespace views{
class HtmlView;
class TextButton;
class Textfield;

namespace examples {
class DemoHtml : public ExampleBase, public views::ButtonListener{
public:
    DemoHtml();
    virtual ~DemoHtml();

		// Overridden from ExampleBase.
		virtual void CreateExampleView(View* container) OVERRIDE;

    // Overridden from view::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,const ui::Event& event) OVERRIDE;

private:
    views::HtmlView* html_view_;
    views::Textfield* html_;
    views::TextButton* button_;

    DISALLOW_COPY_AND_ASSIGN(DemoHtml);
};

}
}

#endif //__ui_views_example_demo_html_h__