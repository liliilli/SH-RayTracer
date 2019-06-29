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

#include <Helper/XHelperIO.hpp>

namespace ray
{

std::optional<std::vector<char>> 
GetBufferFromFile(const std::string& iFilePath)
{
  // ReSharper disable CppDeprecatedEntity
  std::FILE* fd = std::fopen(iFilePath.c_str(), "r");
  if(fd == nullptr) 
  {
    return std::nullopt;
  }

  // ReSharper restore CppDeprecatedEntity
  std::fseek(fd, 0, SEEK_END);
  const auto size = ftell(fd);
  std::fseek(fd, 0, SEEK_SET);

  std::vector<char> buffer(size + 1);
  [[maybe_unused]] const auto readSize = 
	  std::fread(buffer.data(), sizeof(char), size, fd);
  
  std::fclose(fd);

  return std::move(buffer);
}

std::optional<std::vector<char>>  
GetBufferFromFile(const std::filesystem::path& iFilePath)
{
  return GetBufferFromFile(iFilePath.string());
}

} /// ::ray namespace
