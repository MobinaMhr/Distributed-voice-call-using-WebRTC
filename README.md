# Computer-Networks-Course-S2025
Computer Networks course projects in the spring semester of the University of Tehran under the supervision of Dr.Yazdani. 

### Main Files
#### main.cpp
This is the entrly point of the application. It initializes the Qt application adn sets up the main window(UI).
#### main.qml
This is the main QML file for the user interface. It displays the call controls, connected peers, and other UI elements.

### WebRTC Implementation
The provided WebRTC class is designed to manage WebRTC peer connections, handling SDP (Session Description Protocol) generation, ICE (Interactive Connectivity Establishment) candidate gathering, and the management of audio streams. It integrates with the libdatachannel library and uses Qt 6 for event-driven communication. The class includes functionalities for creating peer connections, managing SDP offers/answers, adding audio tracks, and handling RTP (Real-time Transport Protocol) packet transmission.

The WebRTC class can be used for real-time audio communication, where it establishes peer-to-peer connections, negotiates media capabilities, and transmits audio data in the form of RTP packets.

#### Class Structure
- Constructor and Destructor
    - Initializes the WebRTC object with a default audio track and sets up signal connections.
    - Cleans up resources in the destructor.
- Public Methods
    - Methods to initialize the WebRTC instance, add peers, generate SDP offers/answers, and send audio tracks.
- Public Slots
    - Slots for setting remote descriptions and ICE candidates.
- Private Methods
    - Utility methods to generate SDP in JSON format and retrieve the current timestamp for RTP synchronization.
- RTP Header Definition
    - A packed structure that defines the RTP header used for audio data packetization.


<!-- #### webrtc.h
- Header file for the WebRTC class.
- Contains declarations for methods and member variables.
#### webrtc.cpp
- Source file for the WebRTC class.
- Implements the signaling, peer connection management, and media handling (audio tracks). -->

### Audio Handling
#### AudioInput Class
This class captures audio from the microphone, encodes it using the Opus codec, and sends it to the WebRTC connection.
- audioinput.hpp
Header file for the AudioInput class.
Contains declarations for audio capture methods and signals.

- audioinput.cpp
Source file for the AudioInput class.
Implements audio capture from the microphone, encoding with Opus, and sending the encoded audio.
#### AudioOutput Class
This class is responsible for receiving audio packets from the WebRTC track, decoding them using Opus codec, and playing them through the speaker.

- audiooutput.hpp
Header file for the AudioOutput class.
Contains declarations for audio playback methods and signals.

- audiooutput.cpp
Source file for the AudioOutput class.
Implements receiving Opus-encoded audio, decoding it, and playing it through the speaker.

### Call Management
The `CallManager` class handles call-related data and operations, such as managing the caller ID, IP address, and ICE candidates necessary for establishing a WebRTC call.

<!-- ### Signaling Server (Optional) -->