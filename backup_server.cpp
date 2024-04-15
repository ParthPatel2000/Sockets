#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
int main()
{
    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 50000));
    std::vector<tcp::socket> clientSockets;

    // async accept
    //  acceptor.async_accept(clientSocket, [&](const boost::system::error_code &error) {

    while (clientSockets.size() < 2)
    {
        // Accept a new connection
        tcp::socket clientSocket(io);
        acceptor.accept(clientSocket);

        std::cout << "Client connected: " << clientSocket.remote_endpoint() << std::endl;

        // Add the new client socket to the vector
        clientSockets.push_back(std::move(clientSocket));

        // Start sending messages to the connected clients
        // std::string message = "Hello, client!\n";
        // for (auto &socket : clientSockets)
        // {
        //     boost::asio::write(socket, boost::asio::buffer(message));
        //     std::cout << "Sent message to client: " << message;
        // }
    }
    while (1)
    {

        std::string message = "Hello, client!\n";
        for (auto &socket : clientSockets)
        {
            boost::asio::write(socket, boost::asio::buffer(message));
            std::cout << "Sent message to client: " << socket.remote_endpoint() << std::endl;

            // Read message from the client
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');
            std::string message(boost::asio::buffer_cast<const char *>(buffer.data()), buffer.size());
            std::cout << "Received message from client: " << message << std::endl;
        }
        sleep(1);
    }

    return 0;
}
