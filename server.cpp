#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;

void sendMessageToClient(std::shared_ptr<tcp::socket> socket, std::vector<std::shared_ptr<tcp::socket>>& clientSockets, boost::asio::io_context& io) {
    std::string message = "Hello, client!\n";
    boost::asio::async_write(*socket, boost::asio::buffer(message),
        [socket, &clientSockets, &io](const boost::system::error_code& error, std::size_t /*bytesWritten*/) {
            if (error) {
                std::cerr << "Error writing to client: " << error.message() << std::endl;

                // Remove the disconnected client's socket from the vector
                auto it = std::find(clientSockets.begin(), clientSockets.end(), socket);
                if (it != clientSockets.end()) {
                    clientSockets.erase(it);
                }
                return;
            }

            // Schedule the next message to be sent after a short delay
            io.post(boost::bind(sendMessageToClient, socket, std::ref(clientSockets), std::ref(io)));
        });
}

void handleClientRead(std::shared_ptr<tcp::socket> socket, std::vector<std::shared_ptr<tcp::socket>>& clientSockets, boost::asio::io_context& io) {
    auto bufferPtr = std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(*socket, *bufferPtr, '\n',
        [socket, &clientSockets, &io, bufferPtr](const boost::system::error_code& error, std::size_t bytesRead) {
            if (error) {
                std::cerr << "Error reading from client: " << error.message() << std::endl;

                // Remove the disconnected client's socket from the vector
                auto it = std::find(clientSockets.begin(), clientSockets.end(), socket);
                if (it != clientSockets.end()) {
                    clientSockets.erase(it);
                }
                return;
            }

            std::string receivedMessage(boost::asio::buffer_cast<const char*>(bufferPtr->data()), bytesRead - 1);
            std::cout << "Received message from client: " << receivedMessage << std::endl;

            bufferPtr->consume(bytesRead); // Consume the data from the buffer

            handleClientRead(socket, clientSockets, io); // Start reading from the client again
        });
}

void startAccept(tcp::acceptor& acceptor, std::vector<std::shared_ptr<tcp::socket>>& clientSockets, boost::asio::io_context& io) {
    auto newSocket = std::make_shared<tcp::socket>(acceptor.get_executor());
    clientSockets.push_back(newSocket);
    acceptor.async_accept(*newSocket, [&, newSocket](const boost::system::error_code& error) {
        if (!error) {
            std::cout << "Client connected: " << newSocket->remote_endpoint() << std::endl;
            // clientSockets.push_back(std::move(newSocket));
            handleClientRead(newSocket, clientSockets, io); // Start reading from the new client
            sendMessageToClient(newSocket, clientSockets, io); // Start sending messages to the new client
            startAccept(acceptor, clientSockets, io); // Continue accepting new connections
        } else {
            clientSockets.pop_back();
            std::cerr << "Error accepting connection: " << error.message() << std::endl;
        }
    });
}

int main() {
    try {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 50000));
        std::vector<std::shared_ptr<tcp::socket>> clientSockets;

        startAccept(acceptor, clientSockets, io);

        while (true) {
            io.run();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}