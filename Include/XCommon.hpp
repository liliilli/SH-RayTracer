#pragma once
///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <nlohmann/json_fwd.hpp>
#include <Math/Common/TGlobalTypes.h>
#include <Math/Type/Math/DVector2.h>
#include <Math/Type/Math/DVector3.h>
#include <Math/Type/Math/DQuat.h>
#include <Math/Type/Shape/DRay.h>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>
#include <Math/Type/Micellanous/DClamp.h>
#include <Math/Type/Micellanous/DBounds3D.h>
#include <Expr/FCmdArguments.h>
#include <Expr/ESuccess.h>

/// @def RAY_IF_VERBOSE_MODE
/// @define Check application is verbose mode on runtime.
#define RAY_IF_VERBOSE_MODE() \
  if (ray::sArguments->GetValueFrom<bool>('v') == true)

namespace ray
{

using TReal = ::dy::math::TReal;
using TI32 = ::dy::math::TI32;
using TI64 = ::dy::math::TI64;
using TU32 = ::dy::math::TU32;
using TIndex = ::dy::math::TIndex;

using DVec3 = ::dy::math::DVector3<TReal>;
using DIVec2 = ::dy::math::DVector2<TI32>;
using DUVec2 = ::dy::math::DVector2<TU32>;
using DIVec3 = ::dy::math::DVector3<TI32>;
using DVec2 = ::dy::math::DVector2<TReal>;
using DQuat = ::dy::math::DQuaternion<TReal>;
using DAABB = ::dy::math::DBounds3D<TReal>;

extern std::unique_ptr<::dy::expr::FCmdArguments> sArguments;

template <typename TType>
using DDynamicGrid2D = ::dy::math::DDynamicGrid2D<TType>;

template <TI64 start, TI64 end>
using DClamp = ::dy::math::DClamp<TReal, start, end>;

using DRay = ::dy::math::DRay<TReal>;
using ESuccess = ::dy::expr::EDySuccess;

/// @brief Add Default Command Arguments into command manager.
/// @param manager Command Argument Manager Reference.
void AddDefaultCommandArguments(::dy::expr::FCmdArguments& manager);

/// @brief Parse given argc, and argv into command argument manager.
/// @param manager Command Argument Manager Reference.
void ParseCommandArguments(::dy::expr::FCmdArguments& manager, int argc, char* argv[]);

/// @brief Print help log. (`--help`)
void PrintHelp(const ::dy::expr::FCmdArguments& manager);

/// @brief Print overall information.
/// @param manager Command Argument Manager Reference.
void PrintOverallInformation(const ::dy::expr::FCmdArguments& manager);

/// @brief Create image ppm with grid2d container.
/// @return If successful, return true. Otherwise, return false.
bool CreateImagePpm(const char* const path, DDynamicGrid2D<DIVec3>& container);

/// @brief Create image png with grid2d container.
/// @return If successful, return true. Otherwise, return false.
bool CreateImagePng(const char* const path, const DDynamicGrid2D<DIVec3>& container);

} /// ::ray namespace

namespace dy::math
{

void to_json(nlohmann::json& j, const ray::DVec3& p);
void from_json(const nlohmann::json& j, ray::DVec3& p);

void to_json(nlohmann::json& j, const ray::DUVec2& p);
void from_json(const nlohmann::json& j, ray::DUVec2& p);

} /// ::dy::math namespace