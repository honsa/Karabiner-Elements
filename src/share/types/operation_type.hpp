#pragma once

#include <nlohmann/json.hpp>

namespace krbn {
enum class operation_type : uint8_t {
  none,
  // observer -> grabber
  momentary_switch_event_arrived,
  observed_devices_updated,
  caps_lock_state_changed,
  // session_monitor -> grabber
  console_user_id_changed,
  // console_user_server -> grabber
  connect_console_user_server,
  system_preferences_updated,
  frontmost_application_changed,
  input_source_changed,
  // any -> grabber
  set_app_icon,
  set_keyboard_type,
  set_variables,
  // grabber -> console_user_server
  shell_command_execution,
  select_input_source,
  software_function,
  end_,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    operation_type,
    {
        {operation_type::none, nullptr},
        {operation_type::momentary_switch_event_arrived, "momentary_switch_event_arrived"},
        {operation_type::observed_devices_updated, "observed_devices_updated"},
        {operation_type::caps_lock_state_changed, "caps_lock_state_changed"},
        {operation_type::console_user_id_changed, "console_user_id_changed"},
        {operation_type::connect_console_user_server, "connect_console_user_server"},
        {operation_type::system_preferences_updated, "system_preferences_updated"},
        {operation_type::frontmost_application_changed, "frontmost_application_changed"},
        {operation_type::input_source_changed, "input_source_changed"},
        {operation_type::set_app_icon, "set_app_icon"},
        {operation_type::set_keyboard_type, "set_keyboard_type"},
        {operation_type::set_variables, "set_variables"},
        {operation_type::shell_command_execution, "shell_command_execution"},
        {operation_type::select_input_source, "select_input_source"},
        {operation_type::software_function, "software_function"},
        {operation_type::end_, "end_"},
    });
} // namespace krbn
