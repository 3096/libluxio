#pragma once

#include <switch.h>

#include <array>
#include <unordered_map>

#include "lvgl.h"
#include "lx_config.h"

namespace lx {

using HidControllerKeyData = decltype(PadState::buttons_cur);

using LvKeyMap = std::unordered_map<HidControllerKeyData, lv_key_t>;
static const auto DEFAULT_LV_KEY_MAP_INSTANCE = LvKeyMap{DEFAULT_LV_KEY_MAP};

struct ActionKeyMap {
    HidControllerKeyData goBack;
    HidControllerKeyData toggleOverlay;
};
static const auto DEFAULT_ACTION_KEY_MAP_INSTANCE = ActionKeyMap DEFAULT_ACTION_KEY_MAP;

}  // namespace lx
