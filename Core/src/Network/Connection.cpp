#include "stdafx.h"
#include "Network/Connection.h"

#include "Network/Parser.h"

namespace taylor
{

struct Connection::ConnectionImpl
{
	// Send Recv용 소켓
	boost::asio::ip::tcp::socket socket_;
	// 버퍼
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
				// 연결이 끊어짐.
				spdlog::info("Disconnect.");
				pimpl_->socket_.close();
				return;
			}

			// 알 수 없는 오류
			spdlog::error("Error No : {}, Error Message : {}", error.value(), error.message());
			return;
		}

		// 받은 데이터 처리
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
	// 받은 데이터를 저장할 배열
	char* const data = new char[size];
	// 받은 데이터를 카피한다.
	memcpy(data, pimpl_->receiveBuffer_.data(), size);

	// 패킷을 파싱한다.
	pimpl_->parser_.ParseAll(data, size, [](char* const header, char* const body) {
		// 파싱에 성공한 패킷에 대한 처리를 한다.
		BOOST_ASSERT(header);
		BOOST_ASSERT(body);
	});

	delete[] data;
}

}