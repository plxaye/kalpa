
#include "ui/views/examples/html_example.h"

#include "base/strings/sys_string_conversions.h"
#include "ui/views/controls/activex/html_view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"

namespace views {
namespace examples {

DemoHtml::DemoHtml() : ExampleBase("DemoHtml"),
html_view_(NULL), html_(NULL), button_(NULL) {}

DemoHtml::~DemoHtml() {}

void DemoHtml::CreateExampleView(views::View* container)
{
    views::GridLayout* layout = new views::GridLayout(container);
    container->SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);

    views::ColumnSet* column_set1 = layout->AddColumnSet(1);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        1.0f, views::GridLayout::USE_PREF, 0, 0);
    column_set1->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL,
        0.0f, views::GridLayout::USE_PREF, 0, 0);

    layout->StartRow(1.0f, 0);
    html_view_ = new views::HtmlView();
    layout->AddView(html_view_);

    layout->StartRow(0.0f, 1);
    html_ = new views::Textfield();
    html_->SetText(L"<hr><p>I'm hege</p><input/>");
    layout->AddView(html_);
    button_ = new views::TextButton(this, L"��Ⱦ");
    layout->AddView(button_);
}

void DemoHtml::ButtonPressed(views::Button* sender,
                             const ui::Event& event)
{
    if(button_ == sender)
    {
        html_view_->SetHtml(base::SysWideToNativeMB(html_->text()));
    }
}

}
}
