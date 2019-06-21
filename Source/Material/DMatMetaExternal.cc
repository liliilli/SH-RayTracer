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

#include <Material/DMatMetaExternal.hpp>

namespace ray
{

DMatMetaExternal::DMatMetaExternal(const DMatId& id, const tinyobj::material_t& info)
  : mId { id }
{
  this->mProperty.mAmbientColor   = DVec3{info.ambient[0], info.ambient[1], info.ambient[2]};
  this->mProperty.mDiffuseColor   = DVec3{info.diffuse[0], info.diffuse[1], info.diffuse[2]};
  this->mProperty.mSpecularColor  = DVec3{info.specular[0], info.specular[1], info.specular[2]};
  this->mProperty.mTransmitColor  = DVec3{info.transmittance[0], info.transmittance[1], info.transmittance[2]};
  this->mProperty.mEmissionColor  = DVec3{info.emission[0], info.emission[1], info.emission[2]};

  this->mProperty.mShininess = info.shininess;
  this->mProperty.mIor       = info.ior;
  this->mProperty.mDissolve  = info.dissolve;

  this->mProperty.mAmbientTexture       = info.ambient_texname;
  this->mProperty.mDiffuseTexture       = info.diffuse_texname;
  this->mProperty.mSpecularTexture      = info.specular_texname;
  this->mProperty.mSpecularHightlightTexture = info.specular_highlight_texname;
  this->mProperty.mBumpTexture          = info.bump_texname;
  this->mProperty.mDisplacementTexture  = info.displacement_texname;
  this->mProperty.mAlphaTexture         = info.alpha_texname;
}

const DMaterialProperty& DMatMetaExternal::GetMaterialProperty() const noexcept
{
  return this->mProperty;
}

const DMatId& DMatMetaExternal::GetId() const noexcept
{
  return this->mId;
}

} /// ::ray namespace