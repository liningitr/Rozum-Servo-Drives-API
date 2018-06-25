/**
 * @brief Tutorial example of reading device parameter cache
 * 
 * @file read_any_param_cache.c
 * @author Rozum
 * @date 2018-06-25
 */

#include "api.h"
#include "tutorial.h"

/**
 * @brief Tutorial example of reading device parameter cache
 * 
 * @ingroup tutor_c_param_cache
 */
int main(int argc, char *argv[])
{
    /** @code{.c} 
    */
    rr_can_interface_t *iface = rr_init_interface(TUTORIAL_DEVICE);
    rr_servo_t *servo = rr_init_servo(iface, TUTORIAL_SERVO_0_ID);

    API_DEBUG("========== Tutorial of %s ==========\n", "programming and reading the device parameter cache");

    rr_param_cache_setup_entry(servo, APP_PARAM_POSITION_ROTOR, true);
    rr_param_cache_setup_entry(servo, APP_PARAM_VELOCITY_ROTOR, true);
    rr_param_cache_setup_entry(servo, APP_PARAM_VOLTAGE_INPUT, true);
    rr_param_cache_setup_entry(servo, APP_PARAM_CURRENT_INPUT, true);

    rr_param_cache_update(servo);

    API_DEBUG("\t%s value: %.3f\n", STRFY(APP_PARAM_POSITION_ROTOR), servo->pcache[APP_PARAM_POSITION_ROTOR].value);
    API_DEBUG("\t%s value: %.3f\n", STRFY(APP_PARAM_VELOCITY_ROTOR), servo->pcache[APP_PARAM_VELOCITY_ROTOR].value);
    API_DEBUG("\t%s value: %.3f\n", STRFY(APP_PARAM_VOLTAGE_INPUT), servo->pcache[APP_PARAM_VOLTAGE_INPUT].value);
    API_DEBUG("\t%s value: %.3f\n", STRFY(APP_PARAM_CURRENT_INPUT), servo->pcache[APP_PARAM_CURRENT_INPUT].value);
    /** @endcode */
}