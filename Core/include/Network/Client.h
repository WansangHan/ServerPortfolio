#pragma once

namespace taylor
{
	
class Client
{
public:
	Client(boost::asio::io_context& context);
	~Client();
public:
	void Connect(const boost::asio::ip::tcp::endpoint& endpoint);

	void Write(const char* buf, uint16_t size);
private:
	struct ClientImpl;
	std::unique_ptr<ClientImpl> pimpl_;
};

}