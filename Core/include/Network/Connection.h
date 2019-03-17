#pragma once

namespace taylor
{

class Connection
{
public:
	Connection(boost::asio::ip::tcp::socket&& socket);
	~Connection();
public:
	void Read();
	void Write(const char* buf, uint16_t size);
private:
	void HandleReadData(const uint16_t size);
private:
	struct ConnectionImpl;
	std::unique_ptr<ConnectionImpl> pimpl_;
};

}