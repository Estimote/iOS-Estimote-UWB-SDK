## Introduction to Estimote UWB Beacons iOS SDK

Welcome to the Era of Contextual Computing!

This README is created to assist you in developing next-generation, real-world applications that offer magical experiences through location-based and spatial awareness technologies, powered by Estimote UWB Beacons.

Our **Estimote UWB SDK** is a software library designed to showcase precise ranging capabilities between [Estimote UWB Beacons](https://estimote.com/uwb-beacons) and UWB-enabled iPhones (iPhone 11 to iPhone 14 with U1 chip or iPhone 15 and newer with 2nd-gen UWB chip). It leverages the Core Bluetooth API and the Nearby Interactions API to discover, connect to, and range between UWB-enabled iPhones and beacons.

To integrate the Estimote UWB SDK into your project, you need to import the `EstimoteUWB` files into your Xcode project. Additionally, ensure that you have added all the necessary permissions to your `Info.plist` file for the SDK to function correctly. Detailed instructions on the required permissions are provided towards [the end of this document.](https://github.com/Estimote/iOS-Estimote-UWB-SDK/blob/master/README.md#required-user-permissions)

```swift
import EstimoteUWB
```

### Getting Started with Estimote UWB Beacons

Imagine beacons as compact, battery-operated computers equipped with sensors and various radio technologies (BLE, UWB).

By design, beacons function in a low-power mode, primarily broadcasting their presence via Bluetooth Low Energy (BLE). This efficient use of power allows them to operate for several years without needing a battery replacement.

![Estimote UWB Beacons](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/estimote-uwb-beacons.png)

**Scanning for Nearby Beacons Over BLE**

To detect beacons in your proximity, you'll need to run the `startScanning()` method provided by the UWB Manager class. This method initiates the scanning process for nearby beacons.

```swift
uwbManager.startScanning()
```


Under the hood, the scanning process uses Core Bluetooth to search for available Bluetooth Low Energy (BLE) packets. It specifically parses only those packets advertised by our UWB Beacons. 

Upon successful discovery the **didDiscover()** event is triggered. This event is integral to our SDK, as it signifies the detection of a beacon. In the provided code example, we use this event to print the discovery results.

```swift
func didDiscover(device: UWBIdentifiable, with rssi: NSNumber, from manager: EstimoteUWBManager) 
{
  print("Discovered device: \(device.publicIdentifier) rssi: \(rssi)")
}
```

When you run our code sample, your Xcode console log might display something like this:

```
Discovered device: 31787018dfdd4c2fb241f563081da8d2 rssi: -71
Discovered device: b288b83142c74bf0ab28178d93532e32 rssi: -40
```

The long ID is a unique and persistent identifier for each beacon. You can view the same identifier when using our [Estimote UWB app](https://apps.apple.com/us/app/estimote-uwb/id1593848641) from the App Store or when you log in into your [Estimote Cloud](https://cloud.estimote.com/) account.

![Estimote UWB Beacons](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/estimote-uwb-app.png)

> [!NOTE]
> Your Cloud account user login is typically email address you have used to purchase your Dev Kit.

![Estimote UWB Beacons](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/estimote-cloud-uwb-beacons.png)

```
Discovered device: 31787018dfdd4c2fb241f563081da8d2 rssi: -71
Discovered device: b288b83142c74bf0ab28178d93532e32 rssi: -40
```

Next to the identifier, the rssi value represents the received signal strength index (RSSI) in dB units. A higher value indicates closer proximity to the beacon. For instance, -40 dB suggests a relatively close distance (20-50cm), whereas -90 dB indicates a much greater distance (several meters away).

![Estimote UWB Beacons - BLE rssi](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/rssi.gif)

>[!IMPORTANT]
>It's important to note that the RSSI value is determined by the Core Bluetooth API and does not reflect precise UWB ranging yet. RSSI can be quite variable, fluctuating based on the orientation of the phone or obstacles between the phone and the beacon. Therefore, RSSI should only be used as an indication of which beacons are relatively nearby, especially since your phone can scan and discover hundreds of beacons in the vicinity.


**UWB two-way ranging**

If you are running our code example you will notice that UWB Manager instance is created with an option **shouldHandleConnectivity** set to **true**:  

```swift
uwbManager = EstimoteUWBManager(delegate: self, options: EstimoteUWBOptions(
shouldHandleConnectivity: true,isCameraAssisted: false))
```

This setting indicates that the UWB Manager will automatically establish a Bluetooth connection to any discovered beacons. Consequently, whenever the phone connects to a beacon, the **didConnect()** event is triggered.

```swift
func didConnect(to device: UWBIdentifiable) 
{
  print("Successfully connected to: \(device.publicIdentifier)")
}
```

Once the phone and beacon are connected via Bluetooth, they both turn on their UWB radios and begin exchanging security tokens necessary to initiate the Nearby Interactions session.

After the session is successfully established, both devices run two-way UWB ranging, which yields precise measurements of distance and orientation between them. The technique is called **time-of-flight**. Both the iPhone and the beacon have very precise clocks and they measure time of radio propagation back-and-forth. Multiplying this time by speed of light (speed of electromagnetic radio waves) they can compute distance down to few inches (10cm) precision.

![Estimote UWB Beacons](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/estimote-uwb-beacons-ranging.gif)

Each time a distance is computed, the **didUpdatePosition()** event is triggered. In our example app, we print the ranging results to demonstrate how the system works in real-time:

```swift
func didUpdatePosition(for device: EstimoteUWBDevice) 
{
  print("Position updated for device: \(device)")
}
```

In the Xcode console you should see something like below:

```
UWBDevice(31787018dfdd4c2fb241f563081da8d2 - distance: 1.559030302, direction: Vector(x: 0.056995958, y: -0.13974775, z: -0.9859779),
```

- long **id** is the unique identifier of the beacon which phone is ranging with
- **distance** is a measured value in meters between phone and beacon
- **direction** is a vector of orientation from where the beacon signal is coming from

Note the direction vector is computed using a technique known as **angle-of-arrival (AoA)**.

The iPhone 11, 12, and 13 are equipped with 3 distinct UWB antennas, each strategically positioned within the device. 

As a UWB signal from a beacon reaches these antennas in sequence—first hitting antenna 1, then antenna 2, and finally antenna 3—the time differences between these receptions enable iOS to calculate the angle from which the signal arrived. Our SDK then translates this angle into a normalized vector ranging from [0..1] in x, y, z coordinates, providing a precise indication of the beacon's direction relative to the phone. See [Apple documentation for Direction.](https://developer.apple.com/documentation/nearbyinteraction/ninearbyobject/3601347-direction)

```swift
func didUpdatePosition(for device: EstimoteUWBDevice)
{
 print("Distance: \(device.publicIdentifier)")

 print("Distance: \(device.distance)")
        
 if let vector = device.vector
        {
         print("X orientation: \(vector.x)")
         print("Y orientation: \(vector.y)")
         print("Z orientation: \(vector.z)")
        }
}
```


>[!IMPORTANT]
>It's important to remember that due to the inherent limitations of radio signal propagation, computing the angle is feasible only when the beacon is "in front of the phone." A simple way to conceptualize this is to consider whether the beacon would be visible to the phone's camera. If the camera could "see" the beacon, then it's likely that the phone's multiple UWB antennas could also "hear" it. This mental model helps in visualizing the positional relationship required for accurate angle-of-arrival (AoA) calculations. Read more on [Apple documentation for Nearby Interactions.](https://developer.apple.com/documentation/nearbyinteraction/initiating_and_maintaining_a_session#3608165)

If the beacon is located behind the phone, such that the camera wouldn't be able to "see" it, you can still obtain UWB ranging and have the distance value computed. However, the **direction** will be **unavailable**. 

```
UWBDevice(31787018dfdd4c2fb241f563081da8d2 - distance: 1.13343707, direction: unavailable 
```

One trick to access direction information, even when the beacon is outside the phone's field of view, is to utilize the **Camera Assist** feature integrated within Apple's ARKit. This feature leverages the camera and IMU sensors to extend the perceptual field of the device, thereby enabling it to "see" the beacon indirectly. 

In order to enable it change the UWB Manager option **isCameraAssisted** to **true**:

```swift
uwbManager = EstimoteUWBManager(delegate: self,
options: EstimoteUWBOptions(shouldHandleConnectivity: true, isCameraAssisted: true))
```

>[!IMPORTANT]
>Please note that Camera Assist is necessary on iPhones 14, 15, and newer models due to Apple's decision to reduce the number of UWB antennas in these devices. Consequently, the orientation is computed only using sensor fusion, which combines data from the Inertial Measurement Unit (IMU) and camera sensors. 

**Manually connecting to UWB Beacons**

If you don't want your app automatically connecting to discovered UWB beacons you can change UWB Manager option **shouldHandleConnectivity** to **false** as shown below:

```swift
uwbManager = EstimoteUWBManager(delegate: self,
options: EstimoteUWBOptions(shouldHandleConnectivity: false, isCameraAssisted: false))
```

To initiate UWB ranging and receive **didUpdatePosition()** callbacks, you must manually connect to a UWB beacon by supplying its public ID, as demonstrated in the example below:

```swift
func didDiscover(device: UWBIdentifiable, with rssi: NSNumber, from manager: EstimoteUWBManager) 
{
  uwbManager?.connect(to: "31787018dfdd4c2fb241f563081da8d2")
}
```

You can also manually disconnect from UWB Beacon:

```swift
uwbManager?.disconnect(from: "31787018dfdd4c2fb241f563081da8d2") 
```

or manually stop scanning for nearby beacons to completely stop **didDiscover** -> **didConnect** -> **didUpdatePosition** flow.

```swift
uwbManager?.stopScanning()
```

**Multiple phones ranging with the same beacons**

One reason to opt for manual connection/disconnection or to start/stop scanning is to allow the same beacons to be accessible by multiple phones simultaneously.

>[!WARNING]
>With the existing UWB firmware on our beacons, when one iPhone establishes a connection and begins ranging with a UWB Beacon, other phones will not be able to discover, connect to, or range with the same beacon at the same time.

To workaround this limitation, you should disconnect from the beacons once you have obtained the necessary distance measurements, thereby making them available for other phones. Implementing a clever synchronization/timing algorithm is essential to enable ranging from multiple phones.

> [!TIP]
> Another reason to disconnect from UWB Beacons is to preserve battery life for both the phone and the UWB beacon. Every time the beacon actively ranges, it depletes its AA battery's energy. Therefore, the best approach is to obtain the distance, then shut down the UWB radio until the user makes a significant move.

**Phone ranging with multiple UWB beacons**

Your phone is able to discover, connect and range at the same time with multiple UWB beacons that around. You can use these measurements to enhance precision and reliabilit of your experience or to create simple triangulation and positioning algorithm.

![Estimote UWB Beacons](https://s3.amazonaws.com/assets.github.estimote.com/uwb-sdk/phone-ranging-multiple-beacons.gif)


### Required user permissions

Please remember that for your app to successfully discover, connect to, and range with UWB Beacons, it requires several crucial permissions to be defined and granted by the user.

Your app will request these permissions from your users once, so it's important to provide a clear explanation of why these permissions are necessary for your application.

- **NSBluetoothAlwaysUsageDescription** is required in order to be able to connect to nearby UWB Beacons over the Bluetooth. **NSNearbyInteractionUsageDescription** is required in order to start UWB ranging session with nearby UWB Beacon to obtain the precise distance and/or orientation.
- **NSCameraUsageDescription** is required only if you want to use Camera Assist feature to obtain orientation of UWB Beacons behind the visible UWB antenna range or with iPhone 14 or newer

> [!IMPORTANT]
> Make sure to add these permissions to your `Info.plist` file otherwise you might not be able see ranging results
 
 ```xml
<key>NSBluetoothAlwaysUsageDescription</key>
<string>Allow our app to connect with beacons to enable in-store experiences</string>

<key>NSNearbyInteractionUsageDescription</key>
<string>Allow our app to range with beacons to locate you in-store and provide magical experiences </string>

<key>NSCameraUsageDescription</key>
<string>Allow our app to use camera sensors to precisely locate you in the store</string>
```

**Background ranging**

Precise UWB ranging is not limited to when the app is actively running in the foreground. With our beacons, it is also feasible to range and calculate the distance to beacons in the background, even when the phone is locked or hidden in a pocket.

Implementing ranging in the background necessitates additional modifications in the app and SDK.

**Authentication and security**


This UWB SDK is part of the Estimote UWB Beacons Development Kit, designed to showcase the technology and offer the essential tools for evaluating our hardware and software offerings.


>[!CAUTION]
>UWB Beacons that are sold as part of Development Kits do not have authentication enabled. 
This implies that anyone with access to this SDK can discover, connect to, and range with your beacons draining their battery and obtaining their location/orientation.

If you require a secure solution for deployment in a production environment, please contact our team to discuss licensing, as well as production firmware and hardware options.

**Settings and customization**

Our UWB Beacons sold as part of the Development Kit have default settings for Bluetooth advertising interval as well as disconnect timeout or UWB ranging frequency. If your use-case requires different settings please contact our team.


### Contact Estimote Team

Our contact details are provided on our website [www.estimote.com](https://estimote.com) and our customer success team is always available at contact (at) estimote.com to discuss business opportunities or opening a support project. We have shipped milions of beacons and have seen most sophisticated use-cases and are always happy to recommend the best approach or discuss firmware/hardware customization for your project.

Unfortunately our engineers are not able to provide tech assistance to every Dev Kit customer without support project initiated. Free tech support can be only provided via our [Developer Forum](https://forums.estimote.com/).
