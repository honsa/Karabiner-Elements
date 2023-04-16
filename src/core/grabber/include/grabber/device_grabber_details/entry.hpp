#pragma once

#include "core_configuration/core_configuration.hpp"
#include "device_properties.hpp"
#include "device_utility.hpp"
#include "event_queue.hpp"
#include "hid_keyboard_caps_lock_led_state_manager.hpp"
#include "iokit_utility.hpp"
#include "pressed_keys_manager.hpp"
#include "run_loop_thread_utility.hpp"
#include "types.hpp"
#include <pqrs/osx/iokit_hid_queue_value_monitor.hpp>

namespace krbn {
namespace grabber {
namespace device_grabber_details {
class entry final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  entry(device_id device_id,
        IOHIDDeviceRef device,
        std::weak_ptr<const core_configuration::core_configuration> core_configuration) : dispatcher_client(),
                                                                                          device_id_(device_id),
                                                                                          core_configuration_(core_configuration),
                                                                                          hid_queue_value_monitor_async_start_called_(false),
                                                                                          first_value_arrived_(false),
                                                                                          grabbed_(false),
                                                                                          disabled_(false),
                                                                                          event_origin_(event_origin::none),
                                                                                          grabbed_time_stamp_(0),
                                                                                          ungrabbed_time_stamp_(0) {
    device_properties_ = std::make_shared<device_properties>(device_id,
                                                             device);

    pressed_keys_manager_ = std::make_shared<pressed_keys_manager>();
    hid_queue_value_monitor_ = std::make_shared<pqrs::osx::iokit_hid_queue_value_monitor>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                                                          pqrs::cf::run_loop_thread::extra::get_shared_run_loop_thread(),
                                                                                          device);
    caps_lock_led_state_manager_ = std::make_shared<krbn::hid_keyboard_caps_lock_led_state_manager>(device);
    device_name_ = iokit_utility::make_device_name_for_log(device_id,
                                                           device);
    device_short_name_ = iokit_utility::make_device_name(device);

    update_event_origin();
  }

  ~entry(void) {
    detach_from_dispatcher([this] {
      hid_queue_value_monitor_ = nullptr;
      caps_lock_led_state_manager_ = nullptr;
    });
  }

  device_id get_device_id(void) const {
    return device_id_;
  }

  void set_core_configuration(std::weak_ptr<const core_configuration::core_configuration> core_configuration) {
    core_configuration_ = core_configuration;

    //
    // Update event_origin_
    //

    update_event_origin();

    //
    // Update caps_lock_led_state_manager state
    //

    control_caps_lock_led_state_manager();
  }

  std::shared_ptr<device_properties> get_device_properties(void) const {
    return device_properties_;
  }

  std::shared_ptr<pressed_keys_manager> get_pressed_keys_manager(void) const {
    return pressed_keys_manager_;
  }

  std::shared_ptr<pqrs::osx::iokit_hid_queue_value_monitor> get_hid_queue_value_monitor(void) const {
    return hid_queue_value_monitor_;
  }

  bool get_first_value_arrived(void) const {
    return first_value_arrived_;
  }

  void set_first_value_arrived(bool value) {
    first_value_arrived_ = value;
  }

  std::shared_ptr<hid_keyboard_caps_lock_led_state_manager> get_caps_lock_led_state_manager(void) const {
    return caps_lock_led_state_manager_;
  }

  const std::string& get_device_name(void) const {
    return device_name_;
  }

  const std::string& get_device_short_name(void) const {
    return device_short_name_;
  }

  bool get_grabbed(void) const {
    return grabbed_;
  }

  void set_grabbed(bool value) {
    grabbed_ = value;

    if (grabbed_) {
      grabbed_time_stamp_ = pqrs::osx::chrono::mach_absolute_time_point();
    } else {
      ungrabbed_time_stamp_ = pqrs::osx::chrono::mach_absolute_time_point();
    }

    control_caps_lock_led_state_manager();
  }

  bool get_disabled(void) const {
    return disabled_;
  }

  void set_disabled(bool value) {
    disabled_ = value;

    update_event_origin();
  }

  event_origin get_event_origin(void) const {
    return event_origin_;
  }

  bool is_disable_built_in_keyboard_if_exists(void) const {
    if (device_properties_) {
      if (device_properties_->get_is_built_in_keyboard() ||
          device_properties_->get_is_built_in_pointing_device() ||
          device_properties_->get_is_built_in_touch_bar()) {
        return false;
      }

      if (auto c = core_configuration_.lock()) {
        if (auto device_identifiers = device_properties_->get_device_identifiers()) {
          return c->get_selected_profile().get_device_disable_built_in_keyboard_if_exists(
              *device_identifiers);
        }
      }
    }
    return false;
  }

  bool determine_is_built_in_keyboard(void) const {
    if (auto c = core_configuration_.lock()) {
      if (device_properties_) {
        return device_utility::determine_is_built_in_keyboard(*c, *device_properties_);
      }
    }

    return false;
  }

  void async_start_queue_value_monitor(void) {
    auto options = kIOHIDOptionsTypeSeizeDevice;
    if (event_origin_ == event_origin::observed_device) {
      options = kIOHIDOptionsTypeNone;
    }

    //
    // Stop if already started with different options.
    //

    if (options != hid_queue_value_monitor_async_start_option_) {
      async_stop_queue_value_monitor();
    }

    //
    // Start
    //

    if (hid_queue_value_monitor_) {
      if (!hid_queue_value_monitor_async_start_called_) {
        first_value_arrived_ = false;
        hid_queue_value_monitor_async_start_called_ = true;
      }

      hid_queue_value_monitor_async_start_option_ = options;

      hid_queue_value_monitor_->async_start(options,
                                            std::chrono::milliseconds(1000));
    }
  }

  void async_stop_queue_value_monitor(void) {
    if (hid_queue_value_monitor_) {
      hid_queue_value_monitor_async_start_called_ = false;

      hid_queue_value_monitor_->async_stop();
    }
  }

  bool is_grabbed(absolute_time_point time_stamp) {
    if (grabbed_) {
      if (grabbed_time_stamp_ <= time_stamp) {
        return true;
      }
    } else {
      //
      // (grabbed_time_stamp_ <= ungrabbed_time_stamp_) when (grabbed_ == false)
      //

      if (grabbed_time_stamp_ <= time_stamp &&
          time_stamp <= ungrabbed_time_stamp_) {
        return true;
      }
    }

    return false;
  }

private:
  bool is_ignored_device(void) const {
    if (device_properties_) {
      if (auto c = core_configuration_.lock()) {
        if (auto device_identifiers = device_properties_->get_device_identifiers()) {
          return c->get_selected_profile().get_device_ignore(
              *device_identifiers);
        }
      }
    }

    return false;
  }

  void update_event_origin(void) {
    auto old_event_origin = event_origin_;

    if (disabled_) {
      event_origin_ = event_origin::grabbed_device;
    } else {
      if (is_ignored_device()) {
        event_origin_ = event_origin::observed_device;
      } else {
        event_origin_ = event_origin::grabbed_device;
      }
    }

    if (old_event_origin != event_origin_) {
      logger::get_logger()->info("device_grabber_details::entry event_origin_ is updated. {0}: {1} -> {2}",
                                 device_name_,
                                 nlohmann::json(old_event_origin),
                                 nlohmann::json(event_origin_));
    }
  }

  void control_caps_lock_led_state_manager(void) {
    if (caps_lock_led_state_manager_) {
      if (device_properties_) {
        if (auto c = core_configuration_.lock()) {
          if (auto device_identifiers = device_properties_->get_device_identifiers()) {
            if (c->get_selected_profile().get_device_manipulate_caps_lock_led(*device_identifiers)) {
              if (grabbed_ && event_origin_ == event_origin::grabbed_device) {
                caps_lock_led_state_manager_->async_start();
                return;
              }
            }
          }
        }
      }

      caps_lock_led_state_manager_->async_stop();
    }
  }

  device_id device_id_;
  std::weak_ptr<const core_configuration::core_configuration> core_configuration_;
  std::shared_ptr<device_properties> device_properties_;
  std::shared_ptr<pressed_keys_manager> pressed_keys_manager_;

  std::shared_ptr<pqrs::osx::iokit_hid_queue_value_monitor> hid_queue_value_monitor_;
  bool hid_queue_value_monitor_async_start_called_;
  std::optional<IOOptionBits> hid_queue_value_monitor_async_start_option_;
  bool first_value_arrived_;

  std::shared_ptr<hid_keyboard_caps_lock_led_state_manager> caps_lock_led_state_manager_;
  std::string device_name_;
  std::string device_short_name_;

  bool grabbed_;
  bool disabled_;
  event_origin event_origin_;

  absolute_time_point grabbed_time_stamp_;
  absolute_time_point ungrabbed_time_stamp_;
};
} // namespace device_grabber_details
} // namespace grabber
} // namespace krbn
