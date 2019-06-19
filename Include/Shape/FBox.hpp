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

#include <array>
#include <nlohmann/json_fwd.hpp>
#include <Math/Type/Shape/DBox.h>
#include <Expr/XEnumConversion.h>
#include <IHitable.hpp>
#include <XCommon.hpp>

namespace ray
{

/// @class FBox
/// @brief Box type.
class FBox final : public IHitable, public ::dy::math::DBox<TReal>
{
public:
  /// @brief Constructor type of FBox.
  struct PCtor final
  {
    enum EType { _1, _2, _3 };
    EXPR_INIT_ENUMTOTYPE(Type, EType);

    struct PType1 final { DVec3 mOrigin; std::array<TReal, 6> mLength; DVec3 mAngle; };
    struct PType2 final { DVec3 mOrigin, mLength, mAngle; };
    struct PType3 final { DVec3 mOrigin; TReal mLength; DVec3 mAngle; };
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_1, PType1);
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_2, PType2);
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_3, PType3);

    EType mCtorType = EType::_1;
    std::variant<PType1, PType2, PType3> mCtor;
  };

  FBox(const PCtor& ctor, IMaterial* mat);
  virtual ~FBox() = default;

  const DQuat& GetQuaternion() const noexcept { return this->mRotQuat; }

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<std::vector<TReal>> GetRayIntersectedTValues(const DRay& ray) const override final;

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FBox::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FBox::PCtor& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType2& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType3& oCtor);

} /// ::ray namespace
