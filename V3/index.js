const http = require("http")
const Socket = require("websocket").server
const { v4: uuidv4 } = require("uuid");

const server = http.createServer(() => {})

server.listen(3000, () => {
    console.log("Server is running on port 3000")
})

const ws = new Socket({ httpServer: server })

ws.on("request", (req) => {
    const connection = req.accept(null, req.origin)

    connection.on("message", (message) => {
        const data = JSON.parse(message.utf8Data)
        var targetUser
        switch (data.type) {
            case "register":
                console.log(`Received registeration from ${data.user}`)
                peers[data.user] = connection

                break

            case "offer":
                console.log(`Received an offer from ${data.user} to connect to : ${data.target}`)
                targetUser = peers[data.target]
                if (targetUser) {
                    targetUser.send(JSON.stringify({
                        type: "offer",
                        user: data.user,
                        sdp: data.sdp,
                        iceCandidate: data.iceCandidate
                    }))
                } else {
                    console.log(`Target ${data.target} not found`);
                }
                       
                break

                case "answer":
                    console.log(`Received an answer from ${data.user} to connect to : ${data.target}`)
                    targetUser = peers[data.target]
                    if (targetUser) {
                        targetUser.send(JSON.stringify({
                            type: "answer",
                            user: data.user,
                            sdp: data.sdp,
                            iceCandidate: data.iceCandidate
                        }))
                    }

                    break
        }

    })

})