#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Widget.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/ui/UiContext.h>
#include "Theme.h"
#include "Window.h"

#define RCT2_NEW_WINDOW         (gWindowNextSlot)
#define RCT2_LAST_WINDOW        (gWindowNextSlot - 1)

using namespace OpenRCT2;

static bool window_fits_between_others(sint32 x, sint32 y, sint32 width, sint32 height)
{
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        if (x + width <= w->x) continue;
        if (x >= w->x + w->width) continue;
        if (y + height <= w->y) continue;
        if (y >= w->y + w->height) continue;
        return false;
    }

    return true;
}

static bool window_fits_within_space(sint32 x, sint32 y, sint32 width, sint32 height)
{
    if (x < 0) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    if (x + width > context_get_width()) return false;
    if (y + height > context_get_height()) return false;
    return window_fits_between_others(x, y, width, height);
}

static bool window_fits_on_screen(sint32 x, sint32 y, sint32 width, sint32 height)
{
    uint16 screenWidth = context_get_width();
    uint16 screenHeight = context_get_height();
    sint32 unk;

    unk = -(width / 4);
    if (x < unk) return false;
    unk = screenWidth + (unk * 2);
    if (x > unk) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    unk = screenHeight - (height / 4);
    if (y > unk) return false;
    return window_fits_between_others(x, y, width, height);
}

rct_window *window_create(sint32 x, sint32 y, sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    // Check if there are any window slots left
    // include WINDOW_LIMIT_RESERVED for items such as the main viewport and toolbars to not appear to be counted.
    if (RCT2_NEW_WINDOW >= &(g_window_list[gConfigGeneral.window_limit + WINDOW_LIMIT_RESERVED])) {
        rct_window *w = nullptr;
        // Close least recently used window
        for (w = g_window_list; w < RCT2_NEW_WINDOW; w++)
            if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
                break;

        window_close(w);
    }

    rct_window *w = RCT2_NEW_WINDOW;

    // Flags
    if (flags & WF_STICK_TO_BACK) {
        for (; w >= g_window_list + 1; w--) {
            if ((w - 1)->flags & WF_STICK_TO_FRONT)
                continue;
            if ((w - 1)->flags & WF_STICK_TO_BACK)
                break;
        }
    }
    else if (!(flags & WF_STICK_TO_FRONT)) {
        for (; w >= g_window_list + 1; w--) {
            if (!((w - 1)->flags & WF_STICK_TO_FRONT))
                break;
        }
    }

    // Move w to new window slot
    if (w != RCT2_NEW_WINDOW)
        *RCT2_NEW_WINDOW = *w;

    // Setup window
    w->classification = cls;
    w->var_4B8 = -1;
    w->var_4B9 = -1;
    w->flags = flags;

    // Play sounds and flash the window
    if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))) {
        w->flags |= WF_WHITE_BORDER_MASK;
        audio_play_sound(SOUND_WINDOW_OPEN, 0, x + (width / 2));
    }

    w->number = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->viewport = nullptr;
    w->event_handlers = event_handlers;
    w->enabled_widgets = 0;
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;
    w->hold_down_widgets = 0;
    w->viewport_focus_coordinates.var_480 = 0;
    w->viewport_focus_coordinates.x = 0;
    w->viewport_focus_coordinates.y = 0;
    w->viewport_focus_coordinates.z = 0;
    w->viewport_focus_coordinates.rotation = 0;
    w->page = 0;
    w->var_48C = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->var_492 = 0;
    w->selected_tab = 0;
    w->var_4AE = 0;
    w->viewport_smart_follow_sprite = SPRITE_INDEX_NULL;
    RCT2_NEW_WINDOW++;

    colour_scheme_update(w);
    window_invalidate(w);
    return w;
}

rct_window *window_create_auto_pos(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    // TODO dead code, looks like it is cascading the new window offset from an existing window
    // we will have to re-implement this in our own way.
    //
    // if (cls & 0x80) {
    //  cls &= ~0x80;
    //  rct_window *w = window_find_by_number(0, 0);
    //  if (w != nullptr) {
    //      if (w->x > -60 && w->x < screenWidth - 20) {
    //          if (w->y < screenHeight - 20) {
    //              sint32 x = w->x;
    //              if (w->x + width > screenWidth)
    //                  x = screenWidth - 20 - width;
    //              sint32 y = w->y;
    //              return window_create(x + 10, y + 10, width, height, event_handlers, cls, flags);
    //          }
    //      }
    //  }
    // }

    // Place window in an empty corner of the screen
    sint32 x = 0;
    sint32 y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = 0;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    // Place window next to another
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;
    }

    // Overlap
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;
    }

    // Cascade
    x = 0;
    y = 30;
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (x != w->x || y != w->y)
            continue;

        x += 5;
        y += 5;
    }

    // Clamp to inside the screen
foundSpace:
    if (x < 0)
        x = 0;
    if (x + width > screenWidth)
        x = screenWidth - width;

    return window_create(x, y, width, height, event_handlers, cls, flags);
}

rct_window * window_create_centred(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    sint32 x = (screenWidth - width) / 2;
    sint32 y = std::max(TOP_TOOLBAR_HEIGHT + 1, (screenHeight - height) / 2);
    return window_create(x, y, width, height, event_handlers, cls, flags);
}

/**
 *
 *  rct2: 0x006EB15C
 *
 * @param window (esi)
 * @param dpi (edi)
 */
void window_draw_widgets(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_widget *widget;
    rct_widgetindex widgetIndex;

    if ((w->flags & WF_TRANSPARENT) && !(w->flags & WF_NO_BACKGROUND))
        gfx_filter_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + w->height - 1, PALETTE_51);

    //todo: some code missing here? Between 006EB18C and 006EB260

    widgetIndex = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
        // Check if widget is outside the draw region
        if (w->x + widget->left < dpi->x + dpi->width && w->x + widget->right >= dpi->x)
            if (w->y + widget->top < dpi->y + dpi->height && w->y + widget->bottom >= dpi->y)
                widget_draw(dpi, w, widgetIndex);

        widgetIndex++;
    }

    //todo: something missing here too? Between 006EC32B and 006EC369

    if (w->flags & WF_WHITE_BORDER_MASK) {
        gfx_fill_rect_inset(dpi, w->x, w->y, w->x + w->width - 1, w->y + w->height - 1, COLOUR_WHITE, INSET_RECT_FLAG_FILL_NONE);
    }
}

/**
 *
 *  rct2: 0x006EA776
 */
static void window_invalidate_pressed_image_buttons(rct_window *w)
{
    rct_widgetindex widgetIndex;
    rct_widget *widget;

    widgetIndex = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++, widgetIndex++) {
        if (widget->type != WWT_IMGBTN)
            continue;

        if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
            gfx_set_dirty_blocks(w->x, w->y, w->x + w->width, w->y + w->height);
    }
}

/**
 *
 *  rct2: 0x006EA73F
 */
void invalidate_all_windows_after_input()
{
    for (rct_window *w = RCT2_LAST_WINDOW; w >= g_window_list; w--) {
        window_update_scroll_widgets(w);
        window_invalidate_pressed_image_buttons(w);
        window_event_resize_call(w);
    }
}

/**
* Initialises scroll widgets to their virtual size.
*  rct2: 0x006EAEB8
*
* @param window The window (esi).
*/
void window_init_scroll_widgets(rct_window *w)
{
    rct_widget* widget;
    rct_scroll* scroll;
    sint32 widget_index, scroll_index;
    sint32 width, height;

    widget_index = 0;
    scroll_index = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
        if (widget->type != WWT_SCROLL) {
            widget_index++;
            continue;
        }

        scroll = &w->scrolls[scroll_index];
        scroll->flags = 0;
        width = 0;
        height = 0;
        window_get_scroll_size(w, scroll_index, &width, &height);
        scroll->h_left = 0;
        scroll->h_right = width + 1;
        scroll->v_top = 0;
        scroll->v_bottom = height + 1;

        if (widget->content & SCROLL_HORIZONTAL)
            scroll->flags |= HSCROLLBAR_VISIBLE;
        if (widget->content & SCROLL_VERTICAL)
            scroll->flags |= VSCROLLBAR_VISIBLE;

        widget_scroll_update_thumbs(w, widget_index);

        widget_index++;
        scroll_index++;
    }
}
