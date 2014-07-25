
#ifndef __demo_silverlight_h__
#define __demo_silverlight_h__

#include "ui/views/controls/button/button.h"
#include "ui/views/examples/example_base.h"

namespace views{
class TextButton;
class SilverlightView;

namespace examples {
class DemoSilverlight : public ExampleBase, public views::ButtonListener
{
public:
    DemoSilverlight();
    virtual ~DemoSilverlight();

		// Overridden from ExampleBase.
		virtual void CreateExampleView(View* container) OVERRIDE;

    // Overridden from views::ButtonListener:
    virtual void ButtonPressed(views::Button* sender,const ui::Event& event) OVERRIDE;

private:
    views::SilverlightView* silverlight_view1_;
    views::SilverlightView* silverlight_view2_;
    views::TextButton* button_;

    DISALLOW_COPY_AND_ASSIGN(DemoSilverlight);
};

}
}

#endif //__demo_silverlight_h__