#pragma once

namespace taylor
{

// �Ľ� ��ġ
enum class EParsePart
{
	// ��� �Ľ���
	HEADER,
	// �ٵ� �Ľ���
	BODY,
};

// �Ľ� ����
enum class EParseState
{
	// ����
	FAIL,
	// �д� ��
	READING,
	// �Ϸ�
	COMPLETE,
};

struct Header
{
	uint16_t size;
	uint16_t code;
};

class Parser
{
public:
	Parser();
	~Parser();
public:
	bool ParseAll(char * data, const uint16_t bytesTransferred, std::function<void(char* const, char* const)> successCallback);
private:
	EParseState Parse(const char * const data, uint16_t& remain);
private:
	struct ParserImpl;
	std::unique_ptr<ParserImpl> pimpl_;
};

}