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

#include <Shape/FModelPrefab.hpp>
#include <nlohmann/json.hpp>
#include <Helper/XHelperJson.hpp>
#include <Manager/MModel.hpp>

namespace ray
{

FModelPrefab::FModelPrefab(const PModelCtor& ctor, IMaterial* mat)
  : IHitable{EShapeType::Model, mat},
    mOrigin { ctor.mOrigin },
    mScale { ctor.mScale },
    mRotQuat { ctor.mAngle },
    mModelId { ctor.mModelResourceName }
{ }

PModelCtor FModelPrefab::GetPCtor() const noexcept
{
  PModelCtor result;
  result.mOrigin  = this->GetOrigin();
  result.mScale   = this->GetScale();
  result.mAngle   = this->GetQuaternion().ToDegrees();
  result.mModelResourceName = this->mModelId.ToString();

  return result;
}

const DVec3& FModelPrefab::GetOrigin() const noexcept
{
  return this->mOrigin;
}

const TReal& FModelPrefab::GetScale() const noexcept
{
  return this->mScale;
}

const DQuat& FModelPrefab::GetQuaternion() const noexcept
{
  return this->mRotQuat;  
}

} /// ::ray namespace