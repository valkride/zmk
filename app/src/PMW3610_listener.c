// PMW3610_listener.c
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/keys.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct pmw3610_listener_config {
    uint32_t up;
    uint32_t down;
    uint32_t left;
    uint32_t right;
};


static int pmw3610_listener_binding_pressed(struct zmk_behavior_binding *binding,
                                           struct zmk_behavior_binding_event event) {
    if (!binding || !binding->param1) {
        LOG_ERR("Invalid binding or parameter");
        return ZMK_BEHAVIOR_OPAQUE;
    }
    const struct pmw3610_listener_config *config = (const struct pmw3610_listener_config *)binding->param1;
    if (!config->up) {
        LOG_ERR("Invalid or uninitialized 'up' key in config");
        return ZMK_BEHAVIOR_OPAQUE;
    }
    zmk_hid_press(config->up);
    return ZMK_BEHAVIOR_OPAQUE;
}


static int pmw3610_listener_binding_released(struct zmk_behavior_binding *binding,
                                            struct zmk_behavior_binding_event event) {
    if (!binding || !binding->param1) {
        LOG_ERR("Invalid binding or parameter");
        return ZMK_BEHAVIOR_OPAQUE;
    }
    const struct pmw3610_listener_config *config = (const struct pmw3610_listener_config *)binding->param1;
    if (!config->up) {
        LOG_ERR("Invalid or uninitialized 'up' key in config");
        return ZMK_BEHAVIOR_OPAQUE;
    }
    zmk_hid_release(config->up);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api pmw3610_listener_driver_api = {
    .binding_pressed = pmw3610_listener_binding_pressed,
    .binding_released = pmw3610_listener_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, &pmw3610_listener_driver_api);