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
