#include "stdafx.h"
#include "Network/Parser.h"

namespace taylor
{

const uint16_t HEADER_SIZE = sizeof(Header);

struct Parser::ParserImpl
{
	// ���
	char* header_{ nullptr };
	// �ٵ�
	char* body_{ nullptr };
	// �ٵ� ũ��
	uint16_t bodySize_{ 0 };
	// Offset
	uint16_t offset_{ 0 };
	// �Ľ� ��ġ
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

		// data�� offset�� �̵��Ѵ�.
		data += oldRemain - remain;
		oldRemain = remain;

		// �Ľ̿� �����ߴٸ�
		if (state == EParseState::FAIL)
		{
			BOOST_ASSERT(0);
			return false;
		}

		// �Ľ��� ���� ��Ŷ�� �ִٸ�
		if (state == EParseState::COMPLETE)
		{
			successCallback(pimpl_->header_, pimpl_->body_);
		}
		
		// �Ľ��� �����ٸ�
		if (remain <= 0)
			break;
	}

	return true;
}

EParseState Parser::Parse(const char * const data, uint16_t& remain)
{
	// ����� �Ľ��� ���ʶ��
	if (pimpl_->part_ == EParsePart::HEADER)
	{
		// ����� ũ�⺸�� ��Ŷ�� ũ�Ⱑ �� ���� ���
		if (remain + pimpl_->offset_ < HEADER_SIZE)
		{
			memcpy(&pimpl_->header_[pimpl_->offset_], data, remain);
			pimpl_->offset_ += remain;
			remain = 0;
			return EParseState::READING;
		}

		// ��� �� ����
		memcpy(&pimpl_->header_[pimpl_->offset_], data, HEADER_SIZE - pimpl_->offset_);

		// ���� ũ�� ����
		remain -= HEADER_SIZE - pimpl_->offset_;
		// �ٵ� ���� �����̱� ������ offset�� 0���� ������ش�.
		pimpl_->offset_ = 0;

		// �Ľ� ��ġ�� BODY�� �ٲ۴�.
		pimpl_->part_ = EParsePart::BODY;

		memcpy(&pimpl_->bodySize_, pimpl_->header_, sizeof(uint16_t));
		pimpl_->bodySize_ -= HEADER_SIZE;
		// �̹� ���� ��Ŷ�� Body�� �־��ٸ� ���ְ� ���� �����.
		if (pimpl_->body_ != nullptr)
		{
			delete[] pimpl_->body_;
			pimpl_->body_ = nullptr;
		}
		pimpl_->body_ = new char[pimpl_->bodySize_];

		return EParseState::READING;
	}
	// �ٵ� �Ľ��� ���ʶ��
	else if (pimpl_->part_ == EParsePart::BODY)
	{
		// BodySize�� 0�� �� ����.
		BOOST_ASSERT(pimpl_->bodySize_);
		// �ٵ��� ũ�⺸�� ��Ŷ�� ũ�Ⱑ �� ���� ���
		if (remain + pimpl_->offset_ < pimpl_->bodySize_)
		{
			// ���� ��Ŷ�� ũ�⸦ �����Ѵ�.
			memcpy(&pimpl_->body_[pimpl_->offset_], data, remain);
			pimpl_->offset_ += remain;
			remain = 0;
			return EParseState::READING;
		}
		
		// �ٵ� ������ �����Ѵ�.
		memcpy(&pimpl_->body_[pimpl_->offset_], data, pimpl_->bodySize_ - pimpl_->offset_);

		remain -= pimpl_->bodySize_ - pimpl_->offset_;
		// ����� ���� �����̱� ������ offset�� 0���� ������ش�.
		pimpl_->offset_ = 0;
		
		// �Ľ� ���¸� HEADER�� �ٲ۴�.
		pimpl_->part_ = EParsePart::HEADER;

		return EParseState::COMPLETE;
	}

	BOOST_ASSERT(0);
	return EParseState::FAIL;
}

}