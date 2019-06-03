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

/// Header file
#include <XHelperJson.hpp>

#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <XHelperIO.hpp>

namespace ray::json
{

bool HasJsonKey(const nlohmann::json& json, const std::string& key) noexcept
{
  return json.find(key) != json.end();
}

std::optional<nlohmann::json> GetAtlasFromFile(const std::string& filePath) noexcept
{
  const auto optBuffer = GetBufferFromFile(filePath);
  if (optBuffer.has_value() == false) { return std::nullopt; }

  try 
  {
    return nlohmann::json::parse(*optBuffer);
  }
  catch (nlohmann::json::parse_error& e)
  {
    std::cerr << e.what() << '\n';
    return std::nullopt;
  }
}

std::optional<nlohmann::json> 
GetAtlasFromFile(const std::filesystem::path& iFilePath) noexcept
{
  return GetAtlasFromFile(iFilePath.string());
}

std::optional<nlohmann::json> 
GetAtlasFromString(const std::string& iSerializedString) noexcept
{
  try
  {
    auto result = nlohmann::json::parse(iSerializedString);
    return result;
  }
  catch (nlohmann::json::parse_error& e)
  {
    std::cerr << e.what() << '\n';
    return std::nullopt;
  }
}

} /// ::ray::json namespace