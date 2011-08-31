#import <UIKit/UIKit.h>

@class iSAPViewController;

@interface iSAPAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    iSAPViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet iSAPViewController *viewController;

@end

