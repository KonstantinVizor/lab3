#include "../../inc/auth/JwtAuth.h"
#include "../../inc/auth/JwksClient.h"

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

#include <openssl/evp.h>

#include <chrono>
#include <vector>
#include <stdexcept>

namespace
{
	std::string base64UrlDecode(const std::string &data)
	{
		std::string input = data;
		for (char &c : input)
		{
			if (c == '-') c = '+';
			if (c == '_') c = '/';
		}
		while (input.size() % 4 != 0)
			input += '=';

		std::vector<unsigned char> decoded(input.size());
		int len = EVP_DecodeBlock(decoded.data(),
					reinterpret_cast<const unsigned char *>(input.data()),
					static_cast<int>(input.size()));
		if (len < 0)
			throw std::runtime_error("invalid base64url in jwt");

		int padding = 0;
		if (input.size() >= 2 && input[input.size() - 1] == '=') padding++;
		if (input.size() >= 2 && input[input.size() - 2] == '=') padding++;
		len -= padding;

		return std::string(reinterpret_cast<char *>(decoded.data()), len);
	}

	long long now()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	}

	bool verifySignature(const std::string &message, const std::string &signature)
	{
		EVP_PKEY *key = JwksClient::getKey();
		if (!key)
			return false;

		EVP_MD_CTX *ctx = EVP_MD_CTX_new();
		bool ok = false;

		if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, key) == 1)
		{
			EVP_DigestVerifyUpdate(ctx, message.data(), message.size());
			ok = EVP_DigestVerifyFinal(ctx,
				reinterpret_cast<const unsigned char *>(signature.data()),
				signature.size()) == 1;
		}

		EVP_MD_CTX_free(ctx);
		return ok;
	}
}

AuthClaims JwtAuth::validate(const std::string &authorizationHeader)
{
	AuthClaims result;

	if (authorizationHeader.rfind("Bearer ", 0) != 0)
		return result;

	std::string token = authorizationHeader.substr(7);

	size_t p1 = token.find('.');
	size_t p2 = (p1 == std::string::npos) ? std::string::npos : token.find('.', p1 + 1);

	if (p1 == std::string::npos || p2 == std::string::npos)
		return result;

	try
	{
		std::string message = token.substr(0, p2);
		std::string signature = base64UrlDecode(token.substr(p2 + 1));

		if (!verifySignature(message, signature))
			return result;

		std::string payload = base64UrlDecode(token.substr(p1 + 1, p2 - p1 - 1));

		Poco::JSON::Parser parser;
		auto parsed = parser.parse(payload);
		auto object = parsed.extract<Poco::JSON::Object::Ptr>();

		if (!object->has("exp"))
			return result;

		long long exp = object->getValue<long long>("exp");
		if (now() >= exp)
			return result;

		result.username = object->optValue<std::string>("sub", "");
		result.role = object->optValue<std::string>("role", "");
		result.token = token;
		result.valid = !result.username.empty();
	}
	catch (...)
	{
		result.valid = false;
	}

	return result;
}
