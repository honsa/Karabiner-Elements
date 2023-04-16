@import IOKit;
#import "AppDelegate.h"
#import "FingerCount.h"
#import "FingerStatusManager.h"
#import "KarabinerKit/KarabinerKit.h"
#import "MultitouchDeviceManager.h"
#import "MultitouchPrivate.h"
#import "NotificationKeys.h"
#import "PreferencesController.h"
#import "PreferencesKeys.h"
#import <pqrs/weakify.h>

//
// C methods
//

static void setGrabberVariable(FingerCount count, bool sync) {
  static FingerCount previousFingerCount;
  static bool previousFingerCountInitialized = false;

  if (!previousFingerCountInitialized) {
    previousFingerCountInitialized = true;
    previousFingerCount.upperQuarterAreaCount = -1;
    previousFingerCount.lowerQuarterAreaCount = -1;
    previousFingerCount.leftQuarterAreaCount = -1;
    previousFingerCount.rightQuarterAreaCount = -1;
    previousFingerCount.upperHalfAreaCount = -1;
    previousFingerCount.lowerHalfAreaCount = -1;
    previousFingerCount.leftHalfAreaCount = -1;
    previousFingerCount.rightHalfAreaCount = -1;
    previousFingerCount.totalCount = -1;
  }

#define ENTRY(COUNT_NAME, VARIABLE_NAME) \
  { count.COUNT_NAME, &(previousFingerCount.COUNT_NAME), VARIABLE_NAME }

  struct {
    int count;
    int* previousCount;
    const char* name;
  } entries[] = {
      ENTRY(upperQuarterAreaCount, "multitouch_extension_finger_count_upper_quarter_area"),
      ENTRY(lowerQuarterAreaCount, "multitouch_extension_finger_count_lower_quarter_area"),
      ENTRY(leftQuarterAreaCount, "multitouch_extension_finger_count_left_quarter_area"),
      ENTRY(rightQuarterAreaCount, "multitouch_extension_finger_count_right_quarter_area"),
      ENTRY(upperHalfAreaCount, "multitouch_extension_finger_count_upper_half_area"),
      ENTRY(lowerHalfAreaCount, "multitouch_extension_finger_count_lower_half_area"),
      ENTRY(leftHalfAreaCount, "multitouch_extension_finger_count_left_half_area"),
      ENTRY(rightHalfAreaCount, "multitouch_extension_finger_count_right_half_area"),
      ENTRY(totalCount, "multitouch_extension_finger_count_total"),
  };

#undef ENTRY

  for (int i = 0; i < sizeof(entries) / sizeof(entries[0]); ++i) {
    if (*(entries[i].previousCount) != entries[i].count) {
      if (sync) {
        libkrbn_grabber_client_sync_set_variable(entries[i].name, entries[i].count);
      } else {
        libkrbn_grabber_client_async_set_variable(entries[i].name, entries[i].count);
      }

      *(entries[i].previousCount) = entries[i].count;
    }
  }
}

static void enable(void) {
  dispatch_async(dispatch_get_main_queue(), ^{
    MultitouchDeviceManager* manager = [MultitouchDeviceManager sharedMultitouchDeviceManager];

    [manager registerIONotification];
    [manager registerWakeNotification];

    // sleep until devices are settled.
    [NSThread sleepForTimeInterval:1.0];

    [manager setCallback:YES];

    FingerCount fingerCount;
    memset(&fingerCount, 0, sizeof(fingerCount));
    setGrabberVariable(fingerCount, false);
  });
}

static void disable(void) {
  dispatch_async(dispatch_get_main_queue(), ^{
    MultitouchDeviceManager* manager = [MultitouchDeviceManager sharedMultitouchDeviceManager];

    [manager unregisterIONotification];
    [manager unregisterWakeNotification];

    [manager setCallback:NO];
  });
}

//
// AppDelegate
//

@interface AppDelegate ()

@property(weak) IBOutlet PreferencesController* preferences;
@property KarabinerKitSmartObserverContainer* observers;
@property id activity;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  [NSProcessInfo.processInfo enableSuddenTermination];

  [KarabinerKit setup];
  [KarabinerKit observeConsoleUserServerIsDisabledNotification];

  [[NSApplication sharedApplication] disableRelaunchOnLogin];

  //
  // Handle kHideIconInDock
  //

  if (![[NSUserDefaults standardUserDefaults] boolForKey:kHideIconInDock]) {
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
  }

  //
  // Handle --start-at-login
  //

  if (self.startAtLogin) {
    if (![[NSUserDefaults standardUserDefaults] boolForKey:kStartAtLogin]) {
      [NSApp terminate:nil];
    }
  }

  //
  // Handle --show-ui
  //

  if (self.showUI) {
    [self.preferences show];
  }

  //
  // Prepare observers
  //

  self.observers = [KarabinerKitSmartObserverContainer new];
  @weakify(self);

  {
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    id o = [center addObserverForName:kFingerStateChanged
                               object:nil
                                queue:[NSOperationQueue mainQueue]
                           usingBlock:^(NSNotification* note) {
                             @strongify(self);
                             if (!self) {
                               return;
                             }

                             FingerStatusManager* manager = [FingerStatusManager sharedFingerStatusManager];
                             setGrabberVariable([manager createFingerCount], false);
                           }];
    [self.observers addObserver:o notificationCenter:center];
  }

  //
  // Enable grabber_client
  //

  libkrbn_enable_grabber_client(enable,
                                disable,
                                disable);

  //
  // Disable App Nap
  //

  NSActivityOptions options = NSActivityUserInitiated &
                              ~NSActivityIdleSystemSleepDisabled &
                              ~NSActivityIdleDisplaySleepDisabled &
                              ~NSActivitySuddenTerminationDisabled;
  NSString* reason = @"Disable App Nap in order to receive multitouch events even if this app is background";
  self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:options reason:reason];
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
  if (self.activity) {
    [[NSProcessInfo processInfo] endActivity:self.activity];
    self.activity = nil;
  }

  [[MultitouchDeviceManager sharedMultitouchDeviceManager] setCallback:NO];

  FingerCount fingerCount;
  memset(&fingerCount, 0, sizeof(fingerCount));
  setGrabberVariable(fingerCount, true);

  libkrbn_terminate();
}

- (BOOL)applicationShouldHandleReopen:(NSApplication*)theApplication hasVisibleWindows:(BOOL)flag {
  [self.preferences show];
  return YES;
}

- (BOOL)startAtLogin {
  return [[[NSProcessInfo processInfo] arguments] indexOfObject:@"--start-at-login"] != NSNotFound;
}

- (BOOL)showUI {
  return [[[NSProcessInfo processInfo] arguments] indexOfObject:@"--show-ui"] != NSNotFound;
}

@end
