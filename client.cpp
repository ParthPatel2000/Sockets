#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

using boost::asio::ip::tcp;

int main() {

    std::string name;
    std::cout << "Enter your name: ";
    std::cin >> name;
    try {
        boost::asio::io_context io;
        tcp::socket socket(io);
        
        // Retry connecting to the server until successful
        while (true) {
            try {
                socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 50000));
                std::cout << "Connected to server" << std::endl;
                break; // Exit the retry loop if connection successful
            } catch (const std::exception& e) {
                std::cerr << "Error connecting to server: " << e.what() << std::endl;
                std::cout << "Retrying in 1 second..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second before retrying
            }
        }

        while (true) {
            // Read message from the server
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');
            std::string message(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());
            std::cout << "Received message from server: " << message << std::endl;

            if (message == "Hello, client!\n") {
                std::string response = "Hello, server I'm !"+name+"\n";
                boost::asio::write(socket, boost::asio::buffer(response));
                std::cout << "Sent message to server: " << response << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
