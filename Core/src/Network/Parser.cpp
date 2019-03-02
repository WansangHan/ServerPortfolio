#include "stdafx.h"
#include "Network/Parser.h"

namespace taylor
{

const uint16_t HEADER_SIZE = sizeof(Header);

struct Parser::ParserImpl
{
	// 헤더
	char* header_{ nullptr };
	// 바디
	char* body_{ nullptr };
	// 바디 크기
	uint16_t bodySize_{ 0 };
	// Offset
	uint16_t offset_{ 0 };
	// 파싱 위치
	EParsePart part_{ EParsePart::HEADER };

	ParserImpl() : header_(new char[HEADER_SIZE]) {}
	~ParserImpl() 
	{
		delete[] header_;
		if (body_ != nullptr)
		{
			delete[] body_;
			body_ = nullptr;
		}
	}
};

Parser::Parser() : pimpl_(std::make_unique<Parser::ParserImpl>())
{
}

Parser::~Parser() = default;

bool Parser::ParseAll(char * data, const uint16_t bytesTransferred, std::function<void(char* const, char* const)> successCallback)
{
	uint16_t remain = bytesTransferred;
	uint16_t oldRemain = remain;

	while (true)
	{
		EParseState state = Parse(data, remain);

		// data의 offset을 이동한다.
		data += oldRemain - remain;
		oldRemain = remain;

		// 파싱에 실패했다면
		if (state == EParseState::FAIL)
		{
			BOOST_ASSERT(0);
			return false;
		}

		// 파싱을 끝낸 패킷이 있다면
		if (state == EParseState::COMPLETE)
		{
			successCallback(pimpl_->header_, pimpl_->body_);
		}
		
		// 파싱이 끝났다면
		if (remain <= 0)
			break;
	}

	return true;
}

EParseState Parser::Parse(const char * const data, uint16_t& remain)
{
	// 헤더를 파싱할 차례라면
	if (pimpl_->part_ == EParsePart::HEADER)
	{
		// 헤더의 크기보다 패킷의 크기가 더 작은 경우
		if (remain + pimpl_->offset_ < HEADER_SIZE)
		{
			memcpy(&pimpl_->header_[pimpl_->offset_], data, remain);
			pimpl_->offset_ += remain;
			remain = 0;
			return EParseState::READING;
		}

		// 헤더 값 대입
		memcpy(&pimpl_->header_[pimpl_->offset_], data, HEADER_SIZE - pimpl_->offset_);

		// 남은 크기 변경
		remain -= HEADER_SIZE - pimpl_->offset_;
		// 바디를 읽을 차례이기 때문에 offset을 0으로 만들어준다.
		pimpl_->offset_ = 0;

		// 파싱 위치를 BODY로 바꾼다.
		pimpl_->part_ = EParsePart::BODY;

		memcpy(&pimpl_->bodySize_, pimpl_->header_, sizeof(uint16_t));
		pimpl_->bodySize_ -= HEADER_SIZE;
		// 이미 이전 패킷의 Body가 있었다면 없애고 새로 만든다.
		if (pimpl_->body_ != nullptr)
		{
			delete[] pimpl_->body_;
			pimpl_->body_ = nullptr;
		}
		pimpl_->body_ = new char[pimpl_->bodySize_];

		return EParseState::READING;
	}
	// 바디를 파싱할 차례라면
	else if (pimpl_->part_ == EParsePart::BODY)
	{
		// BodySize가 0일 수 없다.
		BOOST_ASSERT(pimpl_->bodySize_);
		// 바디의 크기보다 패킷의 크기가 더 작은 경우
		if (remain + pimpl_->offset_ < pimpl_->bodySize_)
		{
			// 남은 패킷과 크기를 저장한다.
			memcpy(&pimpl_->body_[pimpl_->offset_], data, remain);
			pimpl_->offset_ += remain;
			remain = 0;
			return EParseState::READING;
		}
		
		// 바디 정보를 저장한다.
		memcpy(&pimpl_->body_[pimpl_->offset_], data, pimpl_->bodySize_ - pimpl_->offset_);

		remain -= pimpl_->bodySize_ - pimpl_->offset_;
		// 헤더를 읽을 차례이기 때문에 offset을 0으로 만들어준다.
		pimpl_->offset_ = 0;
		
		// 파싱 상태를 HEADER로 바꾼다.
		pimpl_->part_ = EParsePart::HEADER;

		return EParseState::COMPLETE;
	}

	BOOST_ASSERT(0);
	return EParseState::FAIL;
}

}