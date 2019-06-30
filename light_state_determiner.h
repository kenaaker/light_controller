#ifndef LIGHT_STATE_DETERMINER_H
#define LIGHT_STATE_DETERMINER_H

#include <QTime>

enum light_states { off, on, undefined };

class light_state_determiner {
  public:
    light_state_determiner();
    void set_light_intensity(unsigned int light_in);
    unsigned int light_intensity(void);
    void set_turn_on_low_bound(unsigned int new_low_bound);
    void set_turn_off_high_bound(unsigned int new_high_bound);
    void occupancy_sensor_fired(void);
    QTime last_occupancy_detected(void);
    void occupancy_detected(void);
    void set_occupancy_timeout_delta(QTime &timeout_value);
    light_states light_should_be(void);
    void user_set_light_off(void);
    void user_set_light_on(void);
    light_states user_set_light_state(void);

  private:
    unsigned int light_intensity_value;
    QTime last_time_occupancy_sensor_fired;
    QTime occupancy_timeout_delta;
    enum light_states user_light_setting;
    unsigned int turn_on_low_bound;
    unsigned int turn_off_high_bound;
};

#endif // LIGHT_STATE_DETERMINER_H
