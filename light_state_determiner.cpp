#include "light_state_determiner.h"
#include <QDebug>

light_state_determiner::light_state_determiner() {
    light_intensity_value = 0;
    last_time_occupancy_sensor_fired = QTime(0, 0, 0, 0); /* Start with sense having never fired */
    occupancy_timeout_delta = QTime(0, 5, 0, 0);    /* Use 5 minutes as timeout value default */
    user_light_setting = undefined;
}

void light_state_determiner::set_light_intensity(unsigned int light_in) {
    light_intensity_value = light_in;
}

unsigned int light_state_determiner::light_intensity(void) {
    return light_intensity_value;
}

void light_state_determiner::occupancy_sensor_fired(void) {
    last_time_occupancy_sensor_fired = QTime::currentTime();
}

QTime light_state_determiner::last_occupancy_detected(void) {
    return last_time_occupancy_sensor_fired;
}

void light_state_determiner::occupancy_detected(void) {
    occupancy_sensor_fired();
}

void light_state_determiner::set_occupancy_timeout_delta(QTime &timeout_value) {
    occupancy_timeout_delta = timeout_value;
}

light_states light_state_determiner::light_should_be(void) {
    light_states ret_value;

    // qDebug() << __func__ << " Entered.";
    if (user_light_setting == on) {
        ret_value = on;
    } else if (user_light_setting == off) {
        ret_value = off;
    } else { /* The user hasn't done anything particular yet, undefined state */
        QTime occupancy_timeout;
        unsigned int seconds_to_timeout;

        seconds_to_timeout = occupancy_timeout_delta.hour() * 3600 +
                occupancy_timeout_delta.minute() * 60 +
                occupancy_timeout_delta.second();
        occupancy_timeout = last_occupancy_detected().addSecs(seconds_to_timeout);
        if (occupancy_timeout < QTime::currentTime()) {
            ret_value = off;
        } else {
            /* Now check the light intensity to see if we should turn the light on. */
            //qDebug("%s:%d. light_intensity_value=%d turn_on_low_bound=%d turn_off_high_bound=%d",
            //       __func__, __LINE__, light_intensity_value, turn_on_low_bound, turn_off_high_bound);
            if (light_intensity_value < turn_on_low_bound) {
                // qDebug() << __func__ << " Intensity check says turn light on";
                ret_value = on;
            } else {
                if (light_intensity_value > turn_off_high_bound) {
                    // qDebug() << __func__ << " Intensity check says turn light off";
                    ret_value = off;
                }
            }
        }
    }
    //qDebug() << __func__ << " Exited ret_value =" << ret_value;
    return ret_value;
}

light_states light_state_determiner::user_set_light_state(void) {
    return user_light_setting;
}

void light_state_determiner::user_set_light_off(void) {
    user_light_setting = off;
}

void light_state_determiner::user_set_light_on(void) {
    user_light_setting = on;
}

void light_state_determiner::user_set_light_to_default(void) {
    user_light_setting = undefined;
}

void light_state_determiner::set_turn_on_low_bound(unsigned int new_low_bound) {
    turn_on_low_bound = new_low_bound;
}

void light_state_determiner::set_turn_off_high_bound(unsigned int new_high_bound) {
    turn_off_high_bound = new_high_bound;
}
