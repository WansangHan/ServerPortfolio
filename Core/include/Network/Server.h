#pragma once

namespace boost::asio
{

class io_context;

}

namespace taylor
{

class Server
{
public:
	Server(boost::asio::io_context& context);
	~Server();
public:
	void Accept();
private:
	struct ServerImpl;
	std::unique_ptr<ServerImpl> pimpl_;
};

}