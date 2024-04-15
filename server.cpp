#include <iostream>
#include <vector>
#include <memory> // Include for std::shared_ptr
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void startAccept(tcp::acceptor &acceptor, std::vector<std::shared_ptr<tcp::socket>> &clientSockets)
{
    std::cout << "Waiting for client to connect...\n";
    auto newSocket = std::make_shared<tcp::socket>(acceptor.get_executor());

    acceptor.async_accept(*newSocket, [&](const boost::system::error_code &error)
                          {
        if (!error) {
            std::cout << "Client connected: " << newSocket->remote_endpoint() << std::endl;
            clientSockets.push_back(std::move(newSocket));
        } else {
            std::cerr << "Error accepting connection: " << error.message() << std::endl;
        }

        startAccept(acceptor, clientSockets); });
}

int main()
{
    try
    {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 50000));
        std::vector<std::shared_ptr<tcp::socket>> clientSockets;

        startAccept(acceptor, clientSockets);
        io.run();

        while(1)
        {
            std::cout<<"clientSockets.size() = "<<clientSockets.size()<<"\n";
            while (clientSockets.size() > 0)
            {
                std::string message = "Hello, client!\n";
                for (auto &socket : clientSockets)
                {
                    boost::asio::write(*socket, boost::asio::buffer(message));
                    std::cout << "Sent message to client: " << socket->remote_endpoint() << std::endl;

                    boost::asio::streambuf buffer;
                    boost::asio::read_until(*socket, buffer, '\n');
                    std::string receivedMessage(boost::asio::buffer_cast<const char *>(buffer.data()), buffer.size());
                    std::cout << "Received message from client: " << receivedMessage << std::endl;
                }
                sleep(1);
            }
            sleep(1);
        }
        
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
