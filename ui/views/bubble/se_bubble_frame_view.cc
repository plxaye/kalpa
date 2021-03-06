// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/bubble/se_bubble_frame_view.h"

#include <algorithm>

#include "grit/ui_resources.h"
#include "ui/base/hit_test.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/path.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/skia_util.h"
#include "ui/views/bubble/bubble_border.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/window/client_view.h"

namespace {

// Padding, in pixels, for the title view, when it exists.
const int kTitleTopInset = 0;
const int kTitleLeftInset = 0;
const int kTitleBottomInset = 12;

const int kOpacity_border = 10;

// Get the |vertical| or horizontal screen overflow of the |window_bounds|.
int GetOffScreenLength(const gfx::Rect& monitor_bounds,
                       const gfx::Rect& window_bounds,
                       bool vertical) {
  if (monitor_bounds.IsEmpty() || monitor_bounds.Contains(window_bounds))
    return 0;

  //  window_bounds
  //  +-------------------------------+
  //  |             top               |
  //  |      +----------------+       |
  //  | left | monitor_bounds | right |
  //  |      +----------------+       |
  //  |            bottom             |
  //  +-------------------------------+
  if (vertical)
    return std::max(0, monitor_bounds.y() - window_bounds.y()) +
           std::max(0, window_bounds.bottom() - monitor_bounds.bottom());
  return std::max(0, monitor_bounds.x() - window_bounds.x()) +
         std::max(0, window_bounds.right() - monitor_bounds.right());
}

}  // namespace

namespace views {

// static
const char SeBubbleFrameView::kViewClassName[] = "SeBubbleFrameView";

// static
gfx::Insets SeBubbleFrameView::GetTitleInsets() {
  return gfx::Insets(kTitleTopInset, kTitleLeftInset, kTitleBottomInset, 0);
}

SeBubbleFrameView::SeBubbleFrameView(const gfx::Insets& content_margins)
    : bubble_border_(NULL),
      content_margins_(content_margins),
      title_(NULL),
      close_(NULL),
      titlebar_extra_view_(NULL) {
  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  title_view_ = new View;
  title_view_->set_background(Background::CreateSolidBackground(246, 246, 246));
  AddChildView(title_view_);

  title_ = new Label(string16(), rb.GetFont(ui::ResourceBundle::BaseFont));
  title_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  title_->SetAutoColorReadabilityEnabled(false);
  title_->SetEnabledColor(SkColorSetRGB(76,76,76));
  AddChildView(title_);

  close_ = new LabelButton(this, string16());
  close_->SetImage(CustomButton::STATE_NORMAL,
                   *rb.GetImageNamed(IDR_SE_DIALOG_CLOSE_BTN_N).ToImageSkia());
  close_->SetImage(CustomButton::STATE_HOVERED,
                   *rb.GetImageNamed(IDR_SE_DIALOG_CLOSE_BTN_H).ToImageSkia());
  close_->SetImage(CustomButton::STATE_PRESSED,
                   *rb.GetImageNamed(IDR_SE_DIALOG_CLOSE_BTN_P).ToImageSkia());
  close_->SetSize(close_->GetPreferredSize());
  close_->set_border(NULL);
  close_->SetVisible(false);
  AddChildView(close_);
}

SeBubbleFrameView::~SeBubbleFrameView() {}

gfx::Rect SeBubbleFrameView::GetBoundsForClientView() const {
  gfx::Rect client_bounds = GetLocalBounds();
  client_bounds.Inset(GetInsets());
  client_bounds.Inset(bubble_border_->GetInsets());
  return client_bounds;
}

gfx::Rect SeBubbleFrameView::GetWindowBoundsForClientBounds(
    const gfx::Rect& client_bounds) const {
  return const_cast<SeBubbleFrameView*>(this)->GetUpdatedWindowBounds(
      gfx::Rect(), client_bounds.size(), false);
}

int SeBubbleFrameView::NonClientHitTest(const gfx::Point& point) {
  if (!bounds().Contains(point))
    return HTNOWHERE;
  if (close_->visible() && close_->GetMirroredBounds().Contains(point))
    return HTCLOSE;

  // Allow dialogs to show the system menu and be dragged.
  if (GetWidget()->widget_delegate()->AsDialogDelegate()) {
    gfx::Rect sys_rect(0, 0, title_->x(), title_->y());
    sys_rect.set_origin(gfx::Point(GetMirroredXForRect(sys_rect), 0));
    if (sys_rect.Contains(point))
      return HTSYSMENU;
    if (point.y() < title_->bounds().bottom())
      return HTCAPTION;
  }

  return GetWidget()->client_view()->NonClientHitTest(point);
}

void SeBubbleFrameView::GetWindowMask(const gfx::Size& size,
                                    gfx::Path* window_mask) {
  // NOTE: this only provides implementations for the types used by dialogs.
  if ((bubble_border_->arrow() != BubbleBorder::NONE &&
       bubble_border_->arrow() != BubbleBorder::FLOAT) ||
      (bubble_border_->shadow() != BubbleBorder::SMALL_SHADOW &&
       bubble_border_->shadow() != BubbleBorder::NO_SHADOW_OPAQUE_BORDER))
    return;

  // Use a window mask roughly matching the border in the image assets.
  static const int kBorderStrokeSize = 1;
  static const SkScalar kCornerRadius = SkIntToScalar(6);
  const gfx::Insets border_insets = bubble_border_->GetInsets();
  SkRect rect = { SkIntToScalar(border_insets.left() - kBorderStrokeSize),
                  SkIntToScalar(border_insets.top() - kBorderStrokeSize),
                  SkIntToScalar(size.width() - border_insets.right() +
                                kBorderStrokeSize),
                  SkIntToScalar(size.height() - border_insets.bottom() +
                                kBorderStrokeSize) };
  if (bubble_border_->shadow() == BubbleBorder::NO_SHADOW_OPAQUE_BORDER) {
    window_mask->addRoundRect(rect, kCornerRadius, kCornerRadius);
  } else {
    static const int kBottomBorderShadowSize = 2;
    rect.fBottom += SkIntToScalar(kBottomBorderShadowSize);
    window_mask->addRect(rect);
  }
}

void SeBubbleFrameView::ResetWindowControls() {
  close_->SetVisible(GetWidget()->widget_delegate()->ShouldShowCloseButton());
}

void SeBubbleFrameView::UpdateWindowIcon() {}

void SeBubbleFrameView::UpdateWindowTitle() {
  title_->SetText(GetWidget()->widget_delegate()->ShouldShowWindowTitle() ?
      GetWidget()->widget_delegate()->GetWindowTitle() : string16());
  // Update the close button visibility too, otherwise it's not intialized.
  ResetWindowControls();
}

gfx::Insets SeBubbleFrameView::GetInsets() const {
  gfx::Insets insets = content_margins_;
  const int title_height = title_->text().empty() ? 0 :
      title_->GetPreferredSize().height() + kTitleTopInset + kTitleBottomInset;
  const int close_height = close_->visible() ? close_->height() : 0;
  insets += gfx::Insets(std::max(title_height, close_height), 0, 0, 0);
  return insets;
}

gfx::Size SeBubbleFrameView::GetPreferredSize() {
  gfx::Size preferred_size = GetSizeForClientSize(GetWidget()->client_view()->GetPreferredSize());
  return preferred_size;
}

gfx::Size SeBubbleFrameView::GetMinimumSize() {
  return GetSizeForClientSize(GetWidget()->client_view()->GetMinimumSize());
}

void SeBubbleFrameView::Layout() {
  gfx::Rect bounds(GetLocalBounds());
  bounds.Inset(border()->GetInsets());

  int opacity_border = kOpacity_border;
  if (bubble_border_->shadow() != BubbleBorder::SHADOW)
    opacity_border = 0;

  title_view_->SetBoundsRect(gfx::Rect(bounds.x() -opacity_border, bounds.y() -opacity_border,
    bounds.width() + opacity_border*2, GetInsets().height()-4));
  // Small additional insets yield the desired 10px visual close button insets.
  bounds.Inset(0, 0, close_->width() + 1, 0);
  close_->SetPosition(gfx::Point(bounds.right()+opacity_border+4, bounds.y() -(opacity_border+2)));

  gfx::Rect title_bounds(bounds);
  title_bounds.set_y(title_bounds.y()-opacity_border +
      (GetInsets().height()-title_->GetPreferredSize().height())/2);
  title_bounds.Inset(kTitleLeftInset, kTitleTopInset, 0, 0);
  gfx::Size title_size(title_->GetPreferredSize());
  const int title_width = std::max(0, close_->bounds().x() - title_bounds.x());
  title_size.SetToMin(gfx::Size(title_width, title_size.height()));
  title_bounds.set_x(title_bounds.x() + 1);
  title_bounds.set_y(title_bounds.y() - 2);
  title_bounds.set_size(title_size);
  title_->SetBoundsRect(title_bounds);

  if (titlebar_extra_view_) {
    const int extra_width = close_->bounds().x() - title_->bounds().right();
    gfx::Size size = titlebar_extra_view_->GetPreferredSize();
    size.SetToMin(gfx::Size(std::max(0, extra_width), size.height()));
    gfx::Rect titlebar_extra_view_bounds(
      bounds.right() - size.width(),
      title_bounds.y(),
      size.width(),
      title_bounds.height());
    titlebar_extra_view_bounds.Subtract(title_bounds);
    titlebar_extra_view_->SetBoundsRect(titlebar_extra_view_bounds);
  }
}

const char* SeBubbleFrameView::GetClassName() const {
  return kViewClassName;
}

void SeBubbleFrameView::ChildPreferredSizeChanged(View* child) {
  if (child == titlebar_extra_view_ || child == title_)
    Layout();
}

void SeBubbleFrameView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == close_)
    GetWidget()->Close();
}

void SeBubbleFrameView::SetBubbleBorder(BubbleBorder* border) {
  bubble_border_ = border;
  set_border(bubble_border_);

  // Update the background, which relies on the border.
  set_background(new views::BubbleBackground(border));
}

void SeBubbleFrameView::SetTitlebarExtraView(View* view) {
  DCHECK(view);
  DCHECK(!titlebar_extra_view_);
  AddChildView(view);
  titlebar_extra_view_ = view;
}

gfx::Rect SeBubbleFrameView::GetUpdatedWindowBounds(const gfx::Rect& anchor_rect,
                                                  gfx::Size client_size,
                                                  bool adjust_if_offscreen) {
  gfx::Insets insets(GetInsets());
  client_size.Enlarge(insets.width(), insets.height());

  const BubbleBorder::Arrow arrow = bubble_border_->arrow();
  if (adjust_if_offscreen && BubbleBorder::has_arrow(arrow)) {
    if (!bubble_border_->is_arrow_at_center(arrow)) {
      // Try to mirror the anchoring if the bubble does not fit on the screen.
      MirrorArrowIfOffScreen(true, anchor_rect, client_size);
      MirrorArrowIfOffScreen(false, anchor_rect, client_size);
    } else {
      // Mirror as needed vertically if the arrow is on a horizontal edge and
      // vice-versa.
      MirrorArrowIfOffScreen(BubbleBorder::is_arrow_on_horizontal(arrow),
                             anchor_rect,
                             client_size);
      OffsetArrowIfOffScreen(anchor_rect, client_size);
    }
  }

  // Calculate the bounds with the arrow in its updated location and offset.
  gfx::Rect windowBounds = bubble_border_->GetBounds(anchor_rect, client_size);
  return windowBounds;
}

gfx::Rect SeBubbleFrameView::GetMonitorBounds(const gfx::Rect& rect) {
  // TODO(scottmg): Native is wrong. http://crbug.com/133312
  gfx::Rect monitorbound = gfx::Screen::GetNativeScreen()->GetDisplayNearestPoint(
    rect.CenterPoint()).work_area();
  return monitorbound;
}

void SeBubbleFrameView::MirrorArrowIfOffScreen(
    bool vertical,
    const gfx::Rect& anchor_rect,
    const gfx::Size& client_size) {
  // Check if the bounds don't fit on screen.
  gfx::Rect monitor_rect(GetMonitorBounds(anchor_rect));
  gfx::Rect window_bounds(bubble_border_->GetBounds(anchor_rect, client_size));
  if (GetOffScreenLength(monitor_rect, window_bounds, vertical) > 0) {
    BubbleBorder::Arrow arrow = bubble_border()->arrow();
    // Mirror the arrow and get the new bounds.
    bubble_border_->set_arrow(
        vertical ? BubbleBorder::vertical_mirror(arrow) :
                   BubbleBorder::horizontal_mirror(arrow));
    gfx::Rect mirror_bounds =
        bubble_border_->GetBounds(anchor_rect, client_size);
    // Restore the original arrow if mirroring doesn't show more of the bubble.
    if (GetOffScreenLength(monitor_rect, mirror_bounds, vertical) >=
        GetOffScreenLength(monitor_rect, window_bounds, vertical))
      bubble_border_->set_arrow(arrow);
    else
      SchedulePaint();
  }
}

void SeBubbleFrameView::OffsetArrowIfOffScreen(const gfx::Rect& anchor_rect,
                                             const gfx::Size& client_size) {
  BubbleBorder::Arrow arrow = bubble_border()->arrow();
  DCHECK(BubbleBorder::is_arrow_at_center(arrow));

  // Get the desired bubble bounds without adjustment.
  bubble_border_->set_arrow_offset(0);
  gfx::Rect window_bounds(bubble_border_->GetBounds(anchor_rect, client_size));

  gfx::Rect monitor_rect(GetMonitorBounds(anchor_rect));
  if (monitor_rect.IsEmpty() || monitor_rect.Contains(window_bounds))
    return;

  // Calculate off-screen adjustment.
  const bool is_horizontal = BubbleBorder::is_arrow_on_horizontal(arrow);
  int offscreen_adjust = 0;
  if (is_horizontal) {
    if (window_bounds.x() < monitor_rect.x())
      offscreen_adjust = monitor_rect.x() - window_bounds.x();
    else if (window_bounds.right() > monitor_rect.right())
      offscreen_adjust = monitor_rect.right() - window_bounds.right();
  } else {
    if (window_bounds.y() < monitor_rect.y())
      offscreen_adjust = monitor_rect.y() - window_bounds.y();
    else if (window_bounds.bottom() > monitor_rect.bottom())
      offscreen_adjust = monitor_rect.bottom() - window_bounds.bottom();
  }

  // For center arrows, arrows are moved in the opposite direction of
  // |offscreen_adjust|, e.g. positive |offscreen_adjust| means bubble
  // window needs to be moved to the right and that means we need to move arrow
  // to the left, and that means negative offset.
  bubble_border_->set_arrow_offset(
      bubble_border_->GetArrowOffset(window_bounds.size()) - offscreen_adjust);
  if (offscreen_adjust)
    SchedulePaint();
}

gfx::Size SeBubbleFrameView::GetSizeForClientSize(const gfx::Size& client_size) {
  gfx::Size size(
      GetUpdatedWindowBounds(gfx::Rect(), client_size, false).size());
  // Accommodate the width of the title bar elements.
  int title_bar_width = GetInsets().width() + border()->GetInsets().width();
  if (!title_->text().empty())
    title_bar_width += kTitleLeftInset + title_->GetPreferredSize().width();
  if (close_->visible())
    title_bar_width += close_->width() + 1;
  if (titlebar_extra_view_ != NULL)
    title_bar_width += titlebar_extra_view_->GetPreferredSize().width();
  size.SetToMax(gfx::Size(title_bar_width, 0));
  return size;
}

}  // namespace views
