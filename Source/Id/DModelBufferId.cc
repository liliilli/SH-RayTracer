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

#include <Id/DModelBufferId.hpp>

namespace ray
{

DModelBufferId::DModelBufferId(const::dy::math::DUuid& uuidId)
  : mId { uuidId },
    mIdType { EType::UUID }
{ }

DModelBufferId::DModelBufferId(const std::string& stringId)
  : mId { stringId },
    mIdType { EType::String }
{ }

bool DModelBufferId::HasId() const noexcept
{
  return this->mIdType != EType::None;
}

bool DModelBufferId::IsUUID() const noexcept
{
  return this->mIdType == EType::UUID;
}

bool DModelBufferId::IsString() const noexcept
{
  return this->mIdType == EType::String;
}

std::string DModelBufferId::ToString() const noexcept
{
  switch (this->mIdType)
  {
  case EType::None:   return "";
  case EType::String: return std::get<std::string>(this->mId);
  case EType::UUID:   return std::get<::dy::math::DUuid>(this->mId).ToString();
  default: return "";
  }
}

bool operator==(const DModelBufferId& lhs, const DModelBufferId& rhs)
{
  if (lhs.mIdType != rhs.mIdType) { return false; }
  return lhs.mId == rhs.mId;
}

bool operator!=(const DModelBufferId& lhs, const DModelBufferId& rhs)
{
  return !(lhs == rhs);
}

} /// ::ray namespace

std::hash<ray::DModelBufferId>::result_type 
std::hash<ray::DModelBufferId>::operator()(const argument_type& s) const noexcept
{
  return std::hash<decltype(argument_type::mId)>{}(s.mId);
}