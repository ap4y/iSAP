#import "iSAPAppDelegate.h"
#import "iSAPViewController.h"

@implementation iSAPAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];		
	viewController = [ [ iSAPViewController alloc ] init ];
		
	[window addSubview:[viewController view]];	
	[window makeKeyAndVisible];
				
	return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
	[viewController dealloc];
}

- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
