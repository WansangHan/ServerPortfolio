#include "stdafx.h"
#include "Network/Client.h"

#include "Network/Connection.h"

namespace taylor
{

struct Client::ClientImpl
{
	boost::asio::ip::tcp::socket socket_;
	std::shared_ptr<Connection> connection_;
	ClientImpl(boost::asio::io_context& context) :
		socket_(context)
	{

	}
};

Client::Client(boost::asio::io_context& context) : pimpl_(std::make_unique<Client::ClientImpl>(context))
{
}

Client::~Client() = default;

void Client::Connect(const boost::asio::ip::tcp::endpoint & endpoint)
{
	pimpl_->socket_.async_connect(endpoint, [this](const boost::system::error_code& error)
	{
		if (error)
		{
			spdlog::error("Connect Failed : {}", error.message());
			return;
		}

		spdlog::info("Connected");

		pimpl_->connection_ = std::make_shared<Connection>(std::move(pimpl_->socket_));
		pimpl_->connection_->Read();
	});
}

void Client::Write(const char * buf, uint16_t size)
{
	if (pimpl_->connection_ == nullptr)
	{
		spdlog::error("Connection is nullptr");
		return;
	}

	pimpl_->connection_->Write(buf, size);
}

}