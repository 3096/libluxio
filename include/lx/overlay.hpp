#pragma once

#include <switch.h>

#include <memory>

#include "key_map.hpp"
#include "layer_info.h"
#include "lvgl.h"
#include "lx_config.h"

namespace lx {

class Overlay {
   private:
    Overlay();
    Overlay(const Overlay&) = delete;
    ~Overlay();
    static inline auto& getInstance() {
        static Overlay s_instance;
        return s_instance;
    }

    struct LayerInfo {
        const uint16_t WIDTH;
        const uint16_t HEIGHT;
        const uint16_t POS_X;
        const uint16_t POS_Y;
    };

    static constexpr LayerInfo DOCKED_LAYER_INFO = {LAYER_INFO_DOCKED_WIDTH, LAYER_INFO_DOCKED_HEIGHT,
                                                    OVERLAY_POS_X_DOCKED, OVERLAY_POS_Y_DOCKED};
    static constexpr LayerInfo HANDHELD_LAYER_INFO = {LAYER_INFO_HANDHELD_WIDTH, LAYER_INFO_HANDHELD_HEIGHT,
                                                      OVERLAY_POS_X_HANDHELD, OVERLAY_POS_Y_HANDHELD};

    // libnx members
    PadState m_hidNPad;
    ViDisplay m_viDisplay;
    ViLayer m_viLayer;
    NWindow m_nWindow;
    Framebuffer m_frameBufferInfo;
    Event m_viDisplayVsyncEvent;

    // lvgl members
    lv_disp_t* mp_disp;
    lv_indev_t* mp_keyIn;
    lv_indev_t* mp_touchIn;
    lv_disp_drv_t m_dispDrv;
    lv_disp_buf_t m_dispBufferInfo;
    lv_color_t mp_renderBuf[LAYER_BUFFER_SIZE];
    lv_indev_drv_t m_touchDrv;
    lv_indev_drv_t m_keyDrv;

    // members
    void* mp_frameBuffers[2];
    bool m_doRender;
    bool m_isDocked;
    const LvKeyMap* mp_curLvKeyMap;

    inline void copyPrivFb_();
    inline LayerInfo getCurLayerInfo_() { return m_isDocked ? DOCKED_LAYER_INFO : HANDHELD_LAYER_INFO; }
    void setLayerSizeAndPosition_();
    bool consoleIsDocked_();

    static void flushBuffer_(lv_disp_drv_t* p_disp, const lv_area_t* p_area, lv_color_t* p_lvBuffer);
    static bool touchRead_(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);
    static bool keysRead_(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

   public:
    static inline void instantiate() { getInstance(); };

    // static inline void pauseRendering() { getInstance().m_doRender = false; }  // unused
    // static inline void resumeRendering() { getInstance().m_doRender = true; }  // unused
    // static inline void toggleRendering() { getInstance().m_doRender = !m_doRender; }  // unused
    // static inline bool isRendering() { getInstance().return m_doRender; }  // unused
    static void flushEmptyFb();
    static void waitForVSync();

    static bool updateAndGetIsDockedStatusChanged();
    static inline bool getIsDockedStatus() { return getInstance().m_isDocked; }
    static inline auto scanPadState() {
        auto& padState = getInstance().m_hidNPad;
        padUpdate(&padState);
        return padState;
    }

    static inline auto getCurLayerInfo() { return getInstance().getCurLayerInfo_(); }
    static inline auto getScaledRenderCoord(int baseCord) {
        return getInstance().m_isDocked ? baseCord * OVERLAY_UPSCALE_DOCKED
                                        : baseCord * OVERLAY_UPSCALE_HANDHELD * HANDHELD_DOCK_PIXEL_RATIO;
    }

    static inline void setLvKeyMap(const LvKeyMap& lvKepMap) { getInstance().mp_curLvKeyMap = &lvKepMap; }

    static inline auto getKeyInDev() { return getInstance().mp_keyIn; }
    // static inline auto getTouchInDev() { return getInstance().mp_touchIn; }  // unused
};

}  // namespace lx
