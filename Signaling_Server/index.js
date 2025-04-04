const http = require("http")
const Socket = require("websocket").server

const server = http.createServer(() => {})

server.listen(3000, () => {
    console.log("Server is running on port 3000")
})

const ws = new Socket({ httpServer: server })

let peers = {};

ws.on("request", (req) => {
    const connection = req.accept(null, req.origin)
    console.log("Connection established")
    connection.on("message", (message) => {
        const data = JSON.parse(message.utf8Data)
        console.log(data)
        var targetUser
        switch (data.reqType) {
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
                        Candidate: data.candidate,
                        mid: data.mid
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
                        Candidate: data.candidate,
                        mid: data.mid
                    }))
                } else {
                    console.log(`Target ${data.target} not found`);
                }
                break
            case "candidate":
                console.log(`Received a candidate from ${data.user} to send to : ${data.target}`)
                targetUser = peers[data.target]
                if (targetUser) {
                    targetUser.send(JSON.stringify({
                        Candidate: data.candidate,
                        mid: data.mid
                    }))
                } else {
                    console.log(`Target ${data.target} not found`);
                }
                break
        }
    })

    connection.on('close', () => {
        var found = false
        for (let user in peers) {
            if (peers[user] === connection) {
                console.log(`User ${user} disconnected`);
                delete peers[user];
                found = true
                break;
            }
        }
        if (!found) {
            console.log(`The closed connection wasn't registered`);
        }
    })
})