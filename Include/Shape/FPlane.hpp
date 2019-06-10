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
#include <Math/Type/Shape/DPlane.h>
#include <IHitable.hpp>
#include <XCommon.hpp>

namespace ray
{

/// @class FPlane
/// @brief Plane type.
class FPlane final : public IHitable, public ::dy::math::DPlane<TReal>
{
public:
  /// @brief Constructor type of FPlane.
  struct PCtor final
  {
    enum EType { _1, _2 };
    EXPR_INIT_ENUMTOTYPE(Type, EType);

    struct PType1 final { DVec3 mNormal; TReal mD; };
    struct PType2 final { DVec3 mPos1, mPos2, mPos3; };
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_1, PType1);
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_2, PType2);

    EType mCtorType = EType::_1;
    std::variant<PType1, PType2> mCtor;
  };

  /// @brief Construct DPlane with normalized vector and just d.
  FPlane(const DVec3& normal, TReal d, std::unique_ptr<IMaterial>&& mat);
  /// @brief Construct DPlane with (pos2 - pos1) and (pos3 - pos1) vector.
  FPlane(const DVec3& pos1, const DVec3& pos2, const DVec3& pos3, std::unique_ptr<IMaterial>&& mat);
  /// @brief Construct DPlane with FPlane::PCtor.
  FPlane(const FPlane::PCtor& arg, std::unique_ptr<IMaterial>&& mat);

  virtual ~FPlane() = default;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FPlane::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FPlane::PCtor& oCtor);
void from_json(const nlohmann::json& json, FPlane::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FPlane::PCtor::PType2& oCtor);

} /// ::ray namespace
