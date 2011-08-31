#import "iSAPViewController.h"

@implementation iSAPViewController

BOOL reqChangeMode;
BOOL reqPIN;
const char *pin;
UISwitch *resetControl;
UITextField *stateControl;
uint8_t curState;
CFMessagePortRef clientPort;

+ (iSAPViewController *) sharedInstance{
	return [self alloc];
}

void showAlertMes(const char *mes) {
	iSAPViewController *vc = [iSAPViewController sharedInstance];	
	UIAlertView *pinAlert = 
	[[UIAlertView alloc] initWithTitle:@"PIN code" 
			message:[NSString stringWithUTF8String:mes]
			delegate:vc cancelButtonTitle:NSLocalizedString(@"Cancel",nil) otherButtonTitles:NSLocalizedString(@"OK",nil), nil];
	
	[pinAlert setTransform:CGAffineTransformMakeTranslation(0,109)];
	[pinAlert show];
	[pinAlert release];		
}

void changeStateText(uint8_t newState) {
	
	if(!reqChangeMode)
	{
		if(newState > 0)
		{
			resetControl.on = YES;
		}
		else {
			resetControl.on = NO;
		}
	}
	
	switch (newState) {
		case iSAP_state_error:
			stateControl.text = @"Error!";
			break;
		case iSAP_state_off:			
			stateControl.text = @"Disabled";
			break;
		case iSAP_state_starting:
			stateControl.text = @"Initializing BTstack";
			break;
		case iSAP_state_ready:
			stateControl.text = @"Waiting for connection";
			break;
		case iSAP_state_starting_hci:
			stateControl.text = @"Initializing HCI";
			break;
		case iSAP_state_starting_l2cap:
			stateControl.text = @"Initializing L2CAP";
			break;
		case iSAP_state_starting_rfcomm:
			stateControl.text = @"Initializing RFCOMM";
			break;
		case iSAP_state_starting_sap:
			stateControl.text = @"Initializing rSAP";
			break;
		case iSAP_state_starting_working:
			stateControl.text = @"Connected !";
			break;			
		default:
			break;
	}
}

- (void) showAlert {
	//iSAPViewController *vc = [iSAPViewController sharedInstance];	
	UIAlertView *pinAlert = 
		[[UIAlertView alloc] initWithTitle:@"PIN code" message:@"\n\n\n" 
		 delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel",nil) otherButtonTitles:NSLocalizedString(@"OK",nil), nil];
	
	UILabel *pinLabel = [[UILabel alloc] initWithFrame:CGRectMake(12,40,260,25)];
	pinLabel.font = [UIFont systemFontOfSize:16];
	pinLabel.textColor = [UIColor whiteColor];
	pinLabel.backgroundColor = [UIColor clearColor];
	pinLabel.shadowColor = [UIColor blackColor];
	pinLabel.shadowOffset = CGSizeMake(0,-1);
	pinLabel.textAlignment = UITextAlignmentCenter;
	pinLabel.text = @"Please enter requested PIN code";
	[pinAlert addSubview:pinLabel];
			
	UITextField *pinField = [[UITextField alloc] initWithFrame:CGRectMake(16,83,252,25)];
	pinField.secureTextEntry = NO;
	pinField.keyboardAppearance = UIKeyboardAppearanceAlert;
	pinField.delegate = self;
	pinField.borderStyle = UITextBorderStyleRoundedRect;
	pinField.tag = 1;
	[pinField setKeyboardType:UIKeyboardTypeNumberPad];
	[pinField becomeFirstResponder];
	[pinAlert addSubview:pinField];
	
	[pinAlert setTransform:CGAffineTransformMakeTranslation(0,109)];
	[pinAlert show];
	[pinAlert release];
	[pinField release];
	[pinLabel release];	
}

CFDataRef myCallBack(CFMessagePortRef local, SInt32 msgid, CFDataRef cfData, void *info) {
	
	const char *data = (const char *) CFDataGetBytePtr(cfData);
	UInt16 dataLen = CFDataGetLength(cfData);
	CFDataRef returnData = NULL;
	if (!data) return NULL;
	
	uint8_t res[20];
	switch (msgid){
		case CFMSG_nop: {						
			
			if(dataLen > 0)
			{				
				changeStateText((uint8_t) data[0]);
			}
			
			if (reqChangeMode) {
				uint8_t enabled = 0;
				if(resetControl != NULL && [resetControl isOn])
				{
					enabled = 1;
				}
				
				res[0] = CFMSG_setMode;
				res[1] = enabled;
				returnData = CFDataCreate(kCFAllocatorDefault, (const UInt8 *) res, 2);
				reqChangeMode = FALSE;
				break;
			}
			if (reqPIN) {

				res[0] = CFMSG_setPIN_Resp;
				int i;
				for (i = 0; i < 16; i++) {
					res[1+i] = pin[i];
				}				
				
				returnData = CFDataCreate(kCFAllocatorDefault, (const UInt8 *) res, 17);
				reqPIN = FALSE;
				break;
			}
			break;
		}
		case CFMSG_setPIN: {
			[[iSAPViewController sharedInstance] showAlert];
			break;
		}
		case CFMSG_newState: {
			if (!dataLen) return NULL;
			
			changeStateText((uint8_t) data[0]);
			
			break;
		}
		default:
			printf("Unknown command %u, len %u", data[0], dataLen); 
	}
	
	return returnData;
}

- (id) init {
	self = [ super initWithStyle: UITableViewStyleGrouped ];
	
	if (self != nil) {
		self.title = @"Settings";
	}
	
	clientPort = createMessagePortGUI(CFSTR(_messagePortName_client), (CFMessagePortCallBack)&myCallBack);
	if(clientPort == NULL)
	{
		showAlertMes("Invalid Port!");
	}
	
	return self;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 3;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	switch (section) {
		case(0):
			return 2;
			break;
		case(1):
			return 1;
			break;
		case(2):
			return 1;
			break;			
	}
	
	return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	switch (section) {
		case(0):
			return @"Settings";
			break;
		case(1):
			return @"State";
			break;
		case(2):
			return @"About";
			break;
	}
	return nil;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSString *CellIdentifier = [ NSString stringWithFormat: @"%d:%d", [ indexPath indexAtPosition: 0 ], [ indexPath indexAtPosition:1 ]];
	
	UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier: CellIdentifier];
	
	if (cell == nil) {
		cell = [ [ [ UITableViewCell alloc ] initWithFrame: CGRectZero reuseIdentifier: CellIdentifier] autorelease ];
		
		cell.selectionStyle = UITableViewCellSelectionStyleNone;
		
		switch ([ indexPath indexAtPosition: 0]) {
			case(0):
				switch([ indexPath indexAtPosition: 1]) {
					case(0):
					{
						resetControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200, 10, 0, 0) ];
						resetControl.on = NO;
						resetControl.tag = 1;
						[resetControl addTarget:self action:@selector(switchAction:) forControlEvents:UIControlEventValueChanged];
						[ cell addSubview: resetControl ];
						cell.textLabel.text = @"Enabled";
						[resetControl release];
						
					}
						break;
					case(1):
					{
						UITextField *versionControl = [ [ UITextField alloc ] initWithFrame: CGRectMake(200, 10, 145, 38) ];
						versionControl.text = @"iSAP";
						[ cell addSubview: versionControl ];
						
						[ versionControl setEnabled: YES ];
						versionControl.tag = 2;
						versionControl.delegate = self;
						cell.textLabel.text = @"Device Name";
						[versionControl release];
					}
						break;
				}
				break;
			case(1):
			{
				stateControl = [ [ UITextField alloc ] initWithFrame: CGRectMake(20, 10, 200, 38) ];
				stateControl.text = @"Connecting to the service";
				[ cell addSubview: stateControl ];
				
				[ stateControl setEnabled: YES ];
				stateControl.tag = 3;
				stateControl.delegate = self;
				[stateControl release];
				
			}
				break;
			case(2):
			{
				UITextField *versionControl = [ [ UITextField alloc ] initWithFrame: CGRectMake(200, 10, 145, 38) ];
				versionControl.text = @"0.2.0";
				[ cell addSubview: versionControl ];
				
				[ versionControl setEnabled: YES ];
				versionControl.tag = 2;
				versionControl.delegate = self;
				cell.textLabel.text = @"Version";
				[versionControl release];
			}
				break;
		}
	}
	
	return cell;
}

- (void)switchAction:(UISwitch*)sender
{
	if ([activeTextField canResignFirstResponder])
		[activeTextField resignFirstResponder];
	
	reqChangeMode = TRUE;
}


#pragma mark Methods

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
	activeTextField = textField;
	return YES;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{	
	switch ([textField tag]) {
		case 1:
			pin = [textField.text cStringUsingEncoding:NSASCIIStringEncoding];			
			reqPIN = TRUE;
			
			//sendMessageToGUI(CFMSG_nop, 0, NULL, NULL, CFSTR(_messagePortName_client));
			break;
		default:
			break;
	}
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	activeTextField = nil;
	[textField resignFirstResponder];
	return YES;
}

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	//[self showAlert]; 
	//sendMessageToGUI(CFMSG_setPIN, 0, NULL, NULL, CFSTR(_messagePortName_client));
	[super viewDidLoad];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
}

- (void)dealloc {
	
	if (clientPort != NULL) {
		CFMessagePortInvalidate(clientPort);
		CFRelease(clientPort);
	}
	
    [super dealloc];
}

@end
