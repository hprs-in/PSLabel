# ESL Service Implementation

This document describes the Electronic Shelf Label (ESL) service implementation for the PSLabel project.

## Overview

The ESL service provides Bluetooth Low Energy (BLE) GATT characteristics that enable wireless communication with electronic shelf label devices. This implementation follows the Bluetooth ESL specification and provides essential functionality for managing e-ink displays.

## Service Architecture

### Service UUID
- **ESL Service**: `0x181D` (Bluetooth assigned number for ESL)

### Characteristics

#### 1. ESL Control Point (`0x2BE1`)
- **Properties**: Write, Write Without Response
- **Purpose**: Send commands to control the ESL device
- **Commands**:
  - `0x01`: Update Display - Triggers display refresh
  - `0x02`: Clear Display - Clears the display and image buffer
  - `0x03`: Sleep - Puts device into low-power mode
  - `0x04`: Wake - Wakes device from sleep mode
  - `0x05`: Get Status - Requests current device status

#### 2. ESL Image Transfer (`0x2BE2`)
- **Properties**: Write, Write Without Response
- **Purpose**: Transfer image data to the device
- **Format**: First 4 bytes contain total image size, followed by image data
- **Buffer**: 1KB internal buffer for image data

#### 3. ESL Display Information (`0x2BE3`)
- **Properties**: Read
- **Purpose**: Provides display capabilities information
- **Data Structure**:
  - Width (2 bytes): Display width in pixels
  - Height (2 bytes): Display height in pixels
  - Color Depth (1 byte): Bits per pixel
  - Display Type (1 byte): Display technology type

#### 4. ESL Status (`0x2BE4`)
- **Properties**: Read, Notify
- **Purpose**: Reports current device status
- **Status Values**:
  - `0x00`: Idle - Device ready for commands
  - `0x01`: Updating - Display update in progress
  - `0x02`: Sleeping - Device in low-power mode
  - `0xFF`: Error - Device error state

## Usage Example

### Connecting and Updating Display

1. **Connect** to the PSLabel device via BLE
2. **Subscribe** to status notifications to monitor device state
3. **Read** display information to understand capabilities
4. **Write** image data to the Image Transfer characteristic
5. **Send** update command via Control Point to refresh display

### Image Transfer Protocol

```
First packet: [SIZE_BYTE_3][SIZE_BYTE_2][SIZE_BYTE_1][SIZE_BYTE_0][IMAGE_DATA...]
Subsequent packets: [IMAGE_DATA...]
```

## Implementation Details

### Key Features

- **Connection Management**: Handles BLE connection and disconnection events
- **Status Notifications**: Automatic status updates during operations
- **Image Buffering**: 1KB buffer for image data transfer
- **Command Processing**: Full command set implementation
- **Error Handling**: Proper error responses for invalid operations

### Power Management

- Device enters sleep mode when commanded
- Wake commands bring device back to active state
- Status reporting allows monitoring of power states

### Memory Usage

- Image buffer: 1KB
- Display info structure: 6 bytes
- Minimal RAM footprint for embedded applications

## Building and Testing

The ESL service is integrated into the main PSLabel application. Build using:

```bash
west build app -b nrf54l15dk/nrf54l15/cpuapp
```

## Integration Notes

- Service automatically advertises when BLE is enabled
- Compatible with standard BLE ESL client applications
- Designed for Nordic nRF52/nRF54 series MCUs
- Follows Zephyr RTOS conventions and best practices

## Future Enhancements

- NFC wake functionality integration
- OTA update support via BLE
- Multi-image management
- Advanced power optimization
- Error recovery mechanisms