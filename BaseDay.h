#pragma once

#include <algorithm>
#include <regex>
#include <string>
#include <string_view>
#include <functional>
#include <fstream>
#include <filesystem>
#include <limits>

#ifdef __GNUC__
#include <csignal>
#define __debugbreak() std::raise(SIGINT)
#endif

namespace std
{
	using wsvmatch = match_results<wstring_view::const_iterator>;

	inline int stoi(const std::wstring_view& view, size_t* idx = nullptr, int base = 10)
	{
		if (idx)
		{
			*idx = 0;
		}

		if (base != 0 && (base < 2 || base > 36))
		{
			errno = EINVAL;
			return 0;
		}

		bool bEncounteredSign = false;
		bool bIsNegative = false;
		int result = 0;
		auto it = view.begin();
		for (; it != view.end(); ++it)
		{
			if (!isspace(*it))
			{
				break;
			}
		}

		if (*it == L'+')
		{
			++it;
			bEncounteredSign = true;
		}
		if (*it == L'-')
		{
			if (!bEncounteredSign)
			{
				bEncounteredSign = true;
				++it;
				bIsNegative = true;
			}
		}

		if (base == 16 || base == 0)
		{
			if (*it == L'0' && (*std::next(it) == 'x' || *std::next(it) == 'X'))
			{
				std::advance(it, 2);
				base = 16;
			}
		}
		if (base == 0)
		{
			if (*it == L'0')
			{
				++it;
				base = 8;
			}
		}
		if (base == 0)
		{
			base = 10;
		}

		for (; it != view.end(); ++it)
		{
			int adder = 0;
			if (*it >= L'0' && *it <= L'9')
			{
				adder = *it - L'0';
			}
			else if (*it >= L'A' && *it <= L'Z')
			{
				adder = 10 + (*it - L'A');
			}
			else if (*it >= L'a' && *it <= L'z')
			{
				adder = 10 + (*it - L'a');
			}
			else
			{
				break;
			}

			if (bIsNegative)
			{
				if (std::numeric_limits<int>::min() / base > result)
				{
					errno = ERANGE;
					result = std::numeric_limits<int>::min();
					break;
				}

				result *= base;

				if (std::numeric_limits<int>::min() + adder > result)
				{
					errno = ERANGE;
					result = std::numeric_limits<int>::min();
					break;
				}

				result -= adder;
			}
			else
			{
				if (std::numeric_limits<int>::max() / base < result)
				{
					errno = ERANGE;
					result = std::numeric_limits<int>::max();
					break;
				}

				result *= base;

				if (std::numeric_limits<int>::max() - adder < result)
				{
					errno = ERANGE;
					result = std::numeric_limits<int>::max();
					break;
				}

				result += adder;
			}
		}

		if (idx)
		{
			*idx = std::distance(view.begin(), it);
		}

		return result;
	}
}

class BaseDay
{
public:
	virtual std::wstring GetResultStr() = 0;
	virtual std::wstring GetIdStr() = 0;
	std::filesystem::path GetWorkingFolder();
	std::filesystem::path GetInputPath();
	std::filesystem::path GetTestInputPath(int numSuffix = -1);
	//void ForEachLineInFile(std::wifstream& file, const std::function<void(const std::wstring&)>& func);
	//void ForEachLineInFile(const std::filesystem::path& path, const std::function<void(const std::wstring&)>& func);
	//void ForEachLineInInputFile(const std::function<void(const std::wstring&)>& func);
	//void ForEachLineInTestInputFile(const std::function<void(const std::wstring&)>& func, int numSuffix = -1);
	void ForEachLineInFile(std::wifstream& file, const std::function<void(const std::wstring_view&)>& func);
	void ForEachLineInFile(const std::filesystem::path& path, const std::function<void(const std::wstring_view&)>& func);
	void ForEachLineInInputFile(const std::function<void(const std::wstring_view&)>& func);
	void ForEachLineInTestInputFile(const std::function<void(const std::wstring_view&)>& func, int numSuffix = -1);
};