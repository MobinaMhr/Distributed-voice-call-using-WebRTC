# Computer-Networks-Course-S2025
Computer Networks course projects in the spring semester of the University of Tehran under the supervision of Dr.Yazdani. 

### Main Files
#### main.cpp
This is the entrly point of the application. It initializes the Qt application adn sets up the main window(UI).
#### main.qml
This is the main QML file for the user interface. It displays the call controls, connected peers, and other UI elements.

### WebRTC Implementation
The `WebRTC` class manages the peer connections and signaling necessary for real-time audio communication using the WebRTC protocol. It uses the libdatachannel library for handling peer-to-peer connections and SDP generation.
#### webrtc.h
- Header file for the WebRTC class.
- Contains declarations for methods and member variables.
#### webrtc.cpp
- Source file for the WebRTC class.
- Implements the signaling, peer connection management, and media handling (audio tracks).

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