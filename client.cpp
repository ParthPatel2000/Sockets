#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    tcp::resolver resolver(io_service);

    try {
        boost::asio::connect(socket, resolver.resolve({"localhost", "50000"}));

        // Send data or perform other operations with the connected server

        // Check if the connection is still open
        while (socket.is_open()) {
            // Perform some I/O operation, like reading or writing
            // For example, try reading some data from the socket
            char data[1024];
            size_t length = socket.read_some(boost::asio::buffer(data, sizeof(data)));

            // Handle the received data or perform other actions
            std::cout << "Received data from server: " << std::string(data, length) << std::endl;
        }

        // The connection has been closed by the remote endpoint
        std::cout << "Connection closed by server" << std::endl;
    } catch (const boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof) {
            // Connection closed by the remote endpoint
            std::cout << "Connection closed by server" << std::endl;
        } else {
            // Other error occurred
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
