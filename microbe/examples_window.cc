// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "microbe/examples_window.h"

#include "base/message_loop/message_loop.h"
#include "base/memory/scoped_vector.h"
#include "base/strings/utf_string_conversions.h"
#include "grit/app_resources.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/win/hwnd_util.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/window/custom_frame_view.h"

namespace views {
namespace microbe {

class ExamplesWindowContents : public WidgetDelegateView{
 public:
  ExamplesWindowContents(Operation operation)
      : status_label_(new Label),
        operation_(operation) {
    instance_ = this;
  }
  virtual ~ExamplesWindowContents() {}

  // Prints a message in the status area, at the bottom of the window.
  void SetStatus(const std::string& status) {
    status_label_->SetText(UTF8ToUTF16(status));
  }

  static ExamplesWindowContents* instance() { return instance_; }

 private:
  // Overridden from WidgetDelegateView:
  virtual bool CanResize() const OVERRIDE { return true; }
  virtual bool CanMaximize() const OVERRIDE { return true; }
  virtual string16 GetWindowTitle() const OVERRIDE {
    return ASCIIToUTF16("microbe");
  }
  virtual View* GetContentsView() OVERRIDE { return this; }
  virtual void WindowClosing() OVERRIDE {
    instance_ = NULL;
    if (operation_ == QUIT_ON_CLOSE)
      base::MessageLoopForUI::current()->Quit();
  }
	virtual NonClientFrameView* CreateNonClientFrameView(Widget* widget) OVERRIDE{
		widget->set_frame_type(Widget::FRAME_TYPE_FORCE_CUSTOM);
		CustomFrameView* custom_frame_view = new CustomFrameView;
		custom_frame_view->Init(widget);
		return custom_frame_view;
	}
	virtual gfx::ImageSkia GetWindowIcon() OVERRIDE{
		return *ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PRODUCT_LOGO_16);
	}
	virtual bool ShouldShowWindowIcon() const OVERRIDE{
		return true;
	}

  // Overridden from View:
  virtual void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) OVERRIDE {
    if (details.is_add && details.child == this)
      InitExamplesWindow();
  }

  // Creates the layout within the examples window.
  void InitExamplesWindow() {
    set_background(Background::CreateStandardPanelBackground());
    GridLayout* layout = new GridLayout(this);
    SetLayoutManager(layout);

		// left center right
		ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 5);
    column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                          GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 5);
    
		// top row
    layout->StartRow(0 /* no expand */, 0);
		layout->AddPaddingRow(0, 5);

		// center row
    layout->StartRow(1 /* expand */, 0);
    layout->AddView(status_label_);

		// bottom row
    layout->StartRow(0 /* no expand */, 0);
		layout->AddPaddingRow(0, 5);
  }

  static ExamplesWindowContents* instance_;
  Label* status_label_;
  const Operation operation_;

  DISALLOW_COPY_AND_ASSIGN(ExamplesWindowContents);
};

// static
ExamplesWindowContents* ExamplesWindowContents::instance_ = NULL;

void ShowExamplesWindow(Operation operation) {
  if (ExamplesWindowContents::instance()) {
    ExamplesWindowContents::instance()->GetWidget()->Activate();
  } else {
    Widget* window  = Widget::CreateWindowWithBounds(new ExamplesWindowContents(operation),gfx::Rect());
    gfx::CenterAndSizeWindow(NULL, window->GetNativeWindow(),gfx::Size(800, 600));
		window->Show();
  }
}

void LogStatus(const std::string& string) {
  ExamplesWindowContents::instance()->SetStatus(string);
}

}  // namespace microbe
}  // namespace views
