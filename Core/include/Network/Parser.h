#pragma once

namespace taylor
{

// 파싱 위치
enum class EParsePart
{
	// 헤더 파싱중
	HEADER,
	// 바디 파싱중
	BODY,
};

// 파싱 상태
enum class EParseState
{
	// 실패
	FAIL,
	// 읽는 중
	READING,
	// 완료
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