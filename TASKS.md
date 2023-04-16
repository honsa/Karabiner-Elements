# Tasks

-   [x] Migrate Catch2 to boost-ext/ut.
-   [ ] Migrate Objective-C code to Swift.
    -   [ ] KarabinerKit
    -   [ ] MultitouchExtension
    -   [x] EventViewer
    -   [x] Menu
    -   [x] Settings
    -   [x] NotificationWindow
-   [ ] Migrate to SwiftUI.
    -   [ ] MultitouchExtension
    -   [x] Settings
    -   [x] EventViewer
    -   [x] NotificationWindow
-   [ ] Update launch daemons and agents with dropping macOS 12 support.
    -   <https://developer.apple.com/documentation/servicemanagement/updating_helper_executables_from_earlier_versions_of_macos>
-   [ ] Add disable scroll wheel feature.
-   [ ] Add scroll wheel to source events.
-   [ ] Add mouse movement to source events.
-   [ ] Add `event_type` to `to` in order to support `key_down` only event.
-   [ ] Support ignoring debouncing events

## Done

-   [x] Support sticky keys without lock
        <https://github.com/pqrs-org/Karabiner-Elements/issues/477>

## Wontfix

-   [x] Add IOHIDSystem keyboard implementation to support own adjustable key repeat feature.
    -   IOHIDSystem has been deprecated at macOS Big Sur (macOS 11.0).
