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

namespace ray::json
{

/// @enum EExistance
/// @brief Json member variable existance flag.
enum class EExistance
{
  NotExist,
  Exist
};

/// @brief Existance list.
using FExistanceList = std::vector<EExistance>;

/// @brief Helper function that overwrite original value with newValue, when exist value is Exist.
/// @tparam TType The arbitary type for checking.
/// @param exist Existance value.
/// @param original Original value
/// @param newValue New value to swap when exist is `::Exist`.
template <typename TType>
inline void OverwriteWhenExist(EExistance exist, TType& original, const TType& newValue)
{
  using json::EExistance;
  if (exist == EExistance::NotExist) { return; }
  if (original != newValue) { original = newValue; }
  return;
}

} /// ::ray::json namespace