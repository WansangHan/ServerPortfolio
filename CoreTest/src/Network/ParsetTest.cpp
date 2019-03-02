#include "stdafx.h"

#include "Network/Parser.h"

#define BOOST_TEST_MODULE parser_test

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(parser_test)

BOOST_AUTO_TEST_CASE(test1)
{
	using namespace taylor;

	// �������� ��Ŷ 1�� �׽�Ʈ
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

	// �������� ��Ŷ 2�� �׽�Ʈ
	{
		Parser parser;

		const char* packet = "\x08\x00\x00\x01\x01\x02\x03\x04\x06\x00\x00\x01\x01\x02";

		uint8_t success = 0;
		bool result = parser.ParseAll(const_cast<char*>(packet), 14, [&success](char* const header, char* const body) {
			
			BOOST_CHECK(header);
			BOOST_CHECK(body);

			Header headerVal;
			memcpy(&headerVal, header, sizeof(Header));

			// ù ��° ��Ŷ
			if (success == 0)
			{
				BOOST_CHECK(headerVal.size == 0x0008);
				BOOST_CHECK(headerVal.code == 0x0100);

				uint32_t bodyVal;
				memcpy(&bodyVal, body, sizeof(uint32_t));
				BOOST_CHECK(bodyVal == 0x04030201);
			}
			
			// �� ��° ��Ŷ
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

	// �������� ��Ŷ 1�� + �������� ��Ŷ�� ����� �ٵ� ���κп��� �ڸ� �� �׽�Ʈ
	{
		Parser parser;

		// �������� ��Ŷ 1�� + ����� �κ������� �ڸ� ��Ŷ �Ѱ�
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

		// �ٵ� ��Ŷ �Ѱ�
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

	// �������� ��Ŷ 1�� + �������� ��Ŷ�� ��� �κп��� �ڸ� �� �׽�Ʈ
	{
		Parser parser;

		// �������� ��Ŷ 1�� + ����� �κ������� �ڸ� ��Ŷ �Ѱ�
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
		
		// ����� �߸� ��Ŷ �Ѱ�
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

	// �������� ��Ŷ 1�� + �������� ��Ŷ�� �ٵ� �κп��� �ڸ� �� �׽�Ʈ
	{
		Parser parser;

		// �������� ��Ŷ 1�� + ����� �κ������� �ڸ� ��Ŷ �Ѱ�
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

		// �ٵ� �߸� ��Ŷ �Ѱ�
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

	// ��Ŷ 1�� 1����Ʈ�� �߶� // \x08\x00\x00\x01\x01\x02\x03\x04
	{
		Parser parser;

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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

		// �ڸ� ��Ŷ �Ѱ�
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