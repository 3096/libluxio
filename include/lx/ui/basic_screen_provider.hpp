#pragma once

#include <switch.h>

#include <functional>
#include <list>

#include "../overlay.hpp"
#include "../ui/controller.hpp"
#include "lvgl.h"

namespace lx::ui {

class BasicScreenProvider {
   private:
    struct LvObjPositionUpdater {
        lv_obj_t* p_lvObj;
        std::function<void(lv_obj_t*)> updateCb;
    };
    std::list<LvObjPositionUpdater> m_lvObjUpdaterList;

    IScreen* mp_prevScreen;

    lv_obj_t* mp_screenObj;
    lv_group_t* mp_inputGroup;

   public:
    BasicScreenProvider();
    ~BasicScreenProvider();

    // transform to scaled render coord
    static inline auto coord(int baseCoord) -> lv_coord_t { return Overlay::getScaledRenderCoord(baseCoord); }

    inline void addLvObjPositionUpdater(lv_obj_t* p_lvObj, std::function<void(lv_obj_t*)> updateCb) {
        m_lvObjUpdaterList.push_back({p_lvObj, updateCb});
    }

    inline void removeLvObjPositionUpdaters(lv_obj_t* p_lvObjToRemove) {
        m_lvObjUpdaterList.remove_if(
            [p_lvObjToRemove](LvObjPositionUpdater updater) { return updater.p_lvObj == p_lvObjToRemove; });
    }

    inline void returnToPreviousScreen() {
        if (mp_prevScreen) {
            ui::Controller::show(*mp_prevScreen);
        } else {
            ui::Controller::stop();
        }
    }

    inline void processReturn() {
        if (ui::Controller::getKeysDown() & KEY_L) {  // TODO: make configurable
            returnToPreviousScreen();
        }
    }

    // i_Screen providers
    inline void onMount(IScreen* prevScreen) { mp_prevScreen = prevScreen; }
    void renderScreen();
    inline lv_obj_t* getLvScreenObj() { return mp_screenObj; }
    inline lv_group_t* getLvInputGroup() { return mp_inputGroup; }
};

}  // namespace lx::ui
