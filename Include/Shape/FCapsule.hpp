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

#include <variant>
#include <nlohmann/json_fwd.hpp>
#include <Math/Type/Shape/DCapsule.h>
#include <IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>

namespace ray
{

/// @class FCapsule
/// @brief Capsule type.
class FCapsule final : public IHitable, public ::dy::math::DCapsule<TReal>
{
public:
  /// @brief Constructor type of FCapsule.
  struct PCtor final
  {
    enum EType { _1, _2 };
    EXPR_INIT_ENUMTOTYPE(Type, EType);

    struct PType1 final 
    { 
      DVec3 mOrigin; TReal mHeight; TReal mRadius; DVec3 mAngle; 
      PType1 Overwrite(const PType1& pctor, const json::FExistanceList& list) const;
    };
    struct PType2 final 
    {
      DVec3 mOrigin; TReal mLength; DVec3 mAngle; 
      PType2 Overwrite(const PType2& pctor, const json::FExistanceList& list) const;
    };
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_1, PType1);
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_2, PType2);

    EType mCtorType = EType::_1;
    std::variant<PType1, PType2> mCtor;

    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  /// @brief Construct DCapsule with FCapsule::PCtor.
  FCapsule(const FCapsule::PCtor& arg, IMaterial* mat);
  virtual ~FCapsule() = default;

  const DQuat& GetQuaternion() const noexcept { return this->mRotQuat; }

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<std::vector<TReal>> GetRayIntersectedTValues(const DRay& ray) const override final;

  /// @brief Get PCtor instance from instance, with given type value.
  /// @param type Type value.
  FCapsule::PCtor GetPCtor(FCapsule::PCtor::EType type) const noexcept;

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FCapsule::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FCapsule::PCtor& oCtor);
void from_json(const nlohmann::json& json, FCapsule::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FCapsule::PCtor::PType2& oCtor);

/// @brief FCapsule specialized function.
template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor::PType1>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor::PType2>(const nlohmann::json& json);

} /// ::ray namespace
