// -*- mode: objective-c -*-

@import Foundation;
#import "KarabinerKit/SmartObserverContainer.h"
#import "libkrbn/libkrbn.h"

@interface KarabinerKit : NSObject

+ (void)setup;
+ (void)endAllAttachedSheets:(NSWindow*)window;

+ (void)observeConsoleUserServerIsDisabledNotification;

+ (void)relaunch;
+ (BOOL)quitKarabinerWithConfirmation;

@end
