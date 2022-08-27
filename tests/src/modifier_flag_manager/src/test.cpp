#include "modifier_flag_manager.hpp"
#include <boost/ut.hpp>

namespace {
krbn::modifier_flag_manager::active_modifier_flag left_shift_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                               krbn::modifier_flag::left_shift,
                                                               krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_left_shift_1(krbn::modifier_flag_manager::active_modifier_flag::type::decrease,
                                                                        krbn::modifier_flag::left_shift,
                                                                        krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag lock_left_shift(krbn::modifier_flag_manager::active_modifier_flag::type::increase_lock,
                                                                  krbn::modifier_flag::left_shift,
                                                                  krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_lock_left_shift(krbn::modifier_flag_manager::active_modifier_flag::type::decrease_lock,
                                                                           krbn::modifier_flag::left_shift,
                                                                           krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag lock_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::increase_lock,
                                                                 krbn::modifier_flag::caps_lock,
                                                                 krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_lock_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::decrease_lock,
                                                                          krbn::modifier_flag::caps_lock,
                                                                          krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag led_lock_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::increase_led_lock,
                                                                     krbn::modifier_flag::caps_lock,
                                                                     krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_led_lock_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::decrease_led_lock,
                                                                              krbn::modifier_flag::caps_lock,
                                                                              krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag sticky_left_shift(krbn::modifier_flag_manager::active_modifier_flag::type::increase_sticky,
                                                                    krbn::modifier_flag::left_shift,
                                                                    krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_sticky_left_shift(krbn::modifier_flag_manager::active_modifier_flag::type::decrease_sticky,
                                                                             krbn::modifier_flag::left_shift,
                                                                             krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag sticky_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::increase_sticky,
                                                                   krbn::modifier_flag::caps_lock,
                                                                   krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag decrease_sticky_caps_lock(krbn::modifier_flag_manager::active_modifier_flag::type::decrease_sticky,
                                                                            krbn::modifier_flag::caps_lock,
                                                                            krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag left_shift_2(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                               krbn::modifier_flag::left_shift,
                                                               krbn::device_id(2));

krbn::modifier_flag_manager::active_modifier_flag decrease_left_shift_2(krbn::modifier_flag_manager::active_modifier_flag::type::decrease,
                                                                        krbn::modifier_flag::left_shift,
                                                                        krbn::device_id(2));

krbn::modifier_flag_manager::active_modifier_flag right_shift_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                krbn::modifier_flag::right_shift,
                                                                krbn::device_id(1));

krbn::modifier_flag_manager::active_modifier_flag right_command_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                  krbn::modifier_flag::right_command,
                                                                  krbn::device_id(1));
} // namespace

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "modifier_flag_manager"_test = [] {
    // ----------------------------------------
    // Push back
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::right_shift) == false);
    }

    // ----------------------------------------
    // Erase
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 0);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    // ----------------------------------------
    // Push back twice
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 2);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 0);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    // ----------------------------------------
    // Erase first
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 0);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      expect(modifier_flag_manager.active_modifier_flags_size() == 1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
    }

    // ----------------------------------------
    // Multiple device id
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_2);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_2);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    // ----------------------------------------
    // Erase all
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_2);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.erase_all_active_modifier_flags(krbn::device_id(1));
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.erase_all_active_modifier_flags(krbn::device_id(2));
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    // ----------------------------------------
    // type::increase_lock
    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(lock_left_shift);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.erase_all_active_modifier_flags_except_lock_and_sticky(krbn::device_id(1));
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_lock_left_shift);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    //
    // type::decrease_lock
    //

    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(decrease_lock_left_shift);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      // lock_left_shift(-1), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      // lock_left_shift(-1), left_shift(+2)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      // lock_left_shift(-1), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_lock_left_shift);
      // lock_left_shift(-1), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(lock_left_shift);
      // lock_left_shift(0), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      // lock_left_shift(0), left_shift(0)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    //
    // lock twice
    //

    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(lock_left_shift);
      modifier_flag_manager.push_back_active_modifier_flag(lock_left_shift);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_lock_left_shift);
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
    }

    //
    // led_lock
    //

    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(led_lock_caps_lock);
      // led_lock_caps_lock(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == true);
      expect(modifier_flag_manager.led_lock_size(krbn::modifier_flag::caps_lock) == 1);

      modifier_flag_manager.push_back_active_modifier_flag(led_lock_caps_lock);
      // led_lock_caps_lock(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == true);
      expect(modifier_flag_manager.led_lock_size(krbn::modifier_flag::caps_lock) == 1);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_led_lock_caps_lock);
      // led_lock_caps_lock(0)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == false);
      expect(modifier_flag_manager.led_lock_size(krbn::modifier_flag::caps_lock) == 0);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_led_lock_caps_lock);
      // led_lock_caps_lock(0)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == false);
      expect(modifier_flag_manager.led_lock_size(krbn::modifier_flag::caps_lock) == 0);

      modifier_flag_manager.push_back_active_modifier_flag(led_lock_caps_lock);
      // led_lock_caps_lock(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == true);
      expect(modifier_flag_manager.led_lock_size(krbn::modifier_flag::caps_lock) == 1);
    }

    //
    // led_lock and others
    //

    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(led_lock_caps_lock);
      // led_lock_caps_lock(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_sticky_caps_lock);
      // led_lock_caps_lock(1), sticky_caps_lock(-1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == false);

      modifier_flag_manager.push_back_active_modifier_flag(lock_caps_lock);
      // led_lock_caps_lock(1), sticky_caps_lock(-1), lock_caps_lock(+1)
      // Note: led_lock is ignored because other flags exist.
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::caps_lock) == false);
    }

    //
    // sticky
    //

    {
      krbn::modifier_flag_manager modifier_flag_manager;

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      // left_shift(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_left_shift_1);
      // left_shift(0)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(sticky_left_shift);
      // sticky_left_shift(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(sticky_left_shift);
      // sticky_left_shift(2)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_sticky_left_shift);
      // sticky_left_shift(1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == true);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_sticky_left_shift);
      // sticky_left_shift(0)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(decrease_sticky_left_shift);
      // sticky_left_shift(-1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      // sticky_left_shift(-1), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == false);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);

      modifier_flag_manager.erase_all_sticky_modifier_flags();
      // sticky_left_shift(0), left_shift(+1)
      expect(modifier_flag_manager.is_pressed(krbn::modifier_flag::left_shift) == true);
      expect(modifier_flag_manager.is_sticky_active(krbn::modifier_flag::left_shift) == false);
    }
  };

  "modifier_flag_manager::make_hid_report_modifiers"_test = [] {
    {
      namespace hid_report = pqrs::karabiner::driverkit::virtual_hid_device_driver::hid_report;

      krbn::modifier_flag_manager modifier_flag_manager;
      hid_report::modifiers expected;

      expect(modifier_flag_manager.make_hid_report_modifiers() == expected);

      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_command_1);
      expected.insert(hid_report::modifier::left_shift);
      expected.insert(hid_report::modifier::right_shift);
      expected.insert(hid_report::modifier::right_command);
      expect(modifier_flag_manager.make_hid_report_modifiers() == expected);
    }
  };

  return 0;
}
