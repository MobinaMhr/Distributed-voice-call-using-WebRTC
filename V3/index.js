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

})