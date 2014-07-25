
#include "ui/views/examples/webbrowser_example.h"

#include "ui/views/controls/activex/webbrowser_view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"

namespace views {
namespace examples {

DemoWebBrowser::DemoWebBrowser() : ExampleBase("DemoWebBrowser"),
web_view_(NULL), web_address_(NULL), button_(NULL) {}

DemoWebBrowser::~DemoWebBrowser() {}

void DemoWebBrowser::CreateExampleView(views::View* container)
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
    web_view_ = new views::WebBrowserView();
    layout->AddView(web_view_);

    layout->StartRow(0.0f, 1);
    web_address_ = new views::Textfield();
    web_address_->SetText(L"http://src.chromium.org/viewvc/chrome/trunk/src/ui/views/view.h?view=log");
    layout->AddView(web_address_);
    button_ = new views::TextButton(this, L"����");
    layout->AddView(button_);
}

void DemoWebBrowser::ButtonPressed(views::Button* sender,
                                   const ui::Event& event)
{
    if(button_ == sender)
    {
        web_view_->Navigate(web_address_->text());
    }
}

}  // namespace examples
}  // namespace views
