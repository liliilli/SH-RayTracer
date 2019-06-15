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
#include <IHitable.hpp>
#include <XCommon.hpp>

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

    struct PType1 final { DVec3 mOrigin; TReal mHeight; TReal mRadius; DVec3 mAngle; };
    struct PType2 final { DVec3 mOrigin; TReal mLength; DVec3 mAngle; };
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_1, PType1);
    EXPR_SET_ENUMTOTYPE_CONVERSION(Type, EType::_2, PType2);

    EType mCtorType = EType::_1;
    std::variant<PType1, PType2> mCtor;
  };

  /// @brief Construct DCone with FCone::PCtor.
  FCone(const FCone::PCtor& arg, IMaterial* mat);
  virtual ~FCone() = default;

  DQuat GetQuaternion() const noexcept { return this->mRotQuat; }

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FCone::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FCone::PCtor& oCtor);
void from_json(const nlohmann::json& json, FCone::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FCone::PCtor::PType2& oCtor);

} /// ::ray namespace
