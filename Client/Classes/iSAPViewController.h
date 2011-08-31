#import <UIKit/UIKit.h>
#import "CF_util.h"

@interface iSAPViewController : UITableViewController <UITextFieldDelegate> {
	UITextField *activeTextField;
}

+ (iSAPViewController *) sharedInstance;
- (void) showAlert;

@end

