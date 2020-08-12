#pragma once

#include "lvgl.h"
#include "lx/key_map.hpp"

namespace lx::ui {

class IScreen {
   public:
    virtual void onMount(IScreen* prevScreen) = 0;
    virtual void onUnmount() {}
    virtual void onToggleHide() {}
    virtual void onToggleShow() {}
    virtual void renderScreen() = 0;
    virtual void procFrame() = 0;

    virtual lv_obj_t* getLvScreenObj() = 0;
    virtual lv_group_t* getLvInputGroup() = 0;

    virtual const LvKeyMap& getLvKeyMap() { return DEFAULT_LV_KEY_MAP_INSTANCE; }
    virtual const ActionKeyMap& getActionKeyMap() { return DEFAULT_ACTION_KEY_MAP_INSTANCE; }
};

}  // namespace lx::ui
