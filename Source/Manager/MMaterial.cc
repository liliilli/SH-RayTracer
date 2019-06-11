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

#include <Manager/MMaterial.hpp>
#include <nlohmann/json.hpp>

#include <XCommon.hpp>
#include <XHelperJson.hpp>
#include <OldMaterial/FMatLambertian.hpp>
#include <OldMaterial/FMatDielectric.hpp>
#include <OldMaterial/FMatMetal.hpp>

namespace ray
{

ESuccess MMaterial::pfInitialize()
{
  return ESuccess::DY_SUCCESS;
}

ESuccess MMaterial::pfRelease()
{
  return ESuccess::DY_SUCCESS;
}

IMaterial* MMaterial::GetMaterial(const DMatId& id)
{
  const auto it = this->mContainer.find(id);
  if (it == this->mContainer.end()) { return nullptr; }

  return it->second.get();
}

std::optional<DMatId> MMaterial::AddOldMaterial_FMatLambertian(const nlohmann::json& json)
{
  auto ctor = json::GetValueFrom<FMatLambertian::PCtor>(json, "mat_detail");
  ctor.mId = ::dy::math::DUuid{true};

  this->mContainer.try_emplace(ctor.mId, std::make_unique<FMatLambertian>(ctor));
  return ctor.mId;
}

std::optional<DMatId> MMaterial::AddOldMaterial_FMatMetal(const nlohmann::json& json)
{
  auto ctor = json::GetValueFrom<FMatMetal::PCtor>(json, "mat_detail");
  ctor.mId = ::dy::math::DUuid{true};

  this->mContainer.try_emplace(ctor.mId, std::make_unique<FMatMetal>(ctor));
  return ctor.mId;
}

std::optional<DMatId> MMaterial::AddOldMaterial_FMatDielectric(const nlohmann::json& json)
{
  auto ctor = json::GetValueFrom<FMatDielectric::PCtor>(json, "mat_detail");
  ctor.mId = ::dy::math::DUuid{true};

  this->mContainer.try_emplace(ctor.mId, std::make_unique<FMatDielectric>(ctor));
  return ctor.mId;
}

} /// ::ray namespace