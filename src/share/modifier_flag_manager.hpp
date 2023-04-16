#pragma once

#include "types.hpp"
#include <array>
#include <thread>
#include <vector>

namespace krbn {
class modifier_flag_manager final {
public:
  class active_modifier_flag final {
  public:
    enum class type {
      increase,
      decrease,
      increase_lock,
      decrease_lock,
      increase_led_lock, // Synchronized with LED state such as caps lock.
      decrease_led_lock, // Synchronized with LED state such as caps lock.
      increase_sticky,
      decrease_sticky,
    };

    active_modifier_flag(type type,
                         modifier_flag modifier_flag,
                         device_id device_id) : type_(type),
                                                modifier_flag_(modifier_flag),
                                                device_id_(device_id) {
      switch (type) {
        case type::increase:
        case type::decrease:
        case type::increase_lock:
        case type::decrease_lock:
        case type::increase_sticky:
        case type::decrease_sticky:
          break;

        case type::increase_led_lock:
        case type::decrease_led_lock:
          // The led_lock is shared by all devices because it is not sent from hardware.
          //
          // Note:
          // The caps lock state refers the virtual keyboard LED state and
          // it is not related with the caps lock key_down and key_up events.
          // (The behavior is described in docs/DEVELOPMENT.md.)

          device_id_ = krbn::device_id(0);
          break;
      }
    }

    type get_type(void) const {
      return type_;
    }

    modifier_flag get_modifier_flag(void) const {
      return modifier_flag_;
    }

    device_id get_device_id(void) const {
      return device_id_;
    }

    type get_inverse_type(void) const {
      switch (type_) {
        case type::increase:
          return type::decrease;
        case type::decrease:
          return type::increase;

        case type::increase_lock:
          return type::decrease_lock;
        case type::decrease_lock:
          return type::increase_lock;

        case type::increase_led_lock:
          return type::decrease_led_lock;
        case type::decrease_led_lock:
          return type::increase_led_lock;

        case type::increase_sticky:
          return type::decrease_sticky;
        case type::decrease_sticky:
          return type::increase_sticky;
      }
    }

    int get_count(void) const {
      switch (type_) {
        case type::increase:
        case type::increase_lock:
        case type::increase_led_lock:
        case type::increase_sticky:
          return 1;
        case type::decrease:
        case type::decrease_lock:
        case type::decrease_led_lock:
        case type::decrease_sticky:
          return -1;
      }
    }

    bool any_lock(void) const {
      switch (type_) {
        case type::increase_lock:
        case type::decrease_lock:
        case type::increase_led_lock:
        case type::decrease_led_lock:
          return true;

        case type::increase:
        case type::decrease:
        case type::increase_sticky:
        case type::decrease_sticky:
          return false;
      }
    }

    bool led_lock(void) const {
      switch (type_) {
        case type::increase_led_lock:
        case type::decrease_led_lock:
          return true;

        case type::increase:
        case type::decrease:
        case type::increase_lock:
        case type::decrease_lock:
        case type::increase_sticky:
        case type::decrease_sticky:
          return false;
      }
    }

    bool sticky(void) const {
      switch (type_) {
        case type::increase_sticky:
        case type::decrease_sticky:
          return true;

        case type::increase:
        case type::decrease:
        case type::increase_lock:
        case type::decrease_lock:
        case type::increase_led_lock:
        case type::decrease_led_lock:
          return false;
      }
    }

    bool is_paired(const active_modifier_flag& other) const {
      return get_type() == other.get_inverse_type() &&
             get_modifier_flag() == other.get_modifier_flag() &&
             get_device_id() == other.get_device_id();
    }

    constexpr auto operator<=>(const active_modifier_flag&) const = default;

  private:
    type type_;
    modifier_flag modifier_flag_;
    device_id device_id_;
  };

  modifier_flag_manager(const modifier_flag_manager&) = delete;

  modifier_flag_manager(void) {
  }

  void push_back_active_modifier_flag(const active_modifier_flag& flag) {
    switch (flag.get_type()) {
      case active_modifier_flag::type::increase:
      case active_modifier_flag::type::decrease:
      case active_modifier_flag::type::increase_sticky:
      case active_modifier_flag::type::decrease_sticky:
        active_modifier_flags_.push_back(flag);
        erase_pairs();
        break;

      case active_modifier_flag::type::increase_lock:
      case active_modifier_flag::type::decrease_lock:
      case active_modifier_flag::type::increase_led_lock:
        // Remove same type entries.
        active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                    std::end(active_modifier_flags_),
                                                    [&](auto& f) {
                                                      return f == flag;
                                                    }),
                                     std::end(active_modifier_flags_));

        active_modifier_flags_.push_back(flag);
        erase_pairs();
        break;

      case active_modifier_flag::type::decrease_led_lock:
        // Remove all type::increase_led_lock.
        active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                    std::end(active_modifier_flags_),
                                                    [&](auto& f) {
                                                      return f.is_paired(flag);
                                                    }),
                                     std::end(active_modifier_flags_));
        break;
    }
  }

  void erase_all_active_modifier_flags(device_id device_id) {
    active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                std::end(active_modifier_flags_),
                                                [&](const active_modifier_flag& f) {
                                                  return f.get_device_id() == device_id;
                                                }),
                                 std::end(active_modifier_flags_));
  }

  void erase_all_active_modifier_flags_except_lock_and_sticky(device_id device_id) {
    active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                std::end(active_modifier_flags_),
                                                [&](const active_modifier_flag& f) {
                                                  return f.get_device_id() == device_id && !f.any_lock() && !f.sticky();
                                                }),
                                 std::end(active_modifier_flags_));
  }

  void erase_caps_lock_sticky_modifier_flags(void) {
    active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                std::end(active_modifier_flags_),
                                                [&](const active_modifier_flag& f) {
                                                  return f.get_modifier_flag() == modifier_flag::caps_lock && f.sticky();
                                                }),
                                 std::end(active_modifier_flags_));
  }

  void erase_all_sticky_modifier_flags(void) {
    active_modifier_flags_.erase(std::remove_if(std::begin(active_modifier_flags_),
                                                std::end(active_modifier_flags_),
                                                [&](const active_modifier_flag& f) {
                                                  return f.sticky();
                                                }),
                                 std::end(active_modifier_flags_));
  }

  void reset(void) {
    active_modifier_flags_.clear();
  }

  bool is_pressed(modifier_flag modifier_flag) const {
    int count = 0;
    size_t size = 0;

    int led_count = 0;

    for (const auto& f : active_modifier_flags_) {
      if (f.get_modifier_flag() == modifier_flag) {
        if (f.led_lock()) {
          led_count += f.get_count();
        } else {
          count += f.get_count();
          ++size;
        }
      }
    }

    if (size == 0) {
      // Use led lock if other flags do not exist.
      return led_count > 0;
    } else {
      // Ignore led lock if other flags exist.
      return count > 0;
    }
  }

  size_t active_modifier_flags_size(void) const {
    return active_modifier_flags_.size();
  }

  size_t led_lock_size(modifier_flag modifier_flag) const {
    return std::count_if(std::begin(active_modifier_flags_),
                         std::end(active_modifier_flags_),
                         [&](const active_modifier_flag& f) {
                           return f.get_modifier_flag() == modifier_flag && f.led_lock();
                         });
  }

  size_t sticky_size(modifier_flag modifier_flag) const {
    return std::count_if(std::begin(active_modifier_flags_),
                         std::end(active_modifier_flags_),
                         [&](const active_modifier_flag& f) {
                           return f.get_modifier_flag() == modifier_flag && f.sticky();
                         });
  }

  bool is_sticky_active(modifier_flag modifier_flag) const {
    auto count = std::accumulate(std::begin(active_modifier_flags_),
                                 std::end(active_modifier_flags_),
                                 0,
                                 [&](const auto& count, const auto& f) {
                                   if (f.get_modifier_flag() == modifier_flag && f.sticky()) {
                                     return count + f.get_count();
                                   }
                                   return count;
                                 });
    return count > 0;
  }

  pqrs::karabiner::driverkit::virtual_hid_device_driver::hid_report::modifiers make_hid_report_modifiers(void) const {
    pqrs::karabiner::driverkit::virtual_hid_device_driver::hid_report::modifiers modifiers;

    std::array<modifier_flag, 8> modifier_flags{
        modifier_flag::left_control,
        modifier_flag::left_shift,
        modifier_flag::left_option,
        modifier_flag::left_command,
        modifier_flag::right_control,
        modifier_flag::right_shift,
        modifier_flag::right_option,
        modifier_flag::right_command,
    };
    for (const auto& m : modifier_flags) {
      if (is_pressed(m)) {
        if (auto r = make_hid_report_modifier(m)) {
          modifiers.insert(*r);
        }
      }
    }

    return modifiers;
  }

private:
  void erase_pairs(void) {
    for (size_t i1 = 0; i1 < active_modifier_flags_.size(); ++i1) {
      for (size_t i2 = i1 + 1; i2 < active_modifier_flags_.size(); ++i2) {
        if (active_modifier_flags_[i1].is_paired(active_modifier_flags_[i2])) {
          active_modifier_flags_.erase(std::begin(active_modifier_flags_) + i2);
          active_modifier_flags_.erase(std::begin(active_modifier_flags_) + i1);
          if (i1 > 0) {
            --i1;
          }
          break;
        }
      }
    }
  }

  std::vector<active_modifier_flag> active_modifier_flags_;
};
} // namespace krbn
