// C++
// String Methods

#include "string_methods.h"

void Strip(std::string &str)
{
	LStrip(str);
	RStrip(str);
}

void LStrip(std::string &str)
{
	size_t length = str.size();
	size_t shift = 0;
	if (str[0] == ' ')
	{
		while (shift < length && str[shift] == ' ')
		{
			shift++;
		}
		str = str.substr(shift, length - shift);
	}
}

void RStrip(std::string &str)
{
	size_t shift, length = str.size();
	if (length > 0)
	{
		if (str[shift = length - 1] == ' ')
		{
			while (shift > 0 && str[shift] == ' ')
			{
				shift--;
			}
			str = str.substr(0, shift + 1);
		}
	}
}

void CutCommentLine(std::string &str)
{
	const char comment = '/';
	size_t length = str.size();
	if (length < 2)
	{
		str = "";
		return;
	}
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == comment && i != length - 1 && str[i + 1] == comment)
		{
			str = str.substr(0, i);
			return;
		}
	}
}

void SplitString(std::vector<std::string> &dest, const std::string str, const std::string sep)
{
	std::string buffer;
	size_t length = str.size();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] != sep[0])
		{
			buffer += str[i];
			if (i + 1 != length)
				continue;
		}
		if (buffer.size() > 0)
		{
			dest.push_back(buffer);
			buffer.clear();
		}
	}
}

bool StringBeginsWithPrefix(const std::string str, const std::string prefix)
{
	if (str.size() < prefix.size())
		return false;

	for (size_t i = 0; i < prefix.size(); i++)
	{
		if (str[i] != prefix[i])
		{
			return false;
		}
	}

	return true;
}