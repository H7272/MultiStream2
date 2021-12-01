
# Multi Streaming

# Provides real-time images for Python code.
You can send the image from the USB camera onto your python code and immediately test the image processing.

![System](https://user-images.githubusercontent.com/73897538/144244336-7216f988-45d1-44b6-b7e9-572a11e31c5e.jpg)

## Feature
 Multiple Client can connect a one Camera Server So Share The Same Image.
## Environment 
Server(Camera)
- Windows 10.
- C++ Application(MultiStream.exe).

Client(Accept Image Data)
- Python 3.8
- MultiStream2/SampleClient/SimpleWinUDP.py

## How to use
1. Modify **Setting.ini** to suit your USB camera and network conditions.
2. Execute MultiStream.exe on the PC to which the USB camera is connected.
3. Execute Python code (**SimpleWinUDP.py**) on the client PC.

### Cautions
The author does not guarantee the operation of the target software and does not take any responsibility for the result that occurs.
