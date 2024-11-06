const express = require("express");
const bodyParser = require("body-parser");
const WebSocket = require("ws");

const app = express();
const PORT = 3000;


// Use body-parser to handle JSON payloads
app.use(bodyParser.json());

// WebSocket server setup
const wss = new WebSocket.Server({ noServer: true });

// Handle WebSocket connections
wss.on("connection", (ws) => {
  console.log("Client connected via WebSocket");

  // Listen for messages from the client
  ws.on("message", (message) => {
    var json = JSON.parse(message);
    if (!json.type || json.type !== "register" || !json.account_id) return;
    ws.id = json.account_id;
  });

  // Send a message to the client
  ws.send(JSON.stringify({ message: "Welcome to the WebSocket server!" }));
});

// HTTP server to integrate WebSocket upgrade handling
const server = app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});

// Upgrade HTTP server to handle WebSocket protocol
server.on("upgrade", (request, socket, head) => {
  wss.handleUpgrade(request, socket, head, (ws) => {
    wss.emit("connection", ws, request);
  });
});

// Webhook endpoint to receive POST requests
app.post("/webhook", (req, res) => {
  const payload = req.body;
  console.log("Received webhook:", payload);
  if (!payload.type || !payload.data || !payload.data.accountId) {
    return;
  }
  const type = payload.type;
  if (type !== "envelope-completed") {
    return;
  }
  const accountId = payload.data.accountId;

  // Broadcast the payload to all connected WebSocket clients
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN && client.id === accountId) {
      client.send("SHAKE");
    }
  });

  res.status(200).send("Webhook received");
});
