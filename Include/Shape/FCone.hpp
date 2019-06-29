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
#include <Math/Type/Shape/DCone.h>
#include <Interface/IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>

namespace ray
{

/// @class FCone
/// @brief Cone type.
class FCone final : public IHitable, public ::dy::math::DCone<TReal>
{
public:
  /// @brief Constructor type of FCone.
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

  /// @brief Construct DCone with FCone::PCtor.
  FCone(const FCone::PCtor& arg, const IMaterial* mat);
  virtual ~FCone() = default;

  const DQuat& GetQuaternion() const noexcept { return this->mRotQuat; }

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const override final;
  
  /// @brief Diffuse scattering function.
  /// @param ray World space ray to intersect.
  /// @param t Forwarding value t for moving ray origin into surface approximately.
  /// @param normal Microfacet surface's given world-space normal vector.
  /// @return If scattered, return scattered result of diffused reflection.
  std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t, const DVec3& normal) const override final;

  /// @brief Get PCtor instance from instance, with given type value.
  /// @param type Type value.
  FCone::PCtor GetPCtor(FCone::PCtor::EType type) const noexcept;

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FCone::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FCone::PCtor& oCtor);
void from_json(const nlohmann::json& json, FCone::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FCone::PCtor::PType2& oCtor);

/// @brief FCone specialized function.
template <> json::FExistanceList JsonCheckExistances<FCone::PCtor>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FCone::PCtor::PType1>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FCone::PCtor::PType2>(const nlohmann::json& json);

} /// ::ray namespace
