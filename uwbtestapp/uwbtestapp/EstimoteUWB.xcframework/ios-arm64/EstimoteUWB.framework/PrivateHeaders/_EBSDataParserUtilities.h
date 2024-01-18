//
//    ____  _            _              _   _       ____                        _
//   | __ )| |_   _  ___| |_ ___   ___ | |_| |__   / ___|  ___ __ _ _ __  _ __ (_)_ __   __ _
//   |  _ \| | | | |/ _ \ __/ _ \ / _ \| __| '_ \  \___ \ / __/ _` | '_ \| '_ \| | '_ \ / _` |
//   | |_) | | |_| |  __/ || (_) | (_) | |_| | | |  ___) | (_| (_| | | | | | | | | | | | (_| |
//   |____/|_|\__,_|\___|\__\___/ \___/ \__|_| |_| |____/ \___\__,_|_| |_|_| |_|_|_| |_|\__, |
//                                                                                      |___/
//
//
//  Copyright (c) 2015 Estimote. All rights reserved.

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

typedef NS_ENUM(NSInteger, _ECOByteDirection) {
    _ECOByteDirectionOldYoung,
    _ECOByteDirectionYoungOld,
};

/**
 *  ENUM describing bluetooth packet frame type.
 */
typedef NS_ENUM(NSInteger, ECOScanInfoFrameType)
{
    /**
     *  Unknown frame type, parsing error.
     */
    ECOScanInfoFrameTypeUnknown,
    /**
     *  Service Data frame type.
     */
    ECOScanInfoFrameTypeServiceDevice,
    /**
     *  Estimote Telemetry frame type.
     */
    ECOScanInfoFrameTypeEstimoteTelemetry,
    /**
     *  Estimote Mirror frame type.
     */
    ECOScanInfoFrameTypeMirror,
    /**
     *  Estimote Mesh frame type.
     */
    ECOScanInfoFrameTypeMesh
};

/**b
 Contains methods related to NSData parsing.
 If possible, use EBSHexStringParser instead.
 */
@interface _EBSDataParserUtilities : NSObject

/**
 Convert provided data object to string-encoded hex string. The string is lowercase, with no spaces, i.e.
 {0x01, 0xE2} becomes "01e2". The order of byte characters in string follows the ordering of bytes in data object.
 
 @param data Data to be converted.
 @return Hex string. Nil if provided data is nil. Empty string if data has 0 length.
 */
+ (NSString *)hexStringFromData:(NSData *)data;

+ (NSString *)removeAngleBracketsAndSpacesFromString:(NSString *)string;

+ (NSString *)stringFromHex:(NSString *)hexString
              withHexOffset:(int)offset
          withLengthInBytes:(int)length
              withDirection:(_ECOByteDirection)byteDirection;

+ (unsigned)unsignedFromHex:(NSString *)hexString
              withHexOffset:(int)offset
          withLengthInBytes:(int)length
              withDirection:(_ECOByteDirection)byteDirection;

+ (NSData *)bytesFromHexString:(NSString *)hexString;

/**
 *  Method is parsing frame type of Next-Gen
 *  beacons from bluetooth scan response.
 *
 *  @param data advertisementData to parse
 *
 *  @return parsed frame type as ECOScanInfoFrameType.
 */
+ (ECOScanInfoFrameType)parseFrameType:(NSData *)data;

+ (UInt8)parseFrameTypeInt:(NSData *)data;

+ (NSString *)stringForAdvertisementData:(NSDictionary *)advertisementData;

+ (NSInteger)protocolVersionFromAdvertisementDataString:(NSString *)advertisementDataString;

+ (NSInteger)frameTypeFromAdvertisementDataString:(NSString *)advertisementDataString
                            indoorProtocolVersion:(NSInteger)indoorProtocolVersion;

+ (NSString *)identifierFromAdvertisementDataString:(NSString *)advertisementDataString
                              indoorProtocolVersion:(NSInteger)indoorProtocolVersion;
@end

