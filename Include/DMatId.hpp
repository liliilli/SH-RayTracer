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

/// @class DMatId
/// @brief Material ID Identification type.
class DMatId final
{
public:
  /// @brief ID type
  enum EType { None, UUID, String };

  DMatId() = default;
  DMatId(const ::dy::math::DUuid& uuidId);
  DMatId(const std::string& stringId);

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
  /// Default value is std::monostate, unless DMatId construction is held by customized ctors.
  std::variant<std::monostate, ::dy::math::DUuid, std::string> mId;
  EType mIdType = EType::None;

  friend struct std::hash<::ray::DMatId>;
  friend bool operator==(const DMatId& lhs, const DMatId& rhs);
  friend bool operator!=(const DMatId& lhs, const DMatId& rhs);
};

bool operator==(const DMatId& lhs, const DMatId& rhs);
bool operator!=(const DMatId& lhs, const DMatId& rhs);

} /// ::ray namespace

namespace std
{

template<> 
struct hash<::ray::DMatId>
{
  using argument_type = ::ray::DMatId;
  using result_type = std::size_t;
  result_type operator()(argument_type const& s) const noexcept;
};

} /// ::std namespace