// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tools/profiler/ui/profiler_views_delegate.h"

#include "base/command_line.h"
#include "base/logging.h"

#include "grit/app_resources.h"
#include "ui/gfx/icon_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace views {
namespace profiler {

ProfilerViewsDelegate::ProfilerViewsDelegate()
    : use_transparent_windows_(false) {
  DCHECK(!ViewsDelegate::views_delegate);
  ViewsDelegate::views_delegate = this;
}

ProfilerViewsDelegate::~ProfilerViewsDelegate() {
  ViewsDelegate::views_delegate = NULL;
}

void ProfilerViewsDelegate::SetUseTransparentWindows(bool transparent) {
  use_transparent_windows_ = transparent;
}

void ProfilerViewsDelegate::SaveWindowPlacement(const Widget* window,
                                            const std::string& window_name,
                                            const gfx::Rect& bounds,
                                            ui::WindowShowState show_state) {
}

bool ProfilerViewsDelegate::GetSavedWindowPlacement(
    const std::string& window_name,
    gfx::Rect* bounds,
    ui:: WindowShowState* show_state) const {
  return false;
}

NonClientFrameView* ProfilerViewsDelegate::CreateDefaultNonClientFrameView(
    Widget* widget) {
  return NULL;
}

bool ProfilerViewsDelegate::UseTransparentWindows() const {
  return use_transparent_windows_;
}
/*
content::WebContents* ProfilerViewsDelegate::CreateWebContents(
    content::BrowserContext* browser_context,
    content::SiteInstance* site_instance) {
  return NULL;
}
*/

void ProfilerViewsDelegate::OnBeforeWidgetInit(
    Widget::InitParams* params,
    internal::NativeWidgetDelegate* delegate) {
}

base::TimeDelta ProfilerViewsDelegate::GetDefaultTextfieldObscuredRevealDuration() {
  return base::TimeDelta();
}

HICON ProfilerViewsDelegate::GetDefaultWindowIcon() const{
	return IconUtil::CreateHICONFromSkBitmap(
		ui::ResourceBundle::GetSharedInstance().GetImageNamed(
		IDR_PRODUCT_LOGO_16).AsBitmap());
}

}
}  // namespace views
