#pragma once

#include <cstdlib>

#include "../debug.hpp"
#include "i_screen.hpp"

namespace lx::ui {

class Controller {
    LOGCONSTRUCTM;

   private:
    Controller();
    Controller(const Controller&) = delete;
    ~Controller();
    static inline auto& getInstance() {
        static Controller s_instance;
        return s_instance;
    }

    struct LvFonts {
        lv_font_t* normal;
        lv_font_t* small;
    };

    static constexpr auto DOCKED_FONT = LvFonts OVERLAY_FONT_DOCKED;
    static constexpr auto HANDHELD_FONT = LvFonts OVERLAY_FONT_HANDHELD;

    static constexpr auto DEFAULT_SCREEN_COLOR = lv_color_t{0, 0, 0, 0x7F};

    // controlled styles
    lv_style_t m_fontStyleNormal;
    lv_style_t m_fontStyleSmall;
    lv_style_t m_screenStyle;

    // state members
    IScreen* mp_curScreen;
    IScreen* mp_nextScreen;

    bool m_screenIsOn;
    bool m_shouldRerender;
    bool m_shouldExit;

    uint64_t m_keysDown;
    uint64_t m_keysHeld;

    // helpers
    inline void mountScreen_(IScreen* screenToMount, IScreen* prevScreen);
    inline void updateFontStyles_();
    inline auto keyComboIsJustPressedImpl_(HidControllerKeyData keyCombo) {
        return (m_keysHeld & keyCombo) == keyCombo and m_keysHeld & m_keysDown;
    }

    void threadMain_();

   public:
    static void show(IScreen& screenToShow);
    inline static void stop() { getInstance().m_shouldExit = true; }

    // inline static auto getKeysDown() { return getInstance().m_keysDown; }  // unused
    // inline static auto getKeysHeld() { return getInstance().m_keysHeld; }  // unused
    inline static auto keyComboIsJustPressed(HidControllerKeyData keyCombo) {
        return getInstance().keyComboIsJustPressedImpl_(keyCombo);
    }

    inline static auto getFontStyleNormal() { return &(getInstance().m_fontStyleNormal); }
    inline static auto getFontStyleSmall() { return &(getInstance().m_fontStyleSmall); }
    inline static auto getScreenStyle() { return &(getInstance().m_screenStyle); }
};

}  // namespace lx::ui
