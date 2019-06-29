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

#include <vector>
#include <optional>
#include <filesystem>

namespace ray
{
 
/// @brief Get buffer from file path.
/// @param iFilePath File path
/// @return If successful to read file from iFilePath, return buffer chunk.
[[nodiscard]] std::optional<std::vector<char>> GetBufferFromFile(const std::string& iFilePath);

/// @brief Get buffer from file path.
/// @param iFilePath More safe file path
/// @return If successful to read file from iFilePath, return buffer chunk.
[[nodiscard]] std::optional<std::vector<char>> GetBufferFromFile(const std::filesystem::path& iFilePath);

} /// ::ray namespace
