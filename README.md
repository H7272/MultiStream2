
# Multi Streaming

# Provides Real-time Image to Your Python Code.
You can send the image from the USB camera onto your python code and immediately test the image processing.
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

### Caution
The author does not guarantee the operation of the target software and does not take any responsibility for the result that occurs.
