#pragma once

#include <functional>

#include "../debug.hpp"
#include "../overlay.hpp"
#include "controller.hpp"
#include "lvgl.h"

namespace lx::ui {

namespace size {

static inline auto MARGIN() { return Overlay::getScaledRenderCoord(18); }

static inline auto getScreenHeight() { return Overlay::getCurLayerInfo().HEIGHT; }
static inline auto getScreenWidth() { return Overlay::getCurLayerInfo().WIDTH; }
static inline auto getScreenInnerHeight() { return Overlay::getCurLayerInfo().HEIGHT - MARGIN() * 2; }
static inline auto getScreenInnerWidth() { return Overlay::getCurLayerInfo().WIDTH - MARGIN() * 2; }

// transform to scaled render coord
static inline auto coord(int baseCoord) -> lv_coord_t { return Overlay::getScaledRenderCoord(baseCoord); }

}  // namespace size

namespace lv {

inline void initBgColorStyle(lv_style_t& style, lv_color_t color) {
    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, color);
}

inline auto createWithStyle(lv_obj_t* p_parent, std::function<lv_obj_t*(lv_obj_t*)> createCb, lv_style_t* p_style,
                            lv_obj_part_t objPart) {
    auto result = createCb(p_parent);
    lv_obj_add_style(result, objPart, p_style);
    return result;
}

inline auto createObjWithStyle(lv_obj_t* p_parent, std::function<lv_obj_t*(lv_obj_t*, lv_obj_t*)> lvCreateCb,
                               lv_style_t* p_style, lv_obj_part_t objPart) {
    auto result = lvCreateCb(p_parent, nullptr);
    lv_obj_add_style(result, objPart, p_style);
    return result;
}

inline void updateFitParent(lv_obj_t* p_lvObj) {
    auto* p_parent = lv_obj_get_parent(p_lvObj);
    lv_obj_set_size(p_lvObj, lv_obj_get_width_fit(p_parent), lv_obj_get_height_fit(p_parent));
}

}  // namespace lv

#if LV_USE_LABEL
namespace lv_label {

inline auto create(lv_obj_t* p_parent) {
    return lv::createObjWithStyle(p_parent, lv_label_create, ui::Controller::getFontStyleNormal(), LV_OBJ_PART_MAIN);
}

}  // namespace lv_label
#endif

#if LV_USE_WIN
namespace lv_win {

inline auto HEADER_HEIGHT() { return Overlay::getScaledRenderCoord(50); }

inline auto create(lv_obj_t* p_parent) {
    return lv::createObjWithStyle(p_parent, lv_win_create, ui::Controller::getFontStyleNormal(), LV_WIN_PART_HEADER);
}

inline auto create(lv_obj_t* p_parent, lv_style_t& headerStyle) {
    auto result = create(p_parent);
    lv_obj_add_style(result, LV_WIN_PART_HEADER, &headerStyle);
    lv_obj_add_style(result, LV_WIN_PART_BG, lx::ui::Controller::getScreenStyle());

    return result;
}

inline void updateHeader(lv_obj_t* p_lvWin) { lv_win_set_header_height(p_lvWin, HEADER_HEIGHT()); }

inline void updateFitParent(lv_obj_t* p_lvWin) {
    lv::updateFitParent(p_lvWin);
    updateHeader(p_lvWin);
}

inline auto getWinInnerHeight() { return size::getScreenInnerHeight() - HEADER_HEIGHT(); }

}  // namespace lv_win
#endif

#if LV_USE_BTNMATRIX
namespace lv_btnmatrix {

static constexpr auto LV_BTNMATRIX_END_STR = "";
static constexpr auto LV_BTNMATRIX_NEW_ROW_STR = "\n";

inline auto create(lv_obj_t* p_parent) {
    return lv::createObjWithStyle(p_parent, lv_btnmatrix_create, ui::Controller::getFontStyleSmall(),
                                  LV_BTNMATRIX_PART_BTN);
}

inline auto create(lv_obj_t* p_parent, lv_style_t& btnmatrixBgStyle, lv_style_t& btnmatrixBtnStyle) {
    auto result = create(p_parent);
    lv_obj_add_style(result, LV_BTNMATRIX_PART_BG, &btnmatrixBgStyle);
    lv_obj_add_style(result, LV_BTNMATRIX_PART_BTN, &btnmatrixBtnStyle);
    return result;
}

}  // namespace lv_btnmatrix
#endif

}  // namespace lx::ui
