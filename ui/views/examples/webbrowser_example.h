
#ifndef __demo_webbrowser_h__
#define __demo_webbrowser_h__

#include "ui/views/controls/button/button.h"
#include "ui/views/examples/example_base.h"

namespace views{
class TextButton;
class Textfield;
class WebBrowserView;

namespace examples {
class DemoWebBrowser : public ExampleBase, public views::ButtonListener
{
public:
    DemoWebBrowser();
    virtual ~DemoWebBrowser();

		// Overridden from ExampleBase.
		virtual void CreateExampleView(View* container) OVERRIDE;

    // Overridden from views::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,const ui::Event& event) OVERRIDE;

private:
    views::WebBrowserView* web_view_;
    views::Textfield* web_address_;
    views::TextButton* button_;

    DISALLOW_COPY_AND_ASSIGN(DemoWebBrowser);
};

}  // namespace examples
}  // namespace views

#endif //__demo_webbrowser_h__