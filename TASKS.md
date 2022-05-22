# Tasks

-   [ ] Migrate Objective-C code to Swift.
    -   [x] EventViewer
    -   [ ] KarabinerKit
    -   [x] Menu
    -   [ ] Preferences
    -   [ ] MultitouchExtension
    -   [x] NotificationWindow
-   [ ] Migrate to SwiftUI.
    -   [ ] Preferences
    -   [ ] MultitouchExtension
    -   [x] EventViewer
    -   [x] NotificationWindow
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
