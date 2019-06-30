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

#include <filesystem>
#include <string>
#include <XCommon.hpp>

namespace ray
{

/// @class DMaterialProperty
/// @brief Properties of outer material.
class DMaterialProperty final
{
public:
  using TPath = std::string;

  DVec3 mAmbientColor;
  DVec3 mDiffuseColor;
  DVec3 mSpecularColor;
  DVec3 mTransmitColor;
  DVec3 mEmissionColor;

  TReal mShininess;
  TReal mIor;
  TReal mDissolve; // 1 == opaque, 0 == fully transparent.

  TPath mAmbientTexture;
  TPath mDiffuseTexture;
  TPath mSpecularTexture;
  TPath mSpecularHightlightTexture;
  TPath mBumpTexture;
  TPath mDisplacementTexture;
  TPath mAlphaTexture;
};

} /// ::ray namespace