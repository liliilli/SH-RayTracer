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

#include <Shape/FBox.hpp>

namespace ray
{

FBox::FBox(const PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Box, mat},
    DBox{}
{
  switch (arg.mCtorType)
  {
  case FBox::PCtor::_1:
  {
    const auto ctor = std::get<FBox::PCtor::PType1>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = ctor.mLength;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_2:
  {
    const auto ctor = std::get<FBox::PCtor::PType2>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = {
      ctor.mLength[0], ctor.mLength[1], ctor.mLength[2], 
      ctor.mLength[3], ctor.mLength[4], ctor.mLength[5]};
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_3:
  {
    const auto ctor = std::get<FBox::PCtor::PType3>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    std::fill(this->mLength.begin(), this->mLength.end(), ctor.mLength);
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  }
}

} /// ::ray namespace