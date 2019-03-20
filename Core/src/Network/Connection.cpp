#include "stdafx.h"
#include "Network/Connection.h"

#include "Network/Parser.h"

namespace taylor
{

struct Connection::ConnectionImpl
{
	// Send Recv�� ����
	boost::asio::ip::tcp::socket socket_;
	// ����
	std::array<char, 128> receiveBuffer_;

	Parser parser_;

	ConnectionImpl(boost::asio::ip::tcp::socket&& socket) : socket_(std::move(socket)) {}
};

Connection::Connection(boost::asio::ip::tcp::socket&& socket) :
	pimpl_(std::make_unique<Connection::ConnectionImpl>(std::move(socket)))
{
}

Connection::~Connection() = default;

void Connection::Read()
{
	pimpl_->socket_.async_read_some(boost::asio::buffer(pimpl_->receiveBuffer_),
		[this](const boost::system::error_code& error, const size_t bytesTransferred) {
		if (error)
		{
			if (error == boost::asio::error::eof)
			{
				// ������ ������.
				spdlog::info("Disconnect.");
				pimpl_->socket_.close();
				return;
			}

			// �� �� ���� ����
			spdlog::error("Error No : {}, Error Message : {}", error.value(), error.message());
			return;
		}

		// ���� ������ ó��
		HandleReadData(static_cast<uint16_t>(bytesTransferred));
	});
}

void Connection::Write(const char * buf, uint16_t size)
{
	boost::asio::async_write(pimpl_->socket_, boost::asio::buffer(buf, size),
		[](const boost::system::error_code& error, const size_t bytesTransferred) {
		if (error)
		{
			spdlog::error("Write Failed : {}", error.message());
			return;
		}

		BOOST_ASSERT(bytesTransferred);
	});
}

void Connection::HandleReadData(const uint16_t size)
{
	// ���� �����͸� ������ �迭
	char* const data = new char[size];
	// ���� �����͸� ī���Ѵ�.
	memcpy(data, pimpl_->receiveBuffer_.data(), size);

	// ��Ŷ�� �Ľ��Ѵ�.
	pimpl_->parser_.ParseAll(data, size, [](char* const header, char* const body) {
		// �Ľ̿� ������ ��Ŷ�� ���� ó���� �Ѵ�.
		BOOST_ASSERT(header);
		BOOST_ASSERT(body);
	});

	delete[] data;
}

}