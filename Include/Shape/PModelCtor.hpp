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

#include <nlohmann/json_fwd.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>

namespace ray
{

/// @brief Constructor type of FModel.
struct PModelCtor final
{
  DVec3 mOrigin; 
  TReal mScale; 
  DVec3 mAngle; 
  std::string mModelResourceName;

  /// @brief Overwrite with given pctor instance and create new PCtor.
  PModelCtor Overwrite(const PModelCtor& pctor, const json::FExistanceList& list) const;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FModel::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, PModelCtor& oCtor);

/// @brief FModel specialized function.
template <> json::FExistanceList JsonCheckExistances<PModelCtor>(const nlohmann::json& json);

} /// ::ray namespace