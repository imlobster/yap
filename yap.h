#pragma once
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <cstddef>

#define YAP_PREFIX "yap: "

#define YAP_begin { ::yap::incidents_t YAPINT_incidents_;

#define YAP_CHECK(...) do { if(!(__VA_ARGS__)) { \
	YAPINT_incidents_.emplace_back("untrue at '" #__VA_ARGS__ "'"); \
} } while(0)

#define YAP_CHECK_MSG(msg, ...) do { if(!(__VA_ARGS__)) { \
	YAPINT_incidents_.emplace_back(msg " at '" #__VA_ARGS__ "'"); \
} } while(0)

#define YAP_REQUIRE(...) do { if(!(__VA_ARGS__)) { \
	YAPINT_incidents_.emplace_back("failed at '" #__VA_ARGS__ "'"); \
	return YAPINT_incidents_; \
} } while(0)

#define YAP_REQUIRE_MSG(msg, ...) do { if(!(__VA_ARGS__)) { \
	YAPINT_incidents_.emplace_back(msg " at '" #__VA_ARGS__ "'"); \
	return YAPINT_incidents_; \
} } while(0)

#define YAP_done return YAPINT_incidents_; }

namespace yap {

using incidents_t = std::vector<std::string>;

struct TestResult {
	std::string name;
	incidents_t incidents;
};

using test_t = std::function<incidents_t()>;

class Yap {
	std::vector<std::pair<std::string, test_t>> tests;
	std::unordered_map<std::string, std::size_t> index;

public:
	incidents_t one(const std::string &iname) const {
		auto i = index.find(iname);

		if(i == index.end())
			throw std::runtime_error(YAP_PREFIX "Test not found!");

		return tests.at(i->second).second();
	}

	std::vector<TestResult> run() const {
		std::vector<TestResult> result;
		result.reserve(tests.size());

		for(const auto &t : tests) {
			result.push_back({t.first, t.second()});
		}

		return result;
	}

	void operator() (const std::string &iname, test_t ifn) {
		if(index.find(iname) != index.end())
			throw std::runtime_error(YAP_PREFIX "Cannot override test!");

		tests.emplace_back(iname, std::move(ifn));
		index.emplace(iname, tests.size() - 1);
	}

	void clear() { tests.clear(); index.clear(); }
};

}
