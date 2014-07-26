// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/examples/examples_views_delegate.h"

#include "base/command_line.h"
#include "base/logging.h"

#include "grit/app_resources.h"
#include "ui/gfx/icon_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace views {
namespace examples {
ExamplesViewsDelegate::ExamplesViewsDelegate()
    : use_transparent_windows_(false) {
  DCHECK(!ViewsDelegate::views_delegate);
  ViewsDelegate::views_delegate = this;
}

ExamplesViewsDelegate::~ExamplesViewsDelegate() {
  ViewsDelegate::views_delegate = NULL;
}

void ExamplesViewsDelegate::SetUseTransparentWindows(bool transparent) {
  use_transparent_windows_ = transparent;
}

void ExamplesViewsDelegate::SaveWindowPlacement(const Widget* window,
                                            const std::string& window_name,
                                            const gfx::Rect& bounds,
                                            ui::WindowShowState show_state) {
}

bool ExamplesViewsDelegate::GetSavedWindowPlacement(
    const std::string& window_name,
    gfx::Rect* bounds,
    ui:: WindowShowState* show_state) const {
  return false;
}

NonClientFrameView* ExamplesViewsDelegate::CreateDefaultNonClientFrameView(
    Widget* widget) {
  return NULL;
}

bool ExamplesViewsDelegate::UseTransparentWindows() const {
  return use_transparent_windows_;
}
/*
content::WebContents* ExamplesViewsDelegate::CreateWebContents(
    content::BrowserContext* browser_context,
    content::SiteInstance* site_instance) {
  return NULL;
}
*/

void ExamplesViewsDelegate::OnBeforeWidgetInit(
    Widget::InitParams* params,
    internal::NativeWidgetDelegate* delegate) {
}

base::TimeDelta ExamplesViewsDelegate::GetDefaultTextfieldObscuredRevealDuration() {
  return base::TimeDelta();
}

HICON ExamplesViewsDelegate::GetDefaultWindowIcon() const{
	return IconUtil::CreateHICONFromSkBitmap(
		ui::ResourceBundle::GetSharedInstance().GetImageNamed(
		IDR_PRODUCT_LOGO_16).AsBitmap());
}

}
}  // namespace views
