#include "stdafx.h"

#include "Network/Parser.h"

#define BOOST_TEST_MODULE parser_test

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(parser_test)

BOOST_AUTO_TEST_CASE(test1)
{
	using namespace taylor;

	// 정상적인 패킷 1개 테스트
	{
		Parser parser;

		const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04";

		uint8_t success = 0;
		bool result = parser.ParseAll(const_cast<char*>(packet), 8, [&success](char* const header, char* const body){
			BOOST_CHECK(header);
			BOOST_CHECK(body);

			Header headerVal;
			memcpy(&headerVal, header, sizeof(Header));
			BOOST_CHECK(headerVal.size == 0x0008);
			BOOST_CHECK(headerVal.code == 0x0100);

			uint32_t bodyVal;
			memcpy(&bodyVal, body, sizeof(uint32_t));
			BOOST_CHECK(bodyVal == 0x04030201);

			success++;
		});

		BOOST_CHECK(success == 1);
		BOOST_CHECK(result);
	}

	// 정상적인 패킷 2개 테스트
	{
		Parser parser;

		const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04\x06\x00\x00\x01\x01\x02";

		uint8_t success = 0;
		bool result = parser.ParseAll(const_cast<char*>(packet), 14, [&success](char* const header, char* const body) {
			
			BOOST_CHECK(header);
			BOOST_CHECK(body);

			Header headerVal;
			memcpy(&headerVal, header, sizeof(Header));

			// 첫 번째 패킷
			if (success == 0)
			{
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);
			}
			
			// 두 번째 패킷
			if (success == 1)
			{
				BOOST_CHECK(headerVal.size == 0x0006);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint16_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint16_t));
				BOOST_CHECK(bodyVal == 0x0201);
			}

			success++;
		});

		BOOST_CHECK(success == 2);
		BOOST_CHECK(result);
	}

	// 정상적인 패킷 1개 + 정상적인 패킷을 헤더와 바디 경계부분에서 자른 후 테스트
	{
		Parser parser;

		// 정상적인 패킷 1개 + 헤더만 부분적으로 자른 패킷 한개
		{
			const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04\x06\x00\x00\x01";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 12, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}

		// 바디 패킷 한개
		{
			const char* packet = "\x01\x02";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 2, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0006);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint16_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint16_t));
				BOOST_CHECK(bodyVal == 0x0201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}
	}

	// 정상적인 패킷 1개 + 정상적인 패킷을 헤더 부분에서 자른 후 테스트
	{
		Parser parser;

		// 정상적인 패킷 1개 + 헤더만 부분적으로 자른 패킷 한개
		{
			const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04\x06";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 9, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}
		
		// 헤더가 잘린 패킷 한개
		{
			const char* packet = "\x00\x00\x01\x01\x02";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 5, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0006);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint16_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint16_t));
				BOOST_CHECK(bodyVal == 0x0201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}
	}

	// 정상적인 패킷 1개 + 정상적인 패킷을 바디 부분에서 자른 후 테스트
	{
		Parser parser;

		// 정상적인 패킷 1개 + 헤더만 부분적으로 자른 패킷 한개
		{
			const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04\x06\x00\x00\x01\x01";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 13, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}

		// 바디가 잘린 패킷 한개
		{
			const char* packet = "\x02";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0006);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint16_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint16_t));
				BOOST_CHECK(bodyVal == 0x0201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}
	}

	// 패킷 1개 1바이트씩 잘라서 // \x08\x00\x00\x01\x01\x02\x03\x04
	{
		Parser parser;

		// 자른 패킷 한개
		{
			const char* packet = "\x08";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x00";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x00";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x01";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x01";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x02";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x03";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);
				success++;
			});

			BOOST_CHECK(success == 0);
			BOOST_CHECK(result);
		}

		// 자른 패킷 한개
		{
			const char* packet = "\x04";

			uint8_t success = 0;
			bool result = parser.ParseAll(const_cast<char*>(packet), 1, [&success](char* const header, char* const body) {
				BOOST_CHECK(header);
				BOOST_CHECK(body);

				Header headerVal;
				memcpy(&headerVal, header, sizeof(Header));
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);

				success++;
			});

			BOOST_CHECK(success == 1);
			BOOST_CHECK(result);
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()