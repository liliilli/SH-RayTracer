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
#include <variant>
#include <Math/Type/Micellanous/DUuid.h>

namespace ray
{

/// @class DModelId
/// @brief Model ID Identification type.
class DModelId final
{
public:
  /// @brief ID type
  enum EType { None, UUID, String };

  DModelId() = default;
  DModelId(const ::dy::math::DUuid& uuidId);
  DModelId(const std::string& stringId);

  /// @brief Check this ID instance has valid ID.
  bool HasId() const noexcept;
  /// @brief Check this ID instance has UUID ID.
  bool IsUUID() const noexcept;
  /// @brief Check this ID instance has string ID.
  bool IsString() const noexcept;

  /// @brief Get string of id.
  /// If instance has no id, just return empty string.
  std::string ToString() const noexcept;

private:
  /// Default value is std::monostate, unless DModelId construction is held by customized ctors.
  std::variant<std::monostate, ::dy::math::DUuid, std::string> mId;
  EType mIdType = EType::None;

  friend struct std::hash<::ray::DModelId>;
  friend bool operator==(const DModelId& lhs, const DModelId& rhs);
  friend bool operator!=(const DModelId& lhs, const DModelId& rhs);
};

bool operator==(const DModelId& lhs, const DModelId& rhs);
bool operator!=(const DModelId& lhs, const DModelId& rhs);

} /// ::ray namespace

namespace std
{

template<> 
struct hash<::ray::DModelId>
{
  using argument_type = ::ray::DModelId;
  using result_type = std::size_t;
  result_type operator()(argument_type const& s) const noexcept;
};

} /// ::std namespace