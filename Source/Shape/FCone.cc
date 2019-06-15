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

#include <Shape/FCone.hpp>
#include <nlohmann/json.hpp>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, FCone::PCtor& oCtor)
{
  /// Type 1 ("type" : 1)
  /// Type 2 ("type" : 2)
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1:
  {
    oCtor.mCtorType = FCone::PCtor::_1;
    oCtor.mCtor = json.get<FCone::PCtor::PType1>();
  } break;
  case 2:
  {
    oCtor.mCtorType = FCone::PCtor::_2;
    oCtor.mCtor = json.get<FCone::PCtor::PType2>();
  } break;
  default: break;
  }
}

void from_json(const nlohmann::json& json, FCone::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "pos": [,,], "height": 2, "radius": 1, "angle": [,,] }
  /// }
  assert(json::HasJsonKey(json, "pos") == true); 
  assert(json::HasJsonKey(json, "height") == true);
  assert(json::HasJsonKey(json, "radius") == true);
  assert(json::HasJsonKey(json, "angle") == true);
  
  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mAngle  = json::GetValueFrom<DVec3>(json, "angle");
  oCtor.mHeight = json::GetValueFrom<TReal>(json, "height");
  oCtor.mRadius = json::GetValueFrom<TReal>(json, "radius");
}

void from_json(const nlohmann::json& json, FCone::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos": [,,], "length": 1, "angle": [,,] }
  /// }
  assert(json::HasJsonKey(json, "pos") == true); 
  assert(json::HasJsonKey(json, "length") == true);
  assert(json::HasJsonKey(json, "angle") == true);

  oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos");
  oCtor.mAngle  = json::GetValueFrom<DVec3>(json, "angle");
  oCtor.mLength = json::GetValueFrom<TReal>(json, "length");
}

FCone::FCone(const FCone::PCtor& arg, IMaterial* mat)
  : IHitable{ EShapeType::Cone, mat },
    ::dy::math::DCone<TReal>{DVec3{}, 1}
{ 
  switch (arg.mCtorType)
  {
  case FCone::PCtor::_1:
  {
    const auto ctor = std::get<FCone::PCtor::PType1>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mHeight   = ctor.mHeight;
    this->mRadius   = ctor.mRadius;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FCone::PCtor::_2:
  {
    const auto ctor = std::get<FCone::PCtor::PType2>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mHeight   = ctor.mLength;
    this->mRadius   = ctor.mLength;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  }
}

} /// ::ray namespace
