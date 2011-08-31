CC = /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/arm-apple-darwin10-gcc-4.2.1
LDFLAGS  =  -framework CoreFoundation -F/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/System/Library/Frameworks -L/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/usr/lib -framework Foundation -framework CoreFoundation -framework UIKit -L/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/System/Library/Frameworks/IOKit.framework/Versions/A /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/System/Library/PrivateFrameworks/BluetoothManager.framework/BluetoothManager -lIOKit -lobjc -lBTstack -L../../src
CPPFLAGS =  -I/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/usr/include -I/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/usr/lib/gcc/arm-apple-darwin10/4.2.1/include -F/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.1.3.sdk/System/Library/Frameworks -Werror -I../../include -I../../src
prefix = /usr/local

socket_sources = ../../src/socket_connection.c
iSAP_SOURCES = commons.c rfcomm.c sap_util.c at_utils.c sdp_utils.c CF_util.c $(socket_sources) iSAP.c

all: iSAP

iSAP: $(iSAP_SOURCES)
	$(CC) $(CPPFLAGS) -o $@ $(iSAP_SOURCES) $(LDFLAGS)

clean:
	rm -rf iSAP
	
install:    
	echo "nothing to be done"

