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

#include <Helper/XHelperRegex.hpp>
#include <regex>

namespace ray::regex
{

std::optional<std::vector<std::string>> GetMatches(const std::string& iBuffer, const std::string& iRegex)
{
  // If buffer is empty, just return empty list.
  if (iBuffer.empty() == true) { return std::nullopt; }

  // Set regex instance and match instance for searching & binding result.
  const std::regex parentMatchRegex(iRegex);
  std::smatch parentSpecifierMatch;

  // Matching & Insertion
  std::vector<std::string> result {};

  std::string buffer = iBuffer;
  while (std::regex_search(buffer, parentSpecifierMatch, parentMatchRegex) == true) 
  { 
    for (size_t i = 1, size = parentSpecifierMatch.size(); i < size; ++i)
    {
      result.emplace_back(parentSpecifierMatch[i].str());
    }
    buffer = parentSpecifierMatch.suffix();
  }

  return result;
}

bool IsMatched(const std::string& iString, const std::string& iRegex)
{
  if (iString.empty() == true) { return false; }

  static const std::regex regexPattern(iRegex);
  return std::regex_match(iString, regexPattern);
}

} /// ::ray::regex namespace
