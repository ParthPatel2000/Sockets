#include <iostream>
#include <boost/asio.hpp> // Include the Boost.Asio library
#include <vector>
#include <memory>

using boost::asio::ip::tcp; // Define the IP version to use
using std::string;
using std::vector;

int main()
{
    // vector<std::shared_ptr<tcp::socket>> clients; // Create a vector to store the clients
    vector<tcp::socket *> clients; // Create a vector to store the clients

    boost::asio::io_service io;                                  // Create an I/O service
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 50000)); // Create an acceptor to listen for new connections

    while (clients.size() < 2)
    {
        tcp::socket *socket = new tcp::socket(io);
        acceptor.accept(*socket);

        std::cout << "Client connected: " << socket->remote_endpoint().address() << std::endl;

        // Create a new client object and store it
        // auto client = std::make_shared<tcp::socket>(std::move(socket));
        // clients.push_back(client);

        // auto client = std::make_shared<tcp::socket>(std::move(socket));
        clients.push_back(socket);
        std::cout << "Error before here " << std::endl;

        // Handle client connection here

        // No need to close the socket here
        // The client object will manage its own socket lifecycle

        // std::cout<<"socket is "<<(socket->is_open()? "open" : "closed")<<std::endl;
    }

    while(true)
    {
        for(auto client : clients)
        {
            std::cout<<"socket is "<<(client->is_open()? "open" : "closed")<<std::endl;
            sleep(1);
        }
    }

    return 0;
}
