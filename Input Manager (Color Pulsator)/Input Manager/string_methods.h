// C++
// String Methods Header

#pragma once

#include <string>
#include <vector>

void Strip(std::string &);
void LStrip(std::string &);
void RStrip(std::string &);
void CutCommentLine(std::string &);
void SplitString(std::vector<std::string> &, const std::string, const std::string);
bool StringBeginsWithPrefix(const std::string, const std::string);