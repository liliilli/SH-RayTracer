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

#include <OldMaterial/FMatDielectric.hpp>

#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XGraphicsMath.h>
#include <Math/Utility/XShapeMath.h>
#include <Math/Utility/XRandom.h>
#include <Helper/XHelperJson.hpp>
#include <Manager/MScene.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FMatDielectric::PCtor& oCtor)
{
  assert(json::HasJsonKey(json, "ior") == true);
  assert(json::HasJsonKey(json, "color") == true);

  oCtor.mIor = json::GetValueFrom<TReal>(json, "ior");
  oCtor.mColor = json::GetValueFrom<DVec3>(json, "color");
}

std::optional<PScatterResult> 
FMatDielectric::Scatter(const DRay& intersectedRay, const DVec3& normal) const
{
  using ::dy::math::GetClosestTValueOf;
  using ::dy::math::RandomVector3Length;
  using ::dy::math::RandomUniformReal;
  using ::dy::math::Dot;
  using ::dy::math::Refract;
  using ::dy::math::Reflect;
  using ::dy::math::Schlick;

  // Compare IOR with scene.
  const auto incidentNormal = intersectedRay.GetDirection() * -1.0f;
  const auto cosTheta1  = Dot(incidentNormal, normal);
  const auto sceneIor   = EXPR_SGT(MScene).GetSceneIOR();

  if (cosTheta1 > 0)
  {
    // If from outside to inside of object, IOR factor is inside/outside.
    const auto optRefractDir = Refract(sceneIor, this->mIor(), incidentNormal, normal);
    if (optRefractDir.has_value() == false)
    {
      const auto reflectDir = Reflect(incidentNormal, normal);
      return PScatterResult{reflectDir, DVec3{1}, true};
    }
    else
    {
      const auto presnelFactor = Schlick(sceneIor, this->mIor(), incidentNormal, normal);
      if (RandomUniformReal(0.f, 1.f) < presnelFactor)
      {
        const auto reflectDir = Reflect(incidentNormal, normal);
        return PScatterResult{reflectDir, this->mColor, true};
      }
      else
      {
        assert(Dot(*optRefractDir, normal * -1.0f) >= 0);
        return PScatterResult{*optRefractDir, this->mColor, true};
      }
    }
  }
  else
  {
    // If from inside to outside of object, IOR factor is inside/outside.
    const auto optRefractDir = Refract(this->mIor(), sceneIor, incidentNormal, normal * -1.0f);
    if (optRefractDir.has_value() == false)
    {
      const auto reflectDir = Reflect(incidentNormal, normal * -1.0f);
      return PScatterResult{reflectDir, DVec3{1}, true};
    }
    else
    {
      const auto presnelFactor = Schlick(this->mIor(), sceneIor, incidentNormal, normal);
      if (RandomUniformReal(0.f, 1.f) < presnelFactor)
      {
        const auto reflectDir = Reflect(incidentNormal, normal * -1.0f);
        return PScatterResult{reflectDir, this->mColor, true};
      }
      else
      {
        assert(Dot(*optRefractDir, normal) >= 0);
        return PScatterResult{*optRefractDir, this->mColor, true};
      }
    }
  }

#if 0
  auto incidentIor  = TReal();
  auto refractIor   = TReal();
  const auto incidentNor = intersectedRay.GetDirection() * -1.0f;

  DVec3 outNormal;
  TReal reflectProb = TReal();
  if (Dot(incidentNor, normal) < 0) // When inside => outside
  {
    outNormal = normal * -1.0f;
    incidentIor = this->mIor;
    refractIor  = 1.0f;
  }
  else // When outside => inside
  {
    outNormal = normal;
    incidentIor = 1.0f;
    refractIor  = this->mIor;
  }

  auto optRefract = Refract(incidentIor, refractIor, incidentNor, outNormal);
  if (optRefract.has_value() == false)
  {
    reflectProb = 1.0f;
  }
  else
  {
    reflectProb = Schlick(incidentIor, refractIor, incidentNor, outNormal);
    return PScatterResult {*optRefract, DVec3{1}, true};
  }

  if (::dy::math::RandomUniformReal(0.0f, 1.0f) < reflectProb)
  {
    const auto refDir = Reflect(incidentNor, normal);
    return PScatterResult {refDir, DVec3{1}, true};
  }
  else
  {
    assert(Dot(incidentNor * -1.0f, *optRefract) >= 0);
    return PScatterResult {*optRefract, DVec3{1}, true};
  }
#endif
}

} /// ::ray namespace