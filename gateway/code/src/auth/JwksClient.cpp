#include "../../inc/auth/JwksClient.h"

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

#include <openssl/bn.h>
#include <openssl/rsa.h>

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <vector>

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
			throw std::runtime_error("invalid base64url in jwks");

		int padding = 0;
		if (input.size() >= 2 && input[input.size() - 1] == '=') padding++;
		if (input.size() >= 2 && input[input.size() - 2] == '=') padding++;
		len -= padding;

		return std::string(reinterpret_cast<char *>(decoded.data()), len);
	}

	std::string identityProviderUrl()
	{
		const char *env = std::getenv("IDENTITY_PROVIDER_URL");
		return env ? std::string(env) : std::string("http://identity-provider:8080");
	}

	std::string fetchJwks()
	{
		Poco::URI uri(identityProviderUrl() + "/.well-known/jwks.json");
		Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
		session.setTimeout(Poco::Timespan(5, 0));

		Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery());
		session.sendRequest(req);

		Poco::Net::HTTPResponse resp;
		std::istream &rs = session.receiveResponse(resp);

		if (resp.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
			throw std::runtime_error("jwks endpoint returned non-200");

		std::ostringstream body;
		body << rs.rdbuf();
		return body.str();
	}

	EVP_PKEY *buildKey()
	{
		std::string lastError;

		for (int attempt = 0; attempt < 30; attempt++)
		{
			try
			{
				std::string json = fetchJwks();

				Poco::JSON::Parser parser;
				auto result = parser.parse(json);
				auto object = result.extract<Poco::JSON::Object::Ptr>();
				auto keys = object->getArray("keys");

				if (!keys || keys->size() == 0)
					throw std::runtime_error("jwks has no keys");

				auto key = keys->getObject(0);
				std::string n = base64UrlDecode(key->getValue<std::string>("n"));
				std::string e = base64UrlDecode(key->getValue<std::string>("e"));

				BIGNUM *bnN = BN_bin2bn(reinterpret_cast<const unsigned char *>(n.data()), static_cast<int>(n.size()), nullptr);
				BIGNUM *bnE = BN_bin2bn(reinterpret_cast<const unsigned char *>(e.data()), static_cast<int>(e.size()), nullptr);

				RSA *rsa = RSA_new();
				RSA_set0_key(rsa, bnN, bnE, nullptr);

				EVP_PKEY *pkey = EVP_PKEY_new();
				EVP_PKEY_assign_RSA(pkey, rsa);

				std::cout << "JwksClient: fetched and cached Identity Provider public key" << std::endl;
				return pkey;
			}
			catch (const std::exception &e)
			{
				lastError = e.what();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		throw std::runtime_error("could not fetch JWKS from Identity Provider: " + lastError);
	}
}

EVP_PKEY *JwksClient::getKey()
{
	static EVP_PKEY *cached = nullptr;
	static std::once_flag initialized;

	std::call_once(initialized, []() { cached = buildKey(); });

	return cached;
}
