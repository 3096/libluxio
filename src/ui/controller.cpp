#include "lx/ui/controller.hpp"

#include <cassert>
#include <stdexcept>

#include "lx/overlay.hpp"
#include "lx/ui/lv_helper.hpp"

namespace lx::ui {

Controller::Controller()
    : LOGCONSTRUCTSL mp_curScreen(nullptr),
      mp_nextScreen(nullptr),
      m_screenIsOn(true),
      m_shouldRerender(true),
      m_shouldExit(false) {
    lv_style_init(&m_fontStyleNormal);
    lv_style_init(&m_fontStyleSmall);
    updateFontStyles_();

    lv::initBgColorStyle(m_screenStyle, DEFAULT_SCREEN_COLOR);

    LOGEL("done");
}

Controller::~Controller() {}

void Controller::mountScreen_(IScreen* screenToMount, IScreen* prevScreen) {
    screenToMount->onMount(prevScreen);
    Overlay::setLvKeyMap(screenToMount->getLvKeyMap());
    lv_indev_set_group(Overlay::getKeyInDev(), screenToMount->getLvInputGroup());
    lv_scr_load(screenToMount->getLvScreenObj());
    m_shouldRerender = true;
}

void Controller::updateFontStyles_() {
    // TODO: write a custom shared font renderer
    if (Overlay::getIsDockedStatus()) {
        lv_style_set_text_font(&m_fontStyleNormal, LV_STATE_DEFAULT, DOCKED_FONT.normal);
        lv_style_set_text_font(&m_fontStyleSmall, LV_STATE_DEFAULT, DOCKED_FONT.small);
    } else {
        lv_style_set_text_font(&m_fontStyleNormal, LV_STATE_DEFAULT, HANDHELD_FONT.normal);
        lv_style_set_text_font(&m_fontStyleSmall, LV_STATE_DEFAULT, HANDHELD_FONT.small);
    }
}

void Controller::threadMain_() {
    // mount inital screen
    // DEBUG_ASSERT(mp_curScreen);  // TODO: implement an assert
    mountScreen_(mp_curScreen, nullptr);

    // main loop
    while (true) {
        // check if exit is requested
        if (m_shouldExit) {
            mp_curScreen->onUnmount();
            break;
        }

        // update hid
        auto padState = Overlay::scanPadState();
        m_keysDown = padGetButtonsDown(&padState);
        m_keysHeld = padGetButtons(&padState);

        // update screen toggle
        if (keyComboIsJustPressedImpl_(mp_curScreen->getActionKeyMap().toggleOverlay)) {
            m_screenIsOn = !m_screenIsOn;
            if (m_screenIsOn) {
                m_shouldRerender = true;
                mp_curScreen->onToggleShow();
            } else {
                Overlay::flushEmptyFb();  // Turn off screen
                mp_curScreen->onToggleHide();
            }
        }

        if (m_screenIsOn) {
            if (mp_nextScreen) {  // check if next screen is requested
                mp_curScreen->onUnmount();
                mountScreen_(mp_nextScreen, mp_curScreen);

                mp_curScreen = mp_nextScreen;
                mp_nextScreen = nullptr;
            }

            if (Overlay::updateAndGetIsDockedStatusChanged()) {
                updateFontStyles_();
                m_shouldRerender = true;
            }

            if (m_shouldRerender) {
                lv_obj_refresh_style(mp_curScreen->getLvScreenObj(), LV_STYLE_PROP_ALL);
                lv_obj_invalidate(mp_curScreen->getLvScreenObj());
                mp_curScreen->renderScreen();
                m_shouldRerender = false;
            }

            // curScreen process frame
            mp_curScreen->procFrame();
            lv_task_handler();
        }

        Overlay::waitForVSync();
    }
}

void Controller::show(IScreen& screenToShow) {
    auto& s_instance = getInstance();
    if (s_instance.mp_curScreen == nullptr) {
        s_instance.mp_curScreen = &screenToShow;
        s_instance.threadMain_();  // TODO: create ui thread instead of just calling
    } else {
        s_instance.mp_nextScreen = &screenToShow;
    }
}

}  // namespace lx::ui
