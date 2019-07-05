#pragma once
///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <string>
#include <vector>
#include <optional>

namespace ray::regex
{

/// @brief Return regex matched information keyword recursively.
/// @param iBuffer String buffer.
/// @param IRegex Regex pattern string.
/// @return If matched, get matches as a list.
std::optional<std::vector<std::string>> GetMatches(const std::string& iBuffer, const std::string& iRegex);

/// @brief Check given specifier is matched to given regular expression.
bool IsMatched(const std::string& iString, const std::string& iRegex);

} /// ::ray::json namespace
