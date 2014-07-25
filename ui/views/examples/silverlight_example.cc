
#include "ui/views/examples/silverlight_example.h"

#include "base/strings/sys_string_conversions.h"
#include "ui/views/controls/activex/silverlight_view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/layout/grid_layout.h"

namespace views {
namespace examples {

DemoSilverlight::DemoSilverlight() : ExampleBase("DemoSilverlight"),
silverlight_view1_(NULL), silverlight_view2_(NULL), button_(NULL) {}

DemoSilverlight::~DemoSilverlight() {}

void DemoSilverlight::CreateExampleView(views::View* container)
{
    views::GridLayout* layout = new views::GridLayout(container);
    container->SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);

    views::ColumnSet* column_set1 = layout->AddColumnSet(1);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);

    layout->StartRow(1.0f, 0);
    silverlight_view1_ = new views::SilverlightView();
    layout->AddView(silverlight_view1_);
    silverlight_view2_ = new views::SilverlightView();
    layout->AddView(silverlight_view2_);

    layout->StartRow(0.0f, 1);
    button_ = new views::TextButton(this, L"����Silverlight�ļ�");
    layout->AddView(button_);
}

void DemoSilverlight::ButtonPressed(views::Button* sender,
                                    const ui::Event& event)
{
    if(button_ == sender)
    {
        silverlight_view1_->Play(L"http://www.andybeaulieu.com/silverlight/2.0/sortthefoobars/ClientBin/SortTheFoobars.xap");
        silverlight_view1_->Play(L"http://www.andybeaulieu.com/silverlight/2.0/sortthefoobars/ClientBin/SortTheFoobars.xap");
    }
}

}
}
