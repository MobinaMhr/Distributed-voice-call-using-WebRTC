## index.js 
This Node.js WebSocket server handles real-time communication between peers. It sets up a WebSocket server using Node.js. The server listens for incoming WebSocket connections and handles various types of messages from clients.

- **HTTP and WebSocket Server Setup**:
    - The HTTP server is created and listens on port 3000.
    - A WebSocket server (ws) is created and attached to the HTTP server.
- **Connection Handling** :
    - When a new connection request is received, it is accepted, and a connection is established.
    - Logs the connection establishment.
- **Message Handling** :
    - Listens for messages from clients and processes them based on their request type (register, offer, answer, candidate).
    - For each message type, the server performs specific actions, such as registering a user, forwarding offers/answers, and handling ICE candidates.
- **Disconnection Handling** :
    - Listens for connection closures and removes the disconnected user from the peers list.
    - Logs the disconnection event.

## package.json
This file contains the metadata about the Node.js project, as well as the project's dependancies and scripts.
It contains:
- **name**: The name of the project
- **version**: The version of the project
- **main**: The entry point file for the application.
- **scripts**: 
    - "test": Command to run tests. Currently, it echoes an error message and exits.

    - "start": Command to start your application using nodemon.

- **author** : Placeholder for the author's name.
- **license** : The license under which the project is released.
- **description** : A brief description of the project.
- **dependencies** : 
    - "nodemon": Automatically restarts the node application when file changes are detected.

    - "websocket": A library for WebSocket communication.

## package-lock.json
This file is an automatically generated file in a Node.js project. It provides a detailed description of the exact dependency tree that was generated, ensuring consistent installs across different environments.
