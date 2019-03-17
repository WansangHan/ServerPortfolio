#include "stdafx.h"
#include "Network/Server.h"

#include "Network/Connection.h"

namespace taylor
{

struct Server::ServerImpl
{
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::vector<std::shared_ptr<Connection>> connections;

	ServerImpl(boost::asio::io_context& context) :
		socket_(context),
		acceptor_(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8888))
	{
	}
};

Server::Server(boost::asio::io_context& context) :
	pimpl_(std::make_unique<Server::ServerImpl>(context))
{
}

Server::~Server() = default;

void Server::Accept()
{
	pimpl_->acceptor_.async_accept(pimpl_->socket_,
		[this](const boost::system::error_code& error) {
		if (error)
		{
			spdlog::error("Accept Failed : {}", error.message());
			Accept();
			return;
		}

		spdlog::info("Accept");

		auto connection = std::make_shared<Connection>(std::move(pimpl_->socket_));
		pimpl_->connections.emplace_back(connection);
		connection->Read();

		Accept();
	});
}

}